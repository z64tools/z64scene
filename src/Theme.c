#include "Editor.h"

u32 sIndex;
HSLA8 sDarkTheme[THEME_MAX] = { 0 };
HSLA8 sLightTheme[THEME_MAX] = { 0 };
HSLA8* sTheme[] = {
	sDarkTheme,
	sLightTheme,
};

NVGcolor Theme_GetColor(ThemeColor pal, s32 alpha, f32 mult) {
	HSLA8 col = sTheme[sIndex][pal];
	
	if (sIndex == 1)
		mult = 1 / mult;
	
	return nvgHSLA(col.h, col.s * mult, col.l * mult, CLAMP_MAX(alpha, 255));
}

void Theme_SmoothStepToCol(NVGcolor* src, NVGcolor target, f32 a, f32 b, f32 c) {
	Math_SmoothStepToF(&src->r, target.r, a, b, c);
	Math_SmoothStepToF(&src->g, target.g, a, b, c);
	Math_SmoothStepToF(&src->b, target.b, a, b, c);
	Math_SmoothStepToF(&src->a, target.a, a, b, c);
}

void Theme_Init(u32 themeId) {
	sIndex = themeId;
	{
		sDarkTheme[THEME_BASE_L3] =     (HSLA8) { 225.0 / 360, 0.07, 0.30 };
		sDarkTheme[THEME_BASE_L2] =     (HSLA8) { 199.0 / 360, 0.04, 0.25 };
		sDarkTheme[THEME_BASE_L1] =     (HSLA8) { 199.0 / 360, 0.06, 0.15 };
		sDarkTheme[THEME_BASE_SPLIT] =  (HSLA8) { 199.0 / 360, 0.04, 0.20 };
		sDarkTheme[THEME_BASE] =        (HSLA8) { 199.0 / 360, 0.04, 0.10 };
		
		sDarkTheme[THEME_HIGHLIGHT] =   (HSLA8) { 235.0 / 360, 0.20, 0.90 };
		sDarkTheme[THEME_TEXT] =        (HSLA8) { 042.0 / 360, 0.20, 0.95 };
		sDarkTheme[THEME_TEXT_OUTLINE] = (HSLA8) { 042.0 / 360, 0.20, 0.15 };
		
		sDarkTheme[THEME_PRIM] =        (HSLA8) { 235.0 / 360, 0.6, 0.6 };
		sDarkTheme[THEME_ACCENT] =      (HSLA8) { 45.0 / 360, 0.6, 0.6 };
	}
	/* ───────────────────────────────────────────────────────────────────────── */
	{
		sLightTheme[THEME_BASE_L3] =    (HSLA8) { 225.0 / 360, 0.07, 0.85 };
		sLightTheme[THEME_BASE_L2] =    (HSLA8) { 199.0 / 360, 0.04, 0.75 };
		sLightTheme[THEME_BASE_L1] =    (HSLA8) { 199.0 / 360, 0.06, 0.70 };
		sLightTheme[THEME_BASE_SPLIT] = (HSLA8) { 199.0 / 360, 0.04, 0.80 };
		sLightTheme[THEME_BASE] =       (HSLA8) { 199.0 / 360, 0.04, 0.90 };
		
		sLightTheme[THEME_HIGHLIGHT] =  (HSLA8) { 199.0 / 360, 0.00, 0.15 };
		sLightTheme[THEME_TEXT] =       (HSLA8) { 042.0 / 360, 0.20, 0.1 };
		sLightTheme[THEME_TEXT_OUTLINE] = (HSLA8) { 042.0 / 360, 0.00, 1.0 };
		
		sLightTheme[THEME_PRIM] =       (HSLA8) { 235.0 / 360, 0.6, 0.65 };
		sLightTheme[THEME_ACCENT] =     (HSLA8) { 45.0 / 360, 0.6, 0.65 };
	}
}