#include "Editor.h"
#define INCBIN_PREFIX g
#include "../incbin/incbin.h"

INCBIN(CursorArrowU_, "cursor/arrow_up.ia16");
INCBIN(CursorArrowL_, "cursor/arrow_left.ia16");
INCBIN(CursorArrowD_, "cursor/arrow_down.ia16");
INCBIN(CursorArrowR_, "cursor/arrow_right.ia16");
INCBIN(CursorArrowH_, "cursor/arrow_horizontal.ia16");
INCBIN(CursorArrowV_, "cursor/arrow_vertical.ia16");

static CursorContext* __cursorCtx;

/* / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Cursor_CreateBitmap(CursorContext* cursorCtx, CursorIndex id,  const u8* data, s32 size, s32 xcent, s32 ycent) {
	CursorBitmap* dest = &cursorCtx->cursor[id];
	
	cursorCtx->cursor[id].bitmap = Lib_Malloc(0, sizeof(RGBA8) * size * size);;
	
	for (s32 i = 0, j = 0; i < size * size; i++, j += 2) {
		dest->bitmap[i].r = data[j];
		dest->bitmap[i].g = data[j];
		dest->bitmap[i].b = data[j];
		dest->bitmap[i].a = data[j + 1];
	}
	
	dest->img.height = dest->img.width = size;
	dest->img.pixels = (void*)dest->bitmap;
	dest->glfwCur = glfwCreateCursor((void*)&dest->img, xcent, ycent);
	OsAssert(dest->glfwCur != NULL);
}

void Cursor_CreateCursors(CursorContext* cursorCtx) {
	cursorCtx->cursor[CURSOR_DEFAULT].glfwCur = glfwCreateStandardCursor(0);
	Cursor_CreateBitmap(cursorCtx, CURSOR_ARROW_U, gCursorArrowU_Data, 24, 12, 12);
	Cursor_CreateBitmap(cursorCtx, CURSOR_ARROW_D, gCursorArrowD_Data, 24, 12, 12);
	Cursor_CreateBitmap(cursorCtx, CURSOR_ARROW_L, gCursorArrowL_Data, 24, 12, 12);
	Cursor_CreateBitmap(cursorCtx, CURSOR_ARROW_R, gCursorArrowR_Data, 24, 12, 12);
	Cursor_CreateBitmap(cursorCtx, CURSOR_ARROW_H, gCursorArrowH_Data, 32, 16, 16);
	Cursor_CreateBitmap(cursorCtx, CURSOR_ARROW_V, gCursorArrowV_Data, 32, 16, 16);
}

void Cursor_Init(CursorContext* cursorCtx) {
	Cursor_CreateCursors(cursorCtx);
	__cursorCtx = cursorCtx;
}

void Cursor_SetCursor(CursorIndex cursorId) {
	glfwSetCursor(__appInfo->mainWindow, __cursorCtx->cursor[cursorId].glfwCur);
}