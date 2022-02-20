#include "Editor.h"

void EnRoom_Init(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	
	this->sceneName.txt = Graph_Alloc(512);
	strcpy(this->sceneName.txt, "Untitled Scene");
	this->leButton.txt = Tmp_String("This is a Button");
	this->leButton.toggle = true;
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
		"Scene Name:"
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
}

void EnRoom_Draw(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	void* vg = editCtx->vg;
}