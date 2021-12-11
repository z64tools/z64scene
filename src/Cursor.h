#ifndef __Z64CURSOR_H__
#define __Z64CURSOR_H__
#include <z64.h>
#include <nanovg.h>

typedef struct {
	RGBA8*    bitmap;
	GLFWimage img;
	void* glfwCur;
} CursorBitmap;

typedef struct {
	CursorBitmap cursor[64];
	void* _p;
} CursorContext;

typedef enum {
	CURSOR_DEFAULT,
	CURSOR_ARROW_U,
	CURSOR_ARROW_D,
	CURSOR_ARROW_L,
	CURSOR_ARROW_R,
	
	CURSOR_ARROW_H,
	CURSOR_ARROW_V,
	CURSOR_MAX
} CursorIndex;

void Cursor_Init(CursorContext* cursorCtx);
void Cursor_SetCursor(CursorIndex cursorId);

#endif