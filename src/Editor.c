#include "Editor.h"
#include "Database.h"
#include "Filesystem.h"

extern DataFile gAppIcon16;
extern DataFile gAppIcon32;
extern DataFile gAppIcon64;
extern DataFile gAppIcon128;
extern DataFile gAppIcon256;
extern DataFile gCursor_ArrowUp;
extern DataFile gCursor_ArrowDown;
extern DataFile gCursor_ArrowLeft;
extern DataFile gCursor_ArrowRight;
extern DataFile gCursor_ArrowHorizontal;
extern DataFile gCursor_ArrowVertical;
extern DataFile gCursor_Crosshair;
extern DataFile gCursor_Empty;

Image sTexelFile[5];
const DataFile* sIconData[5] = {
    &gAppIcon16,
    &gAppIcon32,
    &gAppIcon64,
    &gAppIcon128,
    &gAppIcon256,
};
GLFWimage sIconImage[5] = {
    { 16,  16  },
    { 32,  32  },
    { 64,  64  },
    { 128, 128 },
    { 256, 256 },
};

////////////////////////////////////////////////////////////////////////////////

void* NewMtx() {
    return Matrix_ToMtx(n64_graph_alloc(sizeof(Mtx)));
}

static Editor* sEditor;

////////////////////////////////////////////////////////////////////////////////

VectorGfx gVecGfx_EyeOpen;

static void Editor_InitIcons(Editor* editor) {
    extern DataFile gIcon_EyeOpen;

    VectorGfx_New(&gVecGfx_EyeOpen, gIcon_EyeOpen.data, 10.0f);
}

Editor* GetEditor(void) {
    return sEditor;
}

////////////////////////////////////////////////////////////////////////////////

typedef struct {
    GeoGrid grid;
    Split*  split[4];
    char*   path;
    
    struct SidePanel {
        ElPanel     panel;
        ElContainer container;
        Arli list;
    } volumes, bookmarks;
    
    struct SearchPanel {
        ElTextbox path;
        ElTextbox search;
    } search;
    
    struct ConfirmPanel {
        ElTextbox textbox;
        ElButton  confirm, cancel;
    } confirm;
    
    enum FileDialogAction {
        FILE_DIALOG_OPEN_FILE,
        FILE_DIALOG_SAVE_FILE,
        FILE_DIALOG_FOLDER,
        FILE_DIALOG_MAX,
    } action;
} FileDialog;

////////////////////////////////////////////////////////////////////////////////

static void FileDialog_SidePanel(FileDialog* this, ContextMenu* context, Rect mainRect, void* vg) {
    struct SidePanel* volumes = &this->volumes;
    struct SidePanel* bookmarks = &this->bookmarks;
    
    Gfx_DrawRounderRect(vg, mainRect, Theme_GetColor(THEME_BASE, 255, 1.0f));
    
    if (Element_Box(BOX_START, &volumes->panel, "Volumes")) {
        Element_Row(&volumes->container, 1.0f);
        
        Element_Container(&volumes->container);
    }
    Element_Box(BOX_END, &volumes->panel);
    
    if (Element_Box(BOX_START, &bookmarks->panel, "Bookmarks")) {
        Element_Row(&bookmarks->container, 1.0f);
        
        Element_Container(&bookmarks->container);
    }
    Element_Box(BOX_END, &bookmarks->panel);
}

static void FileDialog_FilePanel(FileDialog* this, ContextMenu* context, Rect mainRect, void* vg) {
    
}

static void FileDialog_SearchPanel(FileDialog* this, ContextMenu* context, Rect mainRect, void* vg) {
    struct SearchPanel* search = &this->search;
    
    Element_Row(&search->path, 0.75f, &search->search, 0.25f);
    Element_Textbox(&search->path);
    Element_Textbox(&search->search);
}

static void FileDialog_BottomPanel(FileDialog* this, ContextMenu* context, Rect mainRect, void* vg) {
    struct ConfirmPanel* confirm = &this->confirm;
    
    Element_Row(&confirm->textbox, 0.60f, &confirm->confirm, 0.20f, &confirm->cancel, 0.20f);
    Element_Textbox(&confirm->textbox);
    
    if (Element_Button(&confirm->confirm))
        context->state.setCondition = 1;
    
    if (Element_Button(&confirm->cancel))
        context->state.setCondition = -1;
}

////////////////////////////////////////////////////////////////////////////////

