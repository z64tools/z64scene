#include "Editor.h"

void EnRoom_Init(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	
	this->button.txt = "Press Me!";
}

void EnRoom_Destroy(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
}

void EnRoom_Update(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	Rect buttonRect = {
		SPLIT_TEXT_SPLIT* 2,
		SPLIT_TEXT_SPLIT* 2,
		72,
		SPLIT_TEXT_SPLIT + SPLIT_TEXT_SCALE + SPLIT_TEXT_SPLIT
	};
	
	Element_Button(&editCtx->geoCtx, split, &this->button, &buttonRect);
}

void EnRoom_Draw(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	void* vg = editCtx->vg;
}