#include "Editor.h"

RGBA8 sDarkTheme[] = {
	{ 0x22, 0x24, 0x28, 0xFF }, // GUICOL_BASE_DARK
	{ 0xF2, 0xE7, 0xC4, 0xFF }, // GUICOL_BASE_WHITE
	{ 0x22 * 0.75f, 0x24 * 0.75f, 0x28 * 0.75f, 0xFF }, // GUICOL_SPLITTER
};

NVGcolor Theme_GetColor(ThemeColor pal) {
	return nvgRGBA(
		sDarkTheme[pal].r,
		sDarkTheme[pal].g,
		sDarkTheme[pal].b,
		sDarkTheme[pal].a
	);
}