static inline Toml* FileDialog_OpenToml() {
    const char* file = x_fmt("%sfile_dialog.toml", sys_appdata());
    static Toml toml;
    
    toml = Toml_New();
    
    if (sys_stat(file))
        Toml_Load(&toml, file);
    
    return &toml;
}

static inline void FileDialog_SaveToml(Toml* toml) {
    const char* file = x_fmt("%sfile_dialog.toml", sys_appdata());
    
    Toml_Save(toml, file);
}

static void FileDialog_LoadConfig(FileDialog* this) {
    Toml* toml = FileDialog_OpenToml();
    
    for (char i = 'A'; i <= 'Z'; i++) {
        char volume[128] = {};
        
        xl_snprintf(volume, 128, "%c:/", i);
        
        if (sys_isdir(volume)) {
            xl_snprintf(volume, 128, "%s (%c:)", sys_volumename(volume), i);
            
            Arli_Add(&this->volumes.list, volume);
            info("%s", volume);
        }
    }
    
    char msg[][128] = {
        "Master of Time",
        "The Sealed Palace",
        "The Missing Link",
        "Nimpize Adventure",
    };
    
    for (int i = 0; i < ArrCount(msg); i++)
        Arli_Add(&this->bookmarks.list, msg[i]);
    
    Toml_Free(toml);
}

static void FileDialog_SaveConfig(FileDialog* this) {
    Toml* toml = FileDialog_OpenToml();
    
    FileDialog_SaveToml(toml);
    Toml_Free(toml);
}

////////////////////////////////////////////////////////////////////////////////

static void FileDialog_Init(GeoGrid* mainGrid, ContextMenu* context) {
    FileDialog* this = context->udata;
    struct SidePanel* volumes = &this->volumes;
    struct SidePanel* bookmarks = &this->bookmarks;
    struct SearchPanel* search = &this->search;
    struct ConfirmPanel* confirm = &this->confirm;
    
    for (int i = 0; i < ArrCount(this->split); i++)
        this->split[i] = new(Split);
    this->path = new(char[128]);
    
    volumes->list = Arli_New(char[64]);
    bookmarks->list = Arli_New(char[64]);
    Arli_SetElemNameCallback(&volumes->list, Arli_StringCallback);
    Arli_SetElemNameCallback(&bookmarks->list, Arli_StringCallback);
    
    this->action = (int)context->element;
    context->state.offsetOriginRect = false;
    context->state.widthAdjustment = false;
    context->state.distanceCheck = false;
    context->state.maximize = true;
    mainGrid->state.cullSplits = 1;
    
    GeoGrid_Init(&this->grid, &GetEditor()->app, NULL);
    
    const char* message[FILE_DIALOG_MAX] = {
        [FILE_DIALOG_OPEN_FILE] = "Open",
        [FILE_DIALOG_SAVE_FILE] = "Save",
        [FILE_DIALOG_FOLDER] = "Select",
    };
    
    Element_Button_SetProperties(&confirm->confirm, message[this->action], false, false);
    Element_Button_SetProperties(&confirm->cancel, "Cancel", false, false);
    Element_Container_SetArli(&volumes->container, &volumes->list, 5);
    Element_Container_SetArli(&bookmarks->container, &bookmarks->list, 5);
    volumes->container.align = NVG_ALIGN_LEFT;
    volumes->container.controller = true;
    volumes->container.stretch = true;
    bookmarks->container.align = NVG_ALIGN_LEFT;
    bookmarks->container.controller = true;
    bookmarks->container.stretch = true;
    
    search->path.align = NVG_ALIGN_LEFT;
    search->search.align = NVG_ALIGN_LEFT;
    confirm->textbox.align = NVG_ALIGN_LEFT;
    confirm->confirm.align = confirm->cancel.align = NVG_ALIGN_CENTER;
    confirm->confirm.element.colOvrdBase = THEME_PRIM;
    
    FileDialog_LoadConfig(this);
}

static void FileDialog_Destroy(GeoGrid* mainGrid, ContextMenu* context) {
    FileDialog* this = context->udata;
    struct SidePanel* volumes = &this->volumes;
    struct SidePanel* bookmarks = &this->bookmarks;
    
    // struct SearchPanel* search = &this->search;
    // struct ConfirmPanel* confirm = &this->confirm;
    
    mainGrid->state.cullSplits = false;
    FileDialog_SaveConfig(this);
    
    Arli_Free(&volumes->list);
    Arli_Free(&bookmarks->list);
    
    vfree(this->path);
    for (int i = 0; i < ArrCount(this->split); i++)
        vfree(this->split[i]);
}

