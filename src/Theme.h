#ifndef __Z64GUI_H__
#define __Z64GUI_H__
#include <Global.h>
#include <Light.h>
#include <nanovg.h>
#include "../cJSON/cJSON.h"

typedef enum {
	THEME_BASE,
	THEME_LGHT,
	THEME_LINE,
	THEME_TEXT,
	THEME_HEDR,
	THEME_SPBG,
	THEME_SHDW,
	
	THEME_BUTI, // Button Idle
	THEME_BUTH, // Button Hover
	THEME_BUTP, // BUTTON Press
	
	THEME_SELC, // SELECTED
	
	THEME_MAX
} ThemeColor;

void Theme_Init(u32 themeId);
void Theme_SmoothStepToCol(NVGcolor* src, NVGcolor target, f32 a, f32 b, f32 c);
NVGcolor Theme_GetColor(ThemeColor pal, u8 alpha);

#endif