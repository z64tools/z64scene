#include "Editor.h"

#define INCBIN_PREFIX
#include <incbin.h>

INCBIN(gCursor_ArrowU, "assets/arrow_up.ia16");
INCBIN(gCursor_ArrowL, "assets/arrow_left.ia16");
INCBIN(gCursor_ArrowD, "assets/arrow_down.ia16");
INCBIN(gCursor_ArrowR, "assets/arrow_right.ia16");
INCBIN(gCursor_ArrowH, "assets/arrow_horizontal.ia16");
INCBIN(gCursor_ArrowV, "assets/arrow_vertical.ia16");
INCBIN(gCursor_Crosshair, "assets/crosshair.ia16");
INCBIN(gCursor_Empty, "assets/empty.ia16");

static SplitTask gTaskTable[] = {
	{
		"None",
	},
	{
		"Room",
		DefineTask(EnRoom)
	},
	{
		"Scene View",
		DefineTask(EnSceneView)
	},
};

int main(void) {
	EditorContext* editCtx;
	
	Log_Init();
	printf_WinFix();
	printf_SetPrefix("");
	
	Calloc(editCtx, sizeof(EditorContext));
	editCtx->vg = Interface_Init("z64scene", &editCtx->app, &editCtx->input, editCtx, (void*)Editor_Update, (void*)Editor_Draw, Editor_DropCallback, 980, 480, 4);
	
	editCtx->geoGrid.passArg = editCtx;
	editCtx->geoGrid.taskTable = gTaskTable;
	editCtx->geoGrid.taskTableNum = ArrayCount(gTaskTable);
	
	Theme_Init(0);
	GeoGrid_Init(&editCtx->geoGrid, &editCtx->app.winDim, &editCtx->input, editCtx->vg);
	Cursor_Init(&editCtx->cursor);
	Cursor_CreateCursor(&editCtx->cursor, CURSOR_ARROW_U, gCursor_ArrowUData, 24, 12, 12);
	Cursor_CreateCursor(&editCtx->cursor, CURSOR_ARROW_D, gCursor_ArrowDData, 24, 12, 12);
	Cursor_CreateCursor(&editCtx->cursor, CURSOR_ARROW_L, gCursor_ArrowLData, 24, 12, 12);
	Cursor_CreateCursor(&editCtx->cursor, CURSOR_ARROW_R, gCursor_ArrowRData, 24, 12, 12);
	Cursor_CreateCursor(&editCtx->cursor, CURSOR_ARROW_H, gCursor_ArrowHData, 32, 16, 16);
	Cursor_CreateCursor(&editCtx->cursor, CURSOR_ARROW_V, gCursor_ArrowVData, 32, 16, 16);
	Cursor_CreateCursor(&editCtx->cursor, CURSOR_CROSSHAIR, gCursor_CrosshairData, 40, 19, 20);
	Cursor_CreateCursor(&editCtx->cursor, CURSOR_EMPTY, gCursor_EmptyData, 16, 0, 0);
	
	Rectf32 size = {
		editCtx->geoGrid.workRect.x,
		editCtx->geoGrid.workRect.y,
		editCtx->geoGrid.workRect.w * 0.70,
		editCtx->geoGrid.workRect.h
	};
	
	GeoGrid_AddSplit(&editCtx->geoGrid, "Viewport", &size)->id = 2;
	
	size = (Rectf32) {
		size.w,
		editCtx->geoGrid.workRect.y,
		editCtx->geoGrid.workRect.w - size.w,
		editCtx->geoGrid.workRect.h
	};
	
	GeoGrid_AddSplit(&editCtx->geoGrid, "RoomTab", &size)->id = 1;
	
	ThreadLock_Init();
	Interface_Main();
	
	ThreadLock_Free();
	
	return 0;
}