static void FileDialog_Draw(GeoGrid* mainGrid, ContextMenu* context) {
    FileDialog* this = context->udata;
    void* vg = mainGrid->vg;
    Rect r;
    int sliceA = context->rect.w * 0.25f;
    int sliceB = context->rect.w - sliceA;
    int height = SPLIT_TEXT_H + SPLIT_ELEM_X_PADDING * 4;
    
    r = Rect_ShrinkX(context->rect, -sliceB);
    _log("FileDialow: SidePanel");
    DummySplit_Push(&this->grid, this->split[0], r); {
        r.w--;
        Gfx_DrawRounderOutline(vg, r, Theme_GetColor(THEME_BASE, 255, 1.5f));
        
        FileDialog_SidePanel(this, context, this->split[0]->rect, vg);
    } DummySplit_Pop(&this->grid, this->split[0]);
    
    r = Rect_ShrinkX(context->rect, sliceA);
    r = Rect_ShrinkY(r, (context->rect.h - height));
    _log("FileDialow: SearchPanel");
    DummySplit_Push(&this->grid, this->split[1], r); {
        r.h--;
        Gfx_DrawRounderOutline(vg, r, Theme_GetColor(THEME_BASE, 255, 1.5f));
        
        FileDialog_SearchPanel(this, context, this->split[1]->rect, vg);
    } DummySplit_Pop(&this->grid, this->split[1]);
    
    r = Rect_ShrinkX(context->rect, sliceA);
    r = Rect_ShrinkY(r, -height);
    r = Rect_ShrinkY(r, height);
    _log("FileDialow: FilePanel");
    DummySplit_Push(&this->grid, this->split[2], r); {
        r.h--;
        Gfx_DrawRounderOutline(vg, r, Theme_GetColor(THEME_BASE, 255, 1.5f));
        
        FileDialog_FilePanel(this, context, this->split[2]->rect, vg);
    } DummySplit_Pop(&this->grid, this->split[2]);
    
    r = Rect_ShrinkX(context->rect, sliceA);
    r = Rect_ShrinkY(r, -(context->rect.h - height));
    _log("FileDialow: BottomPanel");
    DummySplit_Push(&this->grid, this->split[3], r); {
        Gfx_DrawRounderOutline(vg, r, Theme_GetColor(THEME_BASE, 255, 1.5f));
        
        FileDialog_BottomPanel(this, context, this->split[3]->rect, vg);
    } DummySplit_Pop(&this->grid, this->split[3]);
}

////////////////////////////////////////////////////////////////////////////////

typedef struct {
    ElCombo    fileMenu;
    ElCombo    optionsMenu;
    FileDialog fileDialog;
} Header;

static void Header_Init(Header* this, Editor* editor) {
    Element_Combo_SetArli(&this->fileMenu, SceneDatabase_GetList(MENUDATA_FILE));
    this->fileMenu.align = NVG_ALIGN_CENTER;

    Element_Combo_SetArli(&this->optionsMenu, SceneDatabase_GetList(MENUDATA_OPTIONS));
    this->optionsMenu.align = NVG_ALIGN_CENTER;
    
}

static void Header_Update(void* pass, void* ____null____, Split* split) {
    Editor* editor = pass;
    Header* this = editor->head[0];
    Rect r = { 0, 0, UnfoldVec2(editor->app.wdim) };
    
    this->fileMenu.title = SceneDatabase_GetName(MENUDATA_FILE);
    this->optionsMenu.title = SceneDatabase_GetName(MENUDATA_OPTIONS);
    Element_Header(NULL, 8, &this->fileMenu, 64, &this->optionsMenu, 64);
    
    switch (Element_Combo(&this->fileMenu)) {
        case 0:
            ContextMenu_Custom(&editor->geo, &this->fileDialog, (void*)FILE_DIALOG_OPEN_FILE, FileDialog_Init, FileDialog_Draw, FileDialog_Destroy, r);
            break;
        case 1:
            ContextMenu_Custom(&editor->geo, &this->fileDialog, (void*)FILE_DIALOG_SAVE_FILE, FileDialog_Init, FileDialog_Draw, FileDialog_Destroy, r);
            break;
        case 2:
            ContextMenu_Custom(&editor->geo, &this->fileDialog, (void*)FILE_DIALOG_SAVE_FILE, FileDialog_Init, FileDialog_Draw, FileDialog_Destroy, r);
            break;
    }
    
    if (Element_Combo(&this->optionsMenu) > -1) {
    }
}

