#include "Editor.h"

void Interface_ListBox(NVGcontext* vg, Rect rect, s32 count, char* item[]) {
	nvgBeginPath(vg);
	nvgRoundedRect(
		vg,
		rect.x,
		rect.y,
		rect.w,
		rect.h,
		5.0f
	);
	nvgFillColor(vg, Theme_GetColor(THEME_BOX));
	nvgFill(vg);
}