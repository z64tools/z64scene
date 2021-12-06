#ifndef __Z64GUI_H__
#define __Z64GUI_H__
#include "HermosauhuLib.h"
#include <Global.h>
#include <Light.h>
#include <nanovg.h>

struct EditorContext;
struct Element;

typedef enum {
	GUICOL_BASE_DARK,
	GUICOL_BASE_WHITE,
	GUICOL_SPLITTER
} GuiColorPalette;

typedef void (* ElemFunc)(struct EditorContext*, struct Element*);

typedef struct {
	u32 index;
	u32 size;
	ElemFunc init;
	ElemFunc destroy;
	ElemFunc update;
	ElemFunc draw;
} ElementInit;

typedef struct Element {
	u32      index;
	char*    title;
	Vec2f    pos;
	Vec2f    dim;
	RGBA8    bgColor;
	RGBA8    fontColor;
	ElemFunc init;
	ElemFunc destroy;
	ElemFunc update;
	ElemFunc draw;
	struct Element* prev;
	struct Element* next;
} Element;

typedef struct {
	Element* head;
	u32 numElements;
} ElementNode;

typedef struct {
	ElementNode node[16];
} ElementContext;

NVGcolor Element_GetColor(GuiColorPalette pal);
Element* Element_Spawn(
	struct EditorContext* editorCtx,
	u32 id,
	u32 priority,
	Vec2f pos,
	char* title
);
void Element_UpdateElements(struct EditorContext* editorCtx, ElementContext* elemCtx);
void Element_DrawElements(struct EditorContext* editorCtx, ElementContext* elemCtx);

extern ElementInit gElPanelInit;

#endif