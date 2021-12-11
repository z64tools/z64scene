#include "Editor.h"
#define INCBIN_PREFIX g
#include "../incbin/incbin.h"

INCBIN(CursorArrowUp_, "cursor/arrow_up.data");

void Cursor_CreateCursors(CursorContext* cursorCtx);

/* / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Cursor_Init(CursorContext* cursorCtx) {
	Cursor_CreateCursors(cursorCtx);
}

void Cursor_CreateBitmap(CursorBitmap* dest,  const u8* data) {
	for (s32 i = 0, j = 0; i < 48 * 48; i++, j += 2) {
		dest->pixel[i].r = data[j];
		dest->pixel[i].g = data[j];
		dest->pixel[i].b = data[j];
		dest->pixel[i].a = data[j + 1];
	}
	
	dest->img.height = dest->img.width = 48;
	dest->img.pixels = (void*)dest->pixel;
	dest->glfwCur = glfwCreateCursor((void*)&dest->img, 2, 0);
	OsAssert(dest->glfwCur != NULL);
}

void Cursor_CreateCursors(CursorContext* cursorCtx) {
	cursorCtx->arrowU = Lib_Calloc(0, sizeof(CursorBitmap));
	Cursor_CreateBitmap(cursorCtx->arrowU, gCursorArrowUp_Data);
	
	cursorCtx->arrowD = Lib_Calloc(0, sizeof(CursorBitmap));
	Cursor_CreateBitmap(cursorCtx->arrowD, gCursorArrowUp_Data);
	
	cursorCtx->arrowL = Lib_Calloc(0, sizeof(CursorBitmap));
	Cursor_CreateBitmap(cursorCtx->arrowL, gCursorArrowUp_Data);
	
	cursorCtx->arrowR = Lib_Calloc(0, sizeof(CursorBitmap));
	Cursor_CreateBitmap(cursorCtx->arrowR, gCursorArrowUp_Data);
}