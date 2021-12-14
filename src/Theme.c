#include "Editor.h"

NVGcolor sDarkTheme[THEME_MAX] = { 0 };

NVGcolor Theme_GetColor(ThemeColor pal) {
	return sDarkTheme[pal];
}

void Theme_Init(u32 themeId) {
	sDarkTheme[THEME_BASE] = nvgHSLA(199.0 / 360, 0.04, 0.12, 255);
	sDarkTheme[THEME_LINE] = nvgHSLA(199.0 / 360, 0.04, 0.08, 255);
	sDarkTheme[THEME_TEXT] = nvgHSLA(042.0 / 360, 0.20, 0.85, 255);
	sDarkTheme[THEME_HEDR] = nvgHSLA(199.0 / 360, 0.06, 0.13, 255);
	sDarkTheme[THEME_SPBG] = nvgHSLA(199.0 / 360, 0.04, 0.16, 255);
	sDarkTheme[THEME_SHDW] = nvgHSLA(199.0 / 360, 0.00, 0.00, 255);
	
	sDarkTheme[THEME_BUTI] = nvgHSLA(210.0 / 360, 0.60, 0.45, 255);
	sDarkTheme[THEME_BUTH] = nvgHSLA(210.0 / 360, 0.70, 0.50, 255);
	sDarkTheme[THEME_BUTP] = nvgHSLA(210.0 / 360, 0.70, 0.55, 255);
}