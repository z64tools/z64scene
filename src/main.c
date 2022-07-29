#include "Editor.h"

#define INCBIN_PREFIX
#include <incbin.h>

INCBIN(gCursor_ArrowU_, "assets/arrow_up.ia16");
INCBIN(gCursor_ArrowL_, "assets/arrow_left.ia16");
INCBIN(gCursor_ArrowD_, "assets/arrow_down.ia16");
INCBIN(gCursor_ArrowR_, "assets/arrow_right.ia16");
INCBIN(gCursor_ArrowH_, "assets/arrow_horizontal.ia16");
INCBIN(gCursor_ArrowV_, "assets/arrow_vertical.ia16");
INCBIN(gCursor_Crosshair_, "assets/crosshair.ia16");
INCBIN(gCursor_Empty_, "assets/empty.ia16");
INCBIN(gAppIcon_, "assets/icon32.rgba");

typedef enum {
	TAB_NULL,
	TAB_ROOM,
	TAB_3DVP,
} SplitEnum;

#include <split/EnRoom.h>
#include <split/EnViewport.h>

SplitTask* gTaskTable[] = {
	[TAB_ROOM] = &gEnRoomTask,
	[TAB_3DVP] = &gEnViewportTask,
};

int main(void) {
	Editor* editor;
	GLFWimage icon = {
		128, 128, (void*)gAppIcon_Data
	};
	
	Log_Init();
	printf_WinFix();
	printf_SetPrefix("");
	
	Calloc(editor, sizeof(Editor));
	editor->vg = Interface_Init("z64scene", &editor->app, &editor->input, editor, (void*)Editor_Update, (void*)Editor_Draw, Editor_DropCallback, 980, 480, 4);
	
	editor->geo.passArg = editor;
	editor->geo.taskTable = gTaskTable;
	
	Theme_Init(0);
	GeoGrid_Init(&editor->geo, &editor->app.winDim, &editor->input, editor->vg);
	Cursor_Init(&editor->cursor, &editor->app);
	Cursor_CreateCursor(CURSOR_ARROW_U, gCursor_ArrowU_Data, 24, 12, 12);
	Cursor_CreateCursor(CURSOR_ARROW_D, gCursor_ArrowD_Data, 24, 12, 12);
	Cursor_CreateCursor(CURSOR_ARROW_L, gCursor_ArrowL_Data, 24, 12, 12);
	Cursor_CreateCursor(CURSOR_ARROW_R, gCursor_ArrowR_Data, 24, 12, 12);
	Cursor_CreateCursor(CURSOR_ARROW_H, gCursor_ArrowH_Data, 32, 16, 16);
	Cursor_CreateCursor(CURSOR_ARROW_V, gCursor_ArrowV_Data, 32, 16, 16);
	Cursor_CreateCursor(CURSOR_CROSSHAIR, gCursor_Crosshair_Data, 40, 19, 20);
	Cursor_CreateCursor(CURSOR_EMPTY, gCursor_Empty_Data, 16, 0, 0);
	
	Rectf32 size = {
		editor->geo.workRect.x,
		editor->geo.workRect.y,
		editor->geo.workRect.w * 0.70,
		editor->geo.workRect.h
	};
	
	GeoGrid_AddSplit(&editor->geo, "Viewport", &size)->id = TAB_3DVP;
	
	size = (Rectf32) {
		size.w,
		editor->geo.workRect.y,
		editor->geo.workRect.w - size.w,
		editor->geo.workRect.h
	};
	
	GeoGrid_AddSplit(&editor->geo, "Room", &size)->id = TAB_ROOM;
	
	// GeoGrid_Debug(true);
	glfwSetWindowIcon(editor->app.window, 1, &icon);
	Editor_Init(editor);
	Interface_Main(&editor->app);
	
	return 0;
}
