#ifndef __Z64CURSOR_H__
#define __Z64CURSOR_H__
#include <z64.h>
#include <nanovg.h>

typedef struct {
	RGBA8 pixel[48 * 48];
	GLFWimage img;
	void* glfwCur;
} CursorBitmap;

typedef struct {
	CursorBitmap* arrowU;
    CursorBitmap* arrowD;
    CursorBitmap* arrowL;
    CursorBitmap* arrowR;
} CursorContext;

void Cursor_Init(CursorContext* cursorCtx);

#endif