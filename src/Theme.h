#ifndef __Z64GUI_H__
#define __Z64GUI_H__
#include "HermosauhuLib.h"
#include <Global.h>
#include <Light.h>
#include <nanovg.h>

struct EditorContext;
struct Subscreen;

typedef enum {
	THEME_BASE_DARK,
	THEME_BASE_WHITE,
	THEME_BOX,
	THEME_SPLITTER,
} ThemeColor;

NVGcolor Theme_GetColor(ThemeColor pal);

#endif