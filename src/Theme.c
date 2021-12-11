#include "Editor.h"

RGBA8 sDarkTheme[] = {
	{ 0x22, 0x24, 0x28, 0xFF }, // THEME_BASE
	{ 0xF2, 0xE7, 0xC4, 0xFF }, // THEME_BASE_CONT
	{ 0x22 * 1.25f, 0x24 * 1.25f, 0x28 * 1.25f, 0xFF }, // THEME_BOX
	{ 0x22 * 0.88f, 0x24 * 0.88f, 0x28 * 0.88f, 0xFF }, // THEME_SPLITTER
	{ 0x22 * 0.75f, 0x24 * 0.75f, 0x28 * 0.75f, 0xFF }, // THEME_SPLITTER_DARKER
};

NVGcolor Theme_GetColor(ThemeColor pal) {
	return nvgRGBA(
		sDarkTheme[pal].r,
		sDarkTheme[pal].g,
		sDarkTheme[pal].b,
		sDarkTheme[pal].a
	);
}