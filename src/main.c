#include "Editor.h"

extern DataFile gAppIcon;
extern DataFile gCursor_ArrowUp;
extern DataFile gCursor_ArrowDown;
extern DataFile gCursor_ArrowLeft;
extern DataFile gCursor_ArrowRight;
extern DataFile gCursor_ArrowHorizontal;
extern DataFile gCursor_ArrowVertical;
extern DataFile gCursor_Crosshair;
extern DataFile gCursor_Empty;

#include <split/task_init.h>

int UnicodeMain(argc, argv) {
    Editor* editor = Calloc(sizeof(*editor));
    GLFWimage icon = {
        128, 128, gAppIcon.data
    };
    
    Theme_Init(0);
    
    GUI_INITIALIZE(editor, "z64scene", 980, 480, 4,
        Editor_Update, Editor_Draw, Editor_DropCallback);
    editor->geo.passArg = editor;
    GeoGrid_Init( &editor->geo, &editor->app.wdim, &editor->input, editor->vg);
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
    
    glfwSetWindowIcon(editor->app.window, 1, &icon);
    Editor_Init(editor);
    Interface_Main(&editor->app);
    Interface_Destroy(&editor->app);
    Editor_Destroy(editor);
    
    return 0;
}