////////////////////////////////////////////////////////////////////////////////

static void Editor_DropCallback_HandleFileArg(char* file, List* roomList, char** currentSceneFile) {
    if (striend(file, ".zroom") || striend(file, ".zmap")) {
        List_Add(roomList, file);
    } else if (striend(file, ".zscene")) {
        vfree(*currentSceneFile);
        *currentSceneFile = strdup(file);
    }
}

static void Editor_DropCallback(GLFWwindow* window, s32 count, char* item[]) {
    Editor* editor = GET_CONTEXT(window);
    
    // Scene & Rooms to load 
    char* currentSceneFile = NULL;
    List roomList = List_New();

    time_start(11);

    // Check args for single scenes/rooms or directories to scan
    for (var i = 0; i < count; i++) {
        char* file = item[i];

        if(FsIsDirectory(file)) {
            var scanResult = FsScanDir(file);
            for(int i=0; i<scanResult.count; ++i) {
                Editor_DropCallback_HandleFileArg(scanResult.fileNames[i], &roomList, &currentSceneFile);
            }
            FsScanDir_Free(scanResult);
        } else {
            Editor_DropCallback_HandleFileArg(file, &roomList, &currentSceneFile);
        }
    }
    
    if (roomList.num == 0 || !currentSceneFile) {
        vfree(currentSceneFile);
        List_Free(&roomList);
        return;
    }

    List_Sort(&roomList);
    
    info("" PRNT_YELW "LOADING: " PRNT_BLUE "%s", currentSceneFile);
    if (editor->scene.segment) {
        Gizmo_UnselectAll(&editor->gizmo);
        Actor_UnselectAll(&editor->scene, NULL);
        Scene_Free(&editor->scene);
        Scene_Init(&editor->scene);
    }

    time_start(10);
    Scene_LoadScene(&editor->scene, currentSceneFile);
    vfree(currentSceneFile);

    info("SceneLoad:  " PRNT_REDD "%.2fms", time_get(10) * 1000);
    
    if (editor->scene.segment) {    
        for (int i = 0; i < roomList.num; i++) {
            char* file = roomList.item[i];
            
            time_start(10);
            Scene_LoadRoom(&editor->scene, file);
            info("RoomLoad:   " PRNT_REDD "%.2fms" PRNT_RSET " [%s]", time_get(10) * 1000, file);
        }
    }
    List_Free(&roomList);
    
    time_start(10);
    Scene_CacheBuild(&editor->scene);
    info("CacheBuild: " PRNT_REDD "%.2fms", time_get(10) * 1000);
    
    editor->scene.animOoT.index = 0;
    
    for (s32 i = 0; i < count; i++) {
        if (striend(item[i], ".toml")) {
            Toml toml = Toml_New();
            
            Toml_Load(&toml, item[i]);
            editor->scene.animOoT.index = Toml_GetInt(&toml, "draw_func_index");
            Toml_Free(&toml);
        }
    }
    
    if (editor->scene.animOoT.index)
        info("SceneAnim: %d", editor->scene.animOoT.index);
    info("Total: " PRNT_REDD "%.2fms", time_get(11) * 1000);
}

