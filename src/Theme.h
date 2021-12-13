#ifndef __Z64GUI_H__
#define __Z64GUI_H__
#include <Global.h>
#include <Light.h>
#include <nanovg.h>

typedef enum {
	THEME_BASE,
	THEME_BAS2,
	THEME_TEXT,
	THEME_DARK,
	
	THEME_MAX
} ThemeColor;

void Theme_Init();
NVGcolor Theme_GetColor(ThemeColor pal);

#endif