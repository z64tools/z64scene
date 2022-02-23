#include "Editor.h"

static char demoText[21] = ":skawoUHHUH:";

void EnRoom_Init(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	
	this->sceneName.txt = editCtx->project.sceneName;
	
	if (!strcmp(this->sceneName.txt, "Untitled_Scene"))
		this->sceneName.isHintText = true;
	
	this->sceneName.align = ALIGN_LEFT;
	this->sceneName.size = 32;
	
	this->lele.txt = Graph_Alloc(32);
	this->lele.align = ALIGN_LEFT;
	this->lele.size = 12;
	
	this->leButton.txt = Tmp_String("This is a Button");
	this->leButton.toggle = true;
	
	this->saveLayout.txt = Tmp_String("Save Layout");
	this->slider.isInt = true;
	this->slider.min = 0;
	this->slider.max = 255;
	Element_Slider_SetValue(&this->slider, 255);
}

void EnRoom_Destroy(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
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
		x += Element_Text(&editCtx->geoCtx, split, &scenNameTx) + 4;
		Element_SetRect(&this->sceneName.rect, x, y, split->rect.w - x - SPLIT_ELEM_X_PADDING);
		Element_Textbox(&editCtx->geoCtx, split, &this->sceneName);
		
		x = SPLIT_ELEM_X_PADDING;
		y += SPLIT_ELEM_Y_PADDING;
		Element_SetRect(&this->lele.rect, x, y, split->rect.w - x - SPLIT_ELEM_X_PADDING);
		Element_Textbox(&editCtx->geoCtx, split, &this->lele);
		
		x = SPLIT_ELEM_X_PADDING;
		y += SPLIT_ELEM_Y_PADDING;
		Element_SetRect_Two(split, &this->leButton.rect, split->rect.w - SPLIT_ELEM_X_PADDING * 3 - SPLIT_TEXT_H, &this->checkBox.rect, y);
		Element_Button(&editCtx->geoCtx, split, &this->leButton);
		Element_Checkbox(&editCtx->geoCtx, split, &this->checkBox);
		
		x = SPLIT_ELEM_X_PADDING;
		y += SPLIT_ELEM_Y_PADDING;
		Element_SetRect(&this->slider.rect, x, y, split->rect.w - SPLIT_ELEM_X_PADDING * 2);
		Element_Slider(&editCtx->geoCtx, split, &this->slider);
	}
	
	x = SPLIT_ELEM_X_PADDING;
	y = split->cect.h - SPLIT_ELEM_Y_PADDING - 4;
	Element_SetRect(&this->saveLayout.rect, x, y, 0);
	
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