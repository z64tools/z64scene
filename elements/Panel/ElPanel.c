#include "ElPanel.h"

void ElPanel_Init(EditorContext* editorCtx, Element* elem);
void ElPanel_Destroy(EditorContext* editorCtx, Element* elem);
void ElPanel_Update(EditorContext* editorCtx, Element* elem);
void ElPanel_Draw(EditorContext* editorCtx, Element* elem);

ElementInit gElPanelInit = {
	.index = 0,
	.size = sizeof(ElPanel),
	.init = ElPanel_Init,
	.destroy = ElPanel_Destroy,
	.update = ElPanel_Update,
	.draw = ElPanel_Draw
};

void ElPanel_Init(EditorContext* editorCtx, Element* elem) {
	ElPanel* this = (ElPanel*)elem;
}

void ElPanel_Destroy(EditorContext* editorCtx, Element* elem) {
	ElPanel* this = (ElPanel*)elem;
}

void ElPanel_Update(EditorContext* editorCtx, Element* elem) {
	ElPanel* this = (ElPanel*)elem;
	
	elem->dim.x = 250;
	elem->dim.y = editorCtx->appInfo.winScale.y;
	
	elem->pos.x = editorCtx->appInfo.winScale.x - elem->dim.x;
	elem->pos.y = 0;
}

void ElPanel_Draw(EditorContext* editorCtx, Element* elem) {
	ElPanel* this = (ElPanel*)elem;
	Vec2f fontPos = elem->pos;
	
	nvgBeginPath(editorCtx->vg);
	nvgRect(editorCtx->vg, elem->pos.x, elem->pos.y, elem->dim.x, elem->dim.y);
	nvgFillColor(editorCtx->vg, Element_GetColor(GUICOL_BASE_DARK));
	nvgFill(editorCtx->vg);
	
	fontPos.x += 20;
	fontPos.y += 20;
	
	nvgFillColor(editorCtx->vg, Element_GetColor(GUICOL_BASE_WHITE));
	nvgFontSize(editorCtx->vg, 17.5f);
	nvgFontFace(editorCtx->vg, "sans");
	nvgTextAlign(editorCtx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	nvgTextBox(editorCtx->vg, fontPos.x, fontPos.y, 150, elem->title, NULL);
}
