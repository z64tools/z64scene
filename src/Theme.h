#ifndef __Z64GUI_H__
#define __Z64GUI_H__
#include <Global.h>
#include <Light.h>
#include <nanovg.h>

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
	
	THEME_MAX
} ThemeColor;

void Theme_Init(u32 themeId);
NVGcolor Theme_GetColor(ThemeColor pal, u8 alpha);

#endif