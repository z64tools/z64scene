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

typedef enum {
	TAB_ROOM,
	TAB_3DVP,
} SplitEnum;

#include <split/Settings.h>
#include <split/Viewport.h>

void nothing() {
}

void draw_nothing(Editor* e, void* instance, Split* split) {
	Element_Header(split, split->taskCombo, 128);
	Element_Combo(split->taskCombo);
}

SplitTask empty = {
	.taskName = "Empty",
	.init     = nothing,
	.destroy  = nothing,
	.update   = nothing,
	.draw     = (void*)draw_nothing,
	.size     = 30
};

SplitTask* gTaskTable[] = {
	[TAB_ROOM] = &gSettingsTask,
	[TAB_3DVP] = &gViewportTask,
	&empty
};

int main(void) {
	Editor* editor;
	GLFWimage icon = {
		128, 128, gAppIcon.data
	};
	
	Log_Init();
	printf_WinFix();
	printf_SetPrefix("");
	Theme_Init(0);
	
	CallocX(editor);
	editor->vg = Interface_Init(
		"z64scene", &editor->app, &editor->input,
		editor, (void*)Editor_Update,
		(void*)Editor_Draw, Editor_DropCallback,
		980, 480, 4
	);
	
	editor->geo.passArg = editor;
	GeoGrid_Init(
		&editor->geo, &editor->app.wdim, &editor->input,
		editor->vg
	);
	GeoGrid_TaskTable(&editor->geo, gTaskTable, ArrayCount(gTaskTable));
	
	Cursor_Init(&editor->cursor, &editor->app);
	Cursor_CreateCursor(CURSOR_ARROW_U, gCursor_ArrowUp.data, 24, 12, 12);
	Cursor_CreateCursor(CURSOR_ARROW_D, gCursor_ArrowDown.data, 24, 12, 12);
	Cursor_CreateCursor(CURSOR_ARROW_L, gCursor_ArrowLeft.data, 24, 12, 12);
	Cursor_CreateCursor(CURSOR_ARROW_R, gCursor_ArrowRight.data, 24, 12, 12);
	Cursor_CreateCursor(CURSOR_ARROW_H, gCursor_ArrowHorizontal.data, 32, 16, 16);
	Cursor_CreateCursor(CURSOR_ARROW_V, gCursor_ArrowVertical.data, 32, 16, 16);
	Cursor_CreateCursor(
		CURSOR_CROSSHAIR, gCursor_Crosshair.data, 40, 19,
		20
	);
	Cursor_CreateCursor(CURSOR_EMPTY, gCursor_Empty.data, 16, 0, 0);
	
	Rectf32 size = {
		editor->geo.workRect.x,
		editor->geo.workRect.y,
		editor->geo.workRect.w * 0.70,
		editor->geo.workRect.h
	};
	
	GeoGrid_AddSplit(&editor->geo, "__split_L__", &size, TAB_3DVP);
	
	size = (Rectf32) {
		size.w,
		editor->geo.workRect.y,
		editor->geo.workRect.w - size.w,
		editor->geo.workRect.h
	};
	
	GeoGrid_AddSplit(&editor->geo, "__split_R__", &size, TAB_ROOM);
	
	glfwSetWindowIcon(editor->app.window, 1, &icon);
	Editor_Init(editor);
	Interface_Main(&editor->app);
	Interface_Destroy(&editor->app);
	
	return 0;
}
