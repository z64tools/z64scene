#ifndef __Z64GUI_H__
#define __Z64GUI_H__
#include <Global.h>
#include <Light.h>
#include <nanovg.h>
#include "../cJSON/cJSON.h"

typedef enum {
	THEME_BASE,
	THEME_LIGHT,
	THEME_LINE,
	THEME_TEXT,
	THEME_TEXT_HIGHLIGHT,
	THEME_HEADER,
	THEME_SPBG,
	THEME_SHADOW,
	
	THEME_BUTTON_IDLE, // Button Idle
	THEME_BUTTON_HOVER, // Button Hover
	THEME_BUTTON_PRESS, // BUTTON Press
	
	THEME_SELECTION, // SELECTED
	
	THEME_MAX
} ThemeColor;

void Theme_Init(u32 themeId);
void Theme_SmoothStepToCol(NVGcolor* src, NVGcolor target, f32 a, f32 b, f32 c);
NVGcolor Theme_GetColor(ThemeColor pal, u8 alpha);

#endif