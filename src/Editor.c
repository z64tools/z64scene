#include "Editor.h"
#include "Database.h"

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
    ElCombo fileMenu;
    ElCombo optionsMenu;
} Header;

typedef struct {
    
} FileDialog;

static void FileDialog_Init(GeoGrid* geo, ContextMenu* context) {
    context->state.offsetOriginRect = false;
    context->state.widthAdjustment = false;
}

static void FileDialog_Draw(GeoGrid* geo, ContextMenu* context) {
    Header* this = context->udata;
}

static void Header_Init(Header* this, Editor* editor) {
    Element_Combo_SetArli(&this->fileMenu, SceneDatabase_GetList(MENUDATA_FILE));
    this->fileMenu.menu = SceneDatabase_GetName(MENUDATA_FILE);
    this->fileMenu.align = NVG_ALIGN_CENTER;
    
    Element_Combo_SetArli(&this->optionsMenu, SceneDatabase_GetList(MENUDATA_OPTIONS));
    this->optionsMenu.menu = SceneDatabase_GetName(MENUDATA_OPTIONS);
    this->optionsMenu.align = NVG_ALIGN_CENTER;
}

static void Header_Update(void* pass, void* ____null____, Split* split) {
    Editor* editor = pass;
    Header* this = editor->head[0];
    
    Element_Header(NULL, 8, &this->fileMenu, 64, &this->optionsMenu, 64);
    
    if (Element_Combo(&this->fileMenu)) {
        switch (this->fileMenu.arlist->cur) {
            case 0:
                Rect r = { 0, 0, UnfoldVec2(editor->app.wdim) };
                
                ContextMenu_Custom(&editor->geo, this, NULL, FileDialog_Init, FileDialog_Draw, NULL, r);
                
                break;
        }
    }
    
    if (Element_Combo(&this->optionsMenu)) {
    }
}

////////////////////////////////////////////////////////////////////////////////

static void Editor_DropCallback(GLFWwindow* window, s32 count, char* item[]) {
    Editor* editor = GET_CONTEXT(window);
    var hasRoom = false;
    var hasScene = false;
    var n = -1;
    List roomList = List_New();
    
    time_start(11);
    
    for (var i = 0; i < count; i++) {
        char* file = item[i];
        
        if (striend(file, ".zroom") || striend(file, ".zmap"))
            hasRoom = true;
        
        if (striend(file, ".zscene")) {
            hasScene = true;
            n = i;
        }
    }
    
    if (!hasRoom || !hasScene)
        return;
    
    info("" PRNT_YELW "LOADING: " PRNT_BLUE "%s", item[n]);
    if (editor->scene.segment) {
        Gizmo_UnselectAll(&editor->gizmo);
        Actor_UnselectAll(&editor->scene, NULL);
        Scene_Free(&editor->scene);
        Scene_Init(&editor->scene);
    }
    time_start(10);
    Scene_LoadScene(&editor->scene, item[n]);
    info("SceneLoad:  " PRNT_REDD "%.2fms", time_get(10) * 1000);
    
    if (editor->scene.segment) {
        List_Alloc(&roomList, count);
        
        for (var i = 0; i < count; i++) {
            char* file = item[i];
            
            if (striend(file, ".zroom") || striend(file, ".zmap"))
                List_Add(&roomList, file);
        }
        
        List_Sort(&roomList);
        
        for (int i = 0; i < roomList.num; i++) {
            char* file = roomList.item[i];
            
            time_start(10);
            Scene_LoadRoom(&editor->scene, file);
            info("RoomLoad:   " PRNT_REDD "%.2fms" PRNT_RSET " [%s]", time_get(10) * 1000, file);
        }
        
        List_Free(&roomList);
    }
    
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