void Editor_Init(Editor* editor) {
    sEditor = editor;
    
    int x = 980;
    int y = 480;
    
    if (Toml_Var(&editor->config, "z64scene.win_res[0]"))
        x = Toml_GetInt(&editor->config, "z64scene.win_res[0]");
    if (Toml_Var(&editor->config, "z64scene.win_res[1]"))
        y = Toml_GetInt(&editor->config, "z64scene.win_res[1]");
    
    Theme_Init(0);
    Undo_Init(512);
    GUI_INITIALIZE(editor, "z64scene", x, y, 4, Editor_Update, Editor_Draw, Editor_DropCallback);
    GeoGrid_Init(&editor->geo, &editor->app, editor);
    GeoGrid_TaskTable(&editor->geo, gTaskTable, ArrCount(gTaskTable));
    
    glfwSetWindowSizeLimits(editor->app.window, 980, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);
    
    Cursor_Init(&editor->cursor, &editor->app);
    Cursor_CreateCursor(CURSOR_ARROW_U, gCursor_ArrowUp.data, 24, 12, 12);
    Cursor_CreateCursor(CURSOR_ARROW_D, gCursor_ArrowDown.data, 24, 12, 12);
    Cursor_CreateCursor(CURSOR_ARROW_L, gCursor_ArrowLeft.data, 24, 12, 12);
    Cursor_CreateCursor(CURSOR_ARROW_R, gCursor_ArrowRight.data, 24, 12, 12);
    Cursor_CreateCursor(CURSOR_ARROW_H, gCursor_ArrowHorizontal.data, 32, 16, 16);
    Cursor_CreateCursor(CURSOR_ARROW_V, gCursor_ArrowVertical.data, 32, 16, 16);
    Cursor_CreateCursor(CURSOR_CROSSHAIR, gCursor_Crosshair.data, 40, 19, 20);
    Cursor_CreateCursor(CURSOR_EMPTY, gCursor_Empty.data, 16, 0, 0);
    
    if (!GeoGrid_LoadLayout(&editor->geo, &editor->config)) {
        Rectf32 size = {
            editor->geo.workRect.x,
            editor->geo.workRect.y,
            editor->geo.workRect.w * 0.70,
            editor->geo.workRect.h
        };
        
        GeoGrid_AddSplit(&editor->geo, &size, TAB_VIEWPORT);
        
        size = (Rectf32) {
            size.w,
            editor->geo.workRect.y,
            editor->geo.workRect.w - size.w,
            editor->geo.workRect.h * 0.35
        };
        
        GeoGrid_AddSplit(&editor->geo, &size, TAB_ROOMLIST);
        
        size = (Rectf32) {
            size.x,
            editor->geo.workRect.y + size.h,
            size.w,
            editor->geo.workRect.h - size.h
        };
        
        GeoGrid_AddSplit(&editor->geo, &size, TAB_SETTINGS);
    }
    
    for (var i = 0; i < 5; i++) {
        Image_LoadMem(&sTexelFile[i], sIconData[i]->data, sIconData[i]->size);
        sIconImage[i].pixels = sTexelFile[i].data;
    }
    
    glfwSetWindowIcon(editor->app.window, 5, sIconImage);
    
    Database_Init();
    Scene_Init(&editor->scene);
    Gizmo_Init(&editor->gizmo, &editor->input, editor->vg);
    Editor_InitIcons(editor);
    
    DisplayList_GatherReferences(&editor->scene);
    
    editor->head[0] = new(Header);
    editor->geo.bar[0].headerFunc = Header_Update;
    Header_Init(editor->head[0], editor);
    
    char* items[] = {
        "../room_0.zroom",
        "../room_1.zroom",
        "../scene.zscene",
    };
    
    Editor_DropCallback(editor->app.window, 3, items);
}

void Editor_Destroy(Editor* editor) {
    Toml_SetVar(&editor->config, "z64scene.win_res[0]", "%d", editor->app.wdim.x);
    Toml_SetVar(&editor->config, "z64scene.win_res[1]", "%d", editor->app.wdim.y);
    
    GeoGrid_SaveLayout(&editor->geo, &editor->config, editor->fpconfig);
    GeoGrid_Destroy(&editor->geo);
    VectorGfx_Free(&gVecGfx_EyeOpen);
    Cursor_Free(&editor->cursor);
    Undo_Destroy();
    Scene_Free(&editor->scene);
    Gizmo_Free(&editor->gizmo);
    Database_Free();
    
    for (var i = 0; i < 5; i++)
        Image_Free(&sTexelFile[i]);
    
    vfree(editor->head[0], editor->head[1], editor);
}

f32 gFpsTime;
f32 gTotalTime;

void Editor_Update(Editor* editor) {
    profi_start(0);
    if (editor->scene.kill) {
        Scene_Free(&editor->scene);
        Scene_Init(&editor->scene);
    } else
        Scene_Update(&editor->scene);
    
    Undo_Update(&editor->input);
    profi_start(1); GeoGrid_Update(&editor->geo); profi_stop(1);
    profi_start(2); Cursor_Update(&editor->cursor); profi_stop(2);
    profi_start(3); Gizmo_Update(&editor->gizmo); profi_stop(3);
    
    profi_stop(PROFILER_FPS);
    gFpsTime = profi_get(PROFILER_FPS);
    profi_start(PROFILER_FPS);
}

void Editor_Draw(Editor* editor) {
    profi_start(4); GeoGrid_Draw(&editor->geo); profi_stop(4);
    profi_stop(0);
    gTotalTime = profi_get(0);
}
