#include "z64scene.h"

RGBA8 sGuiColorPalette[] = {
	{ 0x12, 0x14, 0x18, 0xFF }
};

NVGcolor Gui_GetColor(GuiColorPalette pal) {
	return nvgRGBA(
		sGuiColorPalette[pal].r,
		sGuiColorPalette[pal].g,
		sGuiColorPalette[pal].b,
		sGuiColorPalette[pal].a
	);
}

void Gui_SidePanel(Z64SceneContext* z64Ctx) {
	Vec2f panelPos;
	Vec2f panelScale;
	
	panelScale.x = 300;
	panelScale.y = z64Ctx->appInfo.winScale.y;
	
	panelPos.x = z64Ctx->appInfo.winScale.x - panelScale.x;
	panelPos.y = 0;
	
	nvgBeginPath(z64Ctx->vg);
	nvgRect(z64Ctx->vg, panelPos.x, panelPos.y, panelScale.x, panelScale.y);
	nvgFillColor(z64Ctx->vg, Gui_GetColor(GUICOL_BASE));
	nvgFill(z64Ctx->vg);
}