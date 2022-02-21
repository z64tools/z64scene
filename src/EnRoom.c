#include "Editor.h"

static char demoText[21] = ":skawoUHHUH:";

void EnRoom_Init(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	
	// this->sceneName.txt = editCtx->project.sceneName;
	this->sceneName.txt = demoText;
	this->sceneName.align = ALIGN_LEFT;
	this->sceneName.size = 12;
	this->sceneName.isHintText = true;
	
	this->lele.txt = Graph_Alloc(32);
	this->lele.align = ALIGN_LEFT;
	this->lele.size = 12;
	
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
	
	if (demoText[12] != 0)
		printf("\a");
	
	Element_SetRect_Text(&scenNameTx.rect, x, y, 0);
	x += Element_Text(&editCtx->geoCtx, split, &scenNameTx);
	x += SPLIT_ELEM_X_PADDING;
	Element_SetRect_Text(&this->sceneName.rect, x, y, split->rect.w - x - SPLIT_ELEM_X_PADDING);
	Element_Textbox(&editCtx->geoCtx, split, &this->sceneName);
	
	x = SPLIT_ELEM_X_PADDING;
	y += SPLIT_ELEM_Y_PADDING;
	
	Element_SetRect_Text(&this->lele.rect, x, y, split->rect.w - x - SPLIT_ELEM_X_PADDING);
	Element_Textbox(&editCtx->geoCtx, split, &this->lele);
	
	x = SPLIT_ELEM_X_PADDING;
	y += SPLIT_ELEM_Y_PADDING;
	
	Element_SetRect_Text(&this->leButton.rect, x, y, 140);
	Element_Button(&editCtx->geoCtx, split, &this->leButton);
	
	x += this->leButton.rect.w + SPLIT_ELEM_X_PADDING;
	
	Element_SetRect_Text(&this->checkBox.rect, x, y, 0);
	Element_Checkbox(&editCtx->geoCtx, split, &this->checkBox);
	
	x = SPLIT_ELEM_X_PADDING;
	y += SPLIT_ELEM_Y_PADDING;
	
	Element_SetRect_Text(&this->slider.rect, x, y, 140);
	Element_Slider(&editCtx->geoCtx, split, &this->slider);
	
	x = SPLIT_ELEM_X_PADDING;
	y = split->cect.h - SPLIT_ELEM_Y_PADDING - 4;
	
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