#include "Editor.h"

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

void Editor_Init(Editor* editor) {
    sEditor = editor;
    
    Scene_Init(&editor->scene);
    Editor_InitIcons(editor);
}

void Editor_Destroy(Editor* editor) {
    VectorGfx_Free(&gVecGfx_EyeOpen);
    
    Free(editor);
}

void Editor_DropCallback(GLFWwindow* window, s32 count, char* item[]) {
    Editor* editor = GET_CONTEXT(window);
    s32 hasRoom = false;
    s32 hasScene = false;
    s32 roomCount = 0;
    s32 n = 0;
    
    for (s32 i = 0; i < count; i++) {
        char* file = item[i];
        
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
    for (s32 i = 0; i < count; i++) {
        char* file = item[i];
        
        if (StrEndCase(file, ".zscene")) {
            if (editor->scene.segment)
                Scene_Free(&editor->scene);
            
            Time_Start(10);
            Scene_LoadScene(&editor->scene, file);
            printf_info("SceneLoad:  " PRNT_REDD "%.2fms", Time_Get(10) * 1000);
            break;
        }
    }
    
    Time_Start(10);
    for (s32 i = 0; i < count; i++) {
        char* file = item[i];
        
        if (StrEndCase(file, ".zroom") || StrEndCase(file, ".zmap")) {
            roomCount++;
            if (editor->scene.segment)
                Scene_LoadRoom(&editor->scene, file);
        }
    }
    printf_info("RoomLoad:   " PRNT_REDD "%.2fms" PRNT_RSET " [%d]", Time_Get(10) * 1000, roomCount);
    
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
    
    printf_info("SceneAnim: %d", editor->scene.animOoT.index);
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
