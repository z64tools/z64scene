#ifndef __Z64GUI_H__
#define __Z64GUI_H__
#include "HermosauhuLib.h"
#include <Global.h>
#include <Light.h>
#include <nanovg.h>

struct Z64SceneContext;
struct GuiElement;

typedef enum {
	GUICOL_BASE,
} GuiColorPalette;

typedef void (* ElemFunc)(struct Z64SceneContext*, struct GuiElement*);

typedef struct GuiElement {
	u32      elemId;
	char*    title;
	Vec2f    pos;
	Vec2f    dim;
	RGBA8    bgColor;
	RGBA8    fontColor;
	ElemFunc init;
	ElemFunc destroy;
	ElemFunc update;
	ElemFunc draw;
	struct GuiElement* prev;
	struct GuiElement* next;
} GuiElement;

typedef struct {
} GuiContext;

void Gui_SidePanel(struct Z64SceneContext* z64Ctx);

#endif