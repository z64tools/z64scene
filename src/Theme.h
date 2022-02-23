#ifndef __Z64GUI_H__
#define __Z64GUI_H__
#include <Global.h>
#include <Light.h>
#include <nanovg.h>
#include "../cJSON/cJSON.h"

typedef enum {
	THEME_BASE,
	THEME_BASE_SPLIT,
	THEME_HIGHLIGHT,
	THEME_TEXT,
	THEME_TEXT_OUTLINE,
	
	THEME_BASE_L1,
	THEME_BASE_L2,
	THEME_BASE_L3,
	
	THEME_PRIM, // YELLOW
	THEME_ACCENT, // BLUE
	
	THEME_MAX
} ThemeColor;

void Theme_Init(u32 themeId);
void Theme_SmoothStepToCol(NVGcolor* src, NVGcolor target, f32 a, f32 b, f32 c);
NVGcolor Theme_GetColor(ThemeColor, s32, f32);

#endif