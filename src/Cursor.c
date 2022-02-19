#include "Editor.h"
#define INCBIN_STYLE  INCBIN_STYLE_SNAKE
#define INCBIN_PREFIX gCursor
#include "../incbin/incbin.h"

INCBIN(ArrowU, "cursor/arrow_up.ia16");
INCBIN(ArrowL, "cursor/arrow_left.ia16");
INCBIN(ArrowD, "cursor/arrow_down.ia16");
INCBIN(ArrowR, "cursor/arrow_right.ia16");
INCBIN(ArrowH, "cursor/arrow_horizontal.ia16");
INCBIN(ArrowV, "cursor/arrow_vertical.ia16");
INCBIN(Crosshair, "cursor/crosshair.ia16");

static CursorContext* __cursorCtx;

/* / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Cursor_CreateBitmap(CursorContext* cursorCtx, CursorIndex id,  const u8* data, s32 size, s32 xcent, s32 ycent) {
	CursorBitmap* dest = &cursorCtx->cursor[id];
	
	cursorCtx->cursor[id].bitmap = Malloc(0, sizeof(RGBA8) * size * size);;
	
	for (s32 i = 0, j = 0; i < size * size; i++, j += 2) {
		dest->bitmap[i].r = data[j];
		dest->bitmap[i].g = data[j];
		dest->bitmap[i].b = data[j];
		dest->bitmap[i].a = data[j + 1];
	}
	
	dest->img.height = dest->img.width = size;
	dest->img.pixels = (void*)dest->bitmap;
	dest->glfwCur = glfwCreateCursor((void*)&dest->img, xcent, ycent);
	Assert(dest->glfwCur != NULL);
}

void Cursor_CreateCursors(CursorContext* cursorCtx) {
	cursorCtx->cursor[CURSOR_DEFAULT].glfwCur = glfwCreateStandardCursor(0);
	Cursor_CreateBitmap(cursorCtx, CURSOR_ARROW_U, gCursorArrowU_data, 24, 12, 12);
	Cursor_CreateBitmap(cursorCtx, CURSOR_ARROW_D, gCursorArrowD_data, 24, 12, 12);
	Cursor_CreateBitmap(cursorCtx, CURSOR_ARROW_L, gCursorArrowL_data, 24, 12, 12);
	Cursor_CreateBitmap(cursorCtx, CURSOR_ARROW_R, gCursorArrowR_data, 24, 12, 12);
	Cursor_CreateBitmap(cursorCtx, CURSOR_ARROW_H, gCursorArrowH_data, 32, 16, 16);
	Cursor_CreateBitmap(cursorCtx, CURSOR_ARROW_V, gCursorArrowV_data, 32, 16, 16);
	Cursor_CreateBitmap(cursorCtx, CURSOR_CROSSHAIR, gCursorCrosshair_data, 40, 19, 20);
}

void Cursor_Init(CursorContext* cursorCtx) {
	Cursor_CreateCursors(cursorCtx);
	__cursorCtx = cursorCtx;
	cursorCtx->cursorForce = CURSOR_NONE;
}

void Cursor_Update(CursorContext* cursorCtx) {
	if (cursorCtx->cursorForce != CURSOR_NONE) {
		if (cursorCtx->cursorNow != cursorCtx->cursorForce) {
			glfwSetCursor(__appInfo->mainWindow, __cursorCtx->cursor[cursorCtx->cursorForce].glfwCur);
			cursorCtx->cursorNow = cursorCtx->cursorForce;
		}
		cursorCtx->cursorForce = CURSOR_NONE;
		
		return;
	}
	
	if (cursorCtx->cursorSet != cursorCtx->cursorNow) {
		glfwSetCursor(__appInfo->mainWindow, __cursorCtx->cursor[cursorCtx->cursorSet].glfwCur);
		cursorCtx->cursorNow = cursorCtx->cursorSet;
	}
	
}

void Cursor_SetCursor(CursorIndex cursorId) {
	__cursorCtx->cursorSet = cursorId;
}

void Cursor_ForceCursor(CursorIndex cursorId) {
	__cursorCtx->cursorForce = cursorId;
}