#include "Editor.h"

u32 sIndex;
NVGcolor sDarkTheme[THEME_MAX] = { 0 };
NVGcolor sLightTheme[THEME_MAX] = { 0 };
NVGcolor* sTheme[] = {
	sDarkTheme,
	sLightTheme,
};

NVGcolor Theme_GetColor(ThemeColor pal, s32 alpha) {
	NVGcolor col = sTheme[sIndex][pal];
	
	col.a = (f32)alpha * 0.00392156862745; // X / 255
	
	return col;
}

void Theme_SmoothStepToCol(NVGcolor* src, NVGcolor target, f32 a, f32 b, f32 c) {
	Math_SmoothStepToF(&src->r, target.r, a, b, c);
	Math_SmoothStepToF(&src->g, target.g, a, b, c);
	Math_SmoothStepToF(&src->b, target.b, a, b, c);
	Math_SmoothStepToF(&src->a, target.a, a, b, c);
}

void Theme_Init(u32 themeId) {
	sIndex = themeId;
	sDarkTheme[THEME_BASE_L4] = nvgHSLA(225.0 / 360, 0.07, 0.30, 255);
	sDarkTheme[THEME_BASE_L3] = nvgHSLA(199.0 / 360, 0.04, 0.25, 255);
	sDarkTheme[THEME_BASE_L2] = nvgHSLA(199.0 / 360, 0.04, 0.20, 255);
	sDarkTheme[THEME_BASE_L1] = nvgHSLA(199.0 / 360, 0.06, 0.15, 255);
	sDarkTheme[THEME_BASE] =    nvgHSLA(199.0 / 360, 0.04, 0.10, 255);
	sDarkTheme[THEME_BASE_D1] = nvgHSLA(199.0 / 360, 0.00, 0.08, 255);
	sDarkTheme[THEME_BASE_D2] = nvgHSLA(199.0 / 360, 0.00, 0.05, 255);
	sDarkTheme[THEME_BASE_D3] = nvgHSLA(199.0 / 360, 0.00, 0.02, 255);
	
	sDarkTheme[THEME_TEXT] =           nvgHSLA(042.0 / 360, 0.20, 0.85, 255);
	sDarkTheme[THEME_TEXT_HIGHLIGHT] = nvgHSLA(042.0 / 360, 0.20, 0.93, 255);
	
	sDarkTheme[THEME_PRIM] =   nvgHSLA(45.0 / 360, 0.5, 0.5, 175);
	sDarkTheme[THEME_ACCENT] = nvgHSLA(235.0 / 360, 0.4, 0.5, 175);
	
	/* ───────────────────────────────────────────────────────────────────────── */
	
	sLightTheme[THEME_BASE_L4] = nvgHSLA(225.0 / 360, 0.07, 0.70, 255);
	sLightTheme[THEME_BASE_L3] = nvgHSLA(199.0 / 360, 0.04, 0.75, 255);
	sLightTheme[THEME_BASE_L2] = nvgHSLA(199.0 / 360, 0.04, 0.80, 255);
	sLightTheme[THEME_BASE_L1] = nvgHSLA(199.0 / 360, 0.06, 0.85, 255);
	sLightTheme[THEME_BASE] =    nvgHSLA(199.0 / 360, 0.04, 0.90, 255);
	sLightTheme[THEME_BASE_D1] = nvgHSLA(199.0 / 360, 0.00, 0.94, 255);
	sLightTheme[THEME_BASE_D2] = nvgHSLA(199.0 / 360, 0.00, 0.96, 255);
	sLightTheme[THEME_BASE_D3] = nvgHSLA(199.0 / 360, 0.00, 0.99, 255);
	
	sLightTheme[THEME_TEXT] =           nvgHSLA(042.0 / 360, 0.20, 0.1, 255);
	sLightTheme[THEME_TEXT_HIGHLIGHT] = nvgHSLA(042.0 / 360, 0.20, 0.1, 255);
	
	sLightTheme[THEME_PRIM] =   nvgHSLA(45.0 / 360, 0.9, 0.65, 175);
	sLightTheme[THEME_ACCENT] = nvgHSLA(235.0 / 360, 0.9, 0.65, 175);
}