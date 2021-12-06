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
	
	OsPrintfEx("Init OK");
}

void ElPanel_Destroy(EditorContext* editorCtx, Element* elem) {
	ElPanel* this = (ElPanel*)elem;
}

void ElPanel_Update(EditorContext* editorCtx, Element* elem) {
	ElPanel* this = (ElPanel*)elem;
	
	if (this->update == 0) {
		OsPrintfEx("Update OK");
		this->update++;
	}
}

void ElPanel_Draw(EditorContext* editorCtx, Element* elem) {
	ElPanel* this = (ElPanel*)elem;
}
