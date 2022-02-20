#include "Editor.h"

NVGcolor sDarkTheme[THEME_MAX] = { 0 };

NVGcolor Theme_GetColor(ThemeColor pal, u8 alpha) {
	NVGcolor col = sDarkTheme[pal];
	
	col.a = 255.0 / (f32)alpha;
	
	return col;
}

void Theme_SmoothStepToCol(NVGcolor* src, NVGcolor target, f32 a, f32 b, f32 c) {
	Math_SmoothStepToF(&src->r, target.r, a, b, c);
	Math_SmoothStepToF(&src->g, target.g, a, b, c);
	Math_SmoothStepToF(&src->b, target.b, a, b, c);
	Math_SmoothStepToF(&src->a, target.a, a, b, c);
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
	
	sDarkTheme[THEME_SELC] = nvgHSLA(209.0 / 360, 0.95, 0.59, 175);
}