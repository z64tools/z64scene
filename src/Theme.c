#include "Editor.h"

NVGcolor sDarkTheme[THEME_MAX] = { 0 };

NVGcolor Theme_GetColor(ThemeColor pal) {
	return sDarkTheme[pal];
}

void Theme_Init() {
	sDarkTheme[THEME_BASE] = nvgHSLA(199.0 / 360, 0.1, 0.090, 255);
	sDarkTheme[THEME_BAS2] = nvgHSLA(199.0 / 360, 0.1, 0.120, 255);
	sDarkTheme[THEME_TEXT] = nvgHSLA(042.0 / 360, 0.1, 0.800, 255);
	sDarkTheme[THEME_DARK] = nvgHSLA(199.0 / 360, 0.1, 0.100, 255);
}