#ifndef __Z64CURSOR_H__
#define __Z64CURSOR_H__
#include <z64.h>
#include <nanovg.h>

typedef struct {
	RGBA8*    bitmap;
	GLFWimage img;
	void* glfwCur;
} CursorBitmap;

typedef enum {
	CURSOR_DEFAULT,
	CURSOR_ARROW_L = 1,
	CURSOR_ARROW_U,
	CURSOR_ARROW_R,
	CURSOR_ARROW_D,
	
	CURSOR_ARROW_H,
	CURSOR_ARROW_V,
	
	CURSOR_CROSSHAIR,
	CURSOR_MAX
} CursorIndex;

typedef struct {
	CursorBitmap cursor[64];
	void* _p;
	CursorIndex  cursorNow;
	CursorIndex  cursorSet;
} CursorContext;

void Cursor_Init(CursorContext* cursorCtx);
void Cursor_Update(CursorContext* cursorCtx);
void Cursor_SetCursor(CursorIndex cursorId);

#endif