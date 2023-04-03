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

// # # # # # # # # # # # # # # # # # # # #
// # EDITING                             #
// # # # # # # # # # # # # # # # # # # # #

void* NewMtx() {
    return Matrix_ToMtx(n64_graph_alloc(sizeof(Mtx)));
}

static Editor* sEditor;

// # # # # # # # # # # # # # # # # # # # #
// # EDITOR                              #
// # # # # # # # # # # # # # # # # # # # #

VectorGfx gVecGfx_EyeOpen;

static void Editor_InitIcons(Editor* editor) {
    extern DataFile gIcon_EyeOpen;
    
    VectorGfx_New(&gVecGfx_EyeOpen, gIcon_EyeOpen.data, 10.0f);
}

Editor* GetEditor(void) {
    return sEditor;
}

static void Editor_UpperHeader(void* pass, void* empty, Split* split) {
    // Editor* this = pass;
    // Scene* scene = &this->scene;
}

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
    if (editor->scene.segment)
        Scene_Free(&editor->scene);
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
    
    Theme_Init(0);
    Undo_Init(128);
    GUI_INITIALIZE(editor, "z64scene", 980, 480, 4, Editor_Update, Editor_Draw, Editor_DropCallback);
    GeoGrid_Init(&editor->geo, &editor->app, editor);
    GeoGrid_TaskTable(&editor->geo, gTaskTable, ArrCount(gTaskTable));
    
    Cursor_Init(&editor->cursor, &editor->app);
    Cursor_CreateCursor(CURSOR_ARROW_U, gCursor_ArrowUp.data, 24, 12, 12);
    Cursor_CreateCursor(CURSOR_ARROW_D, gCursor_ArrowDown.data, 24, 12, 12);
    Cursor_CreateCursor(CURSOR_ARROW_L, gCursor_ArrowLeft.data, 24, 12, 12);
    Cursor_CreateCursor(CURSOR_ARROW_R, gCursor_ArrowRight.data, 24, 12, 12);
    Cursor_CreateCursor(CURSOR_ARROW_H, gCursor_ArrowHorizontal.data, 32, 16, 16);
    Cursor_CreateCursor(CURSOR_ARROW_V, gCursor_ArrowVertical.data, 32, 16, 16);
    Cursor_CreateCursor(CURSOR_CROSSHAIR, gCursor_Crosshair.data, 40, 19, 20);
    Cursor_CreateCursor(CURSOR_EMPTY, gCursor_Empty.data, 16, 0, 0);
    
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
    
    for (var i = 0; i < 5; i++) {
        Image_LoadMem(&sTexelFile[i], sIconData[i]->data, sIconData[i]->size);
        sIconImage[i].pixels = sTexelFile[i].data;
    }
    
    glfwSetWindowIcon(editor->app.window, 5, sIconImage);
    
    editor->geo.bar[0].headerFunc = Editor_UpperHeader;
    Database_Init();
    Scene_Init(&editor->scene);
    Gizmo_Init(&editor->gizmo, &editor->input, editor->vg);
    Editor_InitIcons(editor);
    
#if 0
    static FileDialog loadFile;
    // static FileDialog saveFile;
    
    FileDialog_New(&loadFile, &editor->app, "Load File");
    // FileDialog_New(&saveFile, &editor->app, "Save File");
#endif
    
    char* files[] = {
        "../scene.zscene",
        "../room_0.zroom",
        "../room_1.zroom",
    };
    
    Editor_DropCallback(editor->app.window, 3, files);
    DisplayList_GatherReferences(&editor->scene);
}

void Editor_Destroy(Editor* editor) {
    GeoGrid_Destroy(&editor->geo);
    VectorGfx_Free(&gVecGfx_EyeOpen);
    Cursor_Free(&editor->cursor);
    Undo_Destroy();
    Gizmo_Free(&editor->gizmo);
    
    for (var i = 0; i < 5; i++)
        Image_Free(&sTexelFile[i]);
    
    vfree(editor);
}

void Editor_Update(Editor* editor) {
    if (editor->scene.kill)
        Scene_Free(&editor->scene);
    
    Undo_Update(&editor->input);
    GeoGrid_Update(&editor->geo);
    Cursor_Update(&editor->cursor);
    profi_stop(PROFILER_FPS);
    profi_start(PROFILER_FPS);
    
    Gizmo_Update(&editor->gizmo);
}

void Editor_Draw(Editor* editor) {
    GeoGrid_Draw(&editor->geo);
}
