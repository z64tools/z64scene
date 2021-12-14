#include "Editor.h"

NVGcolor sDarkTheme[THEME_MAX] = { 0 };

NVGcolor Theme_GetColor(ThemeColor pal) {
	return sDarkTheme[pal];
}

void Theme_Init(u32 themeId) {
	sDarkTheme[THEME_BASE] = nvgHSLA(199.0 / 360, 0.04, 0.28, 255);
	sDarkTheme[THEME_LINE] = nvgHSLA(199.0 / 360, 0.04, 0.12, 255);
	sDarkTheme[THEME_TEXT] = nvgHSLA(042.0 / 360, 0.20, 0.75, 255);
	sDarkTheme[THEME_HEDR] = nvgHSLA(199.0 / 360, 0.06, 0.17, 255);
	sDarkTheme[THEME_SPBG] = nvgHSLA(199.0 / 360, 0.06, 0.08, 255);
	sDarkTheme[THEME_SHDW] = nvgHSLA(199.0 / 360, 0.00, 0.00, 255);
}