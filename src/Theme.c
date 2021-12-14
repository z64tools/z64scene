#include "Editor.h"

NVGcolor sDarkTheme[THEME_MAX] = { 0 };

NVGcolor Theme_GetColor(ThemeColor pal) {
	return sDarkTheme[pal];
}

void Theme_Init(u32 themeId) {
	sDarkTheme[THEME_BASE] = nvgHSLA(199.0 / 360, 0.04, 0.12, 255);
	sDarkTheme[THEME_LGHT] = nvgHSLA(199.0 / 360, 0.04, 0.26, 175);
	sDarkTheme[THEME_LINE] = nvgHSLA(199.0 / 360, 0.04, 0.08, 255);
	sDarkTheme[THEME_TEXT] = nvgHSLA(042.0 / 360, 0.20, 0.85, 255);
	sDarkTheme[THEME_HEDR] = nvgHSLA(199.0 / 360, 0.06, 0.13, 255);
	sDarkTheme[THEME_SPBG] = nvgHSLA(199.0 / 360, 0.04, 0.16, 255);
	sDarkTheme[THEME_SHDW] = nvgHSLA(199.0 / 360, 0.00, 0.00, 255);
	
	sDarkTheme[THEME_BUTI] = nvgHSLA(225.0 / 360, 0.04, 0.14, 175);
	sDarkTheme[THEME_BUTH] = nvgHSLA(225.0 / 360, 0.07, 0.20, 175);
	sDarkTheme[THEME_BUTP] = nvgHSLA(225.0 / 360, 0.07, 0.30, 175);
}