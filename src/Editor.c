#include "Editor.h"

#include <split/task_init.h>

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

TexFile sTexelFile[5];
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
    Editor* this = pass;
    Scene* scene = &this->scene;
    
    Element_Header(split, &scene->ui.roomCombo, 98);
    Element_Combo(&this->scene.ui.roomCombo);
}

void Editor_Init(Editor* editor) {
    sEditor = editor;
    
    Theme_Init(0);
    GUI_INITIALIZE(editor, "z64scene", 980, 480, 4, Editor_Update, Editor_Draw, Editor_DropCallback);
    GeoGrid_Init(&editor->geo, &editor->app, editor);
    GeoGrid_TaskTable(&editor->geo, gTaskTable, ArrayCount(gTaskTable));
    
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
    
    GeoGrid_AddSplit(&editor->geo, &size, TAB_3DVP);
    
    size = (Rectf32) {
        size.w,
        editor->geo.workRect.y,
        editor->geo.workRect.w - size.w,
        editor->geo.workRect.h
    };
    
    GeoGrid_AddSplit(&editor->geo, &size, TAB_ROOM);
    
    for (var i = 0; i < 5; i++) {
        TexFile_LoadMem(&sTexelFile[i], sIconData[i]->data, sIconData[i]->size);
        sIconImage[i].pixels = sTexelFile[i].data;
    }
    
    glfwSetWindowIcon(editor->app.window, 5, sIconImage);
    
    editor->geo.bar[0].headerFunc = Editor_UpperHeader;
    Scene_Init(&editor->scene);
    Editor_InitIcons(editor);
    
#if 1
    static FileDialog loadFile;
    // static FileDialog saveFile;
    
    FileDialog_New(&loadFile, &editor->app, "Load File");
    // FileDialog_New(&saveFile, &editor->app, "Save File");
#endif
}

void Editor_Destroy(Editor* editor) {
    GeoGrid_Destroy(&editor->geo);
    VectorGfx_Free(&gVecGfx_EyeOpen);
    
    for (var i = 0; i < 5; i++)
        TexFile_Free(&sTexelFile[i]);
    
    Free(editor);
}

void Editor_DropCallback(GLFWwindow* window, s32 count, char* item[]) {
    Editor* editor = GET_CONTEXT(window);
    var hasRoom = false;
    var hasScene = false;
    var n = -1;
    u32 s = 0;
    ItemList roomList = ItemList_Initialize();
    
    Time_Start(11);
    
    for (var i = 0; i < count; i++) {
        char* file = item[i];
        
        s += strlen(item[i]) + 1;
        
        if (StrEndCase(file, ".zroom") || StrEndCase(file, ".zmap"))
            hasRoom = true;
        
        if (StrEndCase(file, ".zscene")) {
            hasScene = true;
            n = i;
        }
    }
    
    if (!hasRoom || !hasScene)
        return;
    
    printf_info("" PRNT_YELW "LOADING: " PRNT_BLUE "%s", item[n]);
    if (editor->scene.segment)
        Scene_Free(&editor->scene);
    Time_Start(10);
    Scene_LoadScene(&editor->scene, item[n]);
    printf_info("SceneLoad:  " PRNT_REDD "%.2fms", Time_Get(10) * 1000);
    
    if (editor->scene.segment) {
        ItemList_Alloc(&roomList, count, s);
        
        for (var i = 0; i < count; i++) {
            char* file = item[i];
            
            if (StrEndCase(file, ".zroom") || StrEndCase(file, ".zmap"))
                ItemList_AddItem(&roomList, file);
        }
        
        ItemList_SortNatural(&roomList);
        
        forlist(i, roomList) {
            char* file = roomList.item[i];
            
            Time_Start(10);
            Scene_LoadRoom(&editor->scene, file);
            printf_info("RoomLoad:   " PRNT_REDD "%.2fms" PRNT_RSET " [%s]", Time_Get(10) * 1000, file);
        }
        
        ItemList_Free(&roomList);
    }
    
    Time_Start(10);
    Scene_CacheBuild(&editor->scene);
    printf_info("CacheBuild: " PRNT_REDD "%.2fms", Time_Get(10) * 1000);
    
    editor->scene.animOoT.index = 0;
    for (s32 i = 0; i < count; i++) {
        if (StrEndCase(item[i], ".cfg")) {
            MemFile cfg = MemFile_Initialize();
            
            MemFile_LoadFile_String(&cfg, item[i]);
            
            if (Config_Variable(cfg.str, "scene_func_id"))
                editor->scene.animOoT.index = Config_GetInt(&cfg, "scene_func_id");
            
            MemFile_Free(&cfg);
        }
    }
    
    if (editor->scene.animOoT.index)
        printf_info("SceneAnim: %d", editor->scene.animOoT.index);
    printf_info("Total: " PRNT_REDD "%.2fms", Time_Get(11) * 1000);
}

void Editor_Update(Editor* editor) {
    if (editor->scene.kill)
        Scene_Free(&editor->scene);
    
    GeoGrid_Update(&editor->geo);
    Cursor_Update(&editor->cursor);
    Profiler_O(PROFILER_FPS);
    Profiler_I(PROFILER_FPS);
}

void Editor_Draw(Editor* editor) {
    GeoGrid_Draw(&editor->geo);
}
