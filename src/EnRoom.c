#include "Editor.h"

void EnRoom_Init(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	
	this->sceneName.txt = editCtx->project.sceneName;
	this->leButton.txt = Tmp_String("This is a Button");
	this->leButton.toggle = true;
	this->saveLayout.txt = Tmp_String("Save Layout");
}

void EnRoom_Destroy(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
}

void EnRoom_Update(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	f32 x = SPLIT_ELEM_X_PADDING;
	f32 y = SPLIT_ELEM_X_PADDING;
	static ElText scenNameTx = {
		"Name:"
	};
	
	Element_SetRect_Text(&scenNameTx.rect, x, y, 0);
	x += Element_Text(&editCtx->geoCtx, split, &scenNameTx);
	x += SPLIT_ELEM_X_PADDING;
	Element_SetRect_Text(&this->sceneName.rect, x, y, split->rect.w - x - SPLIT_ELEM_X_PADDING);
	Element_Textbox(&editCtx->geoCtx, split, &this->sceneName);
	
	x = SPLIT_ELEM_X_PADDING;
	y += SPLIT_TEXT_H + SPLIT_ELEM_X_PADDING;
	
	Element_SetRect_Text(&this->leButton.rect, x, y, 0);
	Element_Button(&editCtx->geoCtx, split, &this->leButton);
	
	x += this->leButton.rect.w + SPLIT_ELEM_X_PADDING;
	
	Element_SetRect_Text(&this->checkBox.rect, x, y, 0);
	Element_Checkbox(&editCtx->geoCtx, split, &this->checkBox);
	
	x = SPLIT_ELEM_X_PADDING;
	y = split->rect.h - SPLIT_TEXT_H - SPLIT_BAR_HEIGHT - SPLIT_TEXT_PADDING * 2;
	
	Element_SetRect_Text(&this->saveLayout.rect, x, y, 0);
	
	if (Element_Button(&editCtx->geoCtx, split, &this->saveLayout)) {
		printf_debug("Layout Saved");
		GeoGrid_Layout_SaveJson(&editCtx->geoCtx);
	}
}

void EnRoom_Draw(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	void* vg = editCtx->vg;
}