#include "Editor.h"

void EnRoom_Init(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	
	this->sceneName.txt = StrDupX("Untitled_Scene", 64);
	this->sceneName.isHintText = true;
	
	this->sceneName.align = ALIGN_LEFT;
	this->sceneName.size = 64;
	
	this->leButton.txt = xFmt("This is a Button");
	this->leButton.toggle = true;
	
	this->saveLayout.txt = xFmt("Save Layout");
	this->slider.isInt = true;
	this->slider.min = 0;
	this->slider.max = 255;
	Element_Slider_SetValue(&this->slider, 255);
}

void EnRoom_Destroy(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	
	Free(this->sceneName.txt);
}

void EnRoom_Update(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	s32 x = SPLIT_ELEM_X_PADDING;
	s32 y = SPLIT_ELEM_X_PADDING;
	static ElText scenNameTx = {
		"Name:"
	};
	
	{
		Element_SetRect(&scenNameTx.rect, x, y, 0);
		x += Element_Text(&editCtx->geoGrid, split, &scenNameTx) + 4;
		Element_SetRect(&this->sceneName.rect, x, y, split->rect.w - x - SPLIT_ELEM_X_PADDING);
		Element_Textbox(&editCtx->geoGrid, split, &this->sceneName);
		
		x = SPLIT_ELEM_X_PADDING;
		y += SPLIT_ELEM_Y_PADDING;
		Element_SetRect_Multiple(split, y, &this->leButton.rect, 0.5, &this->checkBox.rect, 0.5);
		Element_Button(&editCtx->geoGrid, split, &this->leButton);
		Element_Checkbox(&editCtx->geoGrid, split, &this->checkBox);
		
		x = SPLIT_ELEM_X_PADDING;
		y += SPLIT_ELEM_Y_PADDING;
		Element_SetRect(&this->slider.rect, x, y, split->rect.w - SPLIT_ELEM_X_PADDING * 2);
		Element_Slider(&editCtx->geoGrid, split, &this->slider);
	}
	
	x = SPLIT_ELEM_X_PADDING;
	y = split->cect.h - SPLIT_ELEM_Y_PADDING - 4;
	Element_SetRect(&this->saveLayout.rect, x, y, 0);
}

void EnRoom_Draw(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	void* vg = editCtx->vg;
}