#include "Editor.h"

void EnRoom_Init(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	
	this->button.txt = "Press Me!";
	this->button.rect = (Rect) {
		(SPLIT_TEXT_PADDING) * 2,
		(SPLIT_TEXT_PADDING) * 2,
		72,
		SPLIT_TEXT_PADDING + SPLIT_TEXT_SMALL + SPLIT_TEXT_PADDING
	};
	
	strcpy(this->txtbox.txt, "textbox");
	this->txtbox.rect = (Rect) {
		(SPLIT_TEXT_PADDING) * 2,
		this->button.rect.y + this->button.rect.h + (SPLIT_TEXT_PADDING),
		72 * 3,
		SPLIT_TEXT_PADDING + SPLIT_TEXT_SMALL + SPLIT_TEXT_PADDING
	};
	
	strcpy(this->txtbox2.txt, "textbox2");
	this->txtbox2.rect = (Rect) {
		(SPLIT_TEXT_PADDING) * 2,
		this->txtbox.rect.y + this->txtbox.rect.h + (SPLIT_TEXT_PADDING),
		72 * 3,
		SPLIT_TEXT_PADDING + SPLIT_TEXT_SMALL + SPLIT_TEXT_PADDING
	};
}

void EnRoom_Destroy(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
}

void EnRoom_Update(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	
	Element_Button(&editCtx->geoCtx, split, &this->button);
	Element_Textbox(&editCtx->geoCtx, split, &this->txtbox);
	Element_Textbox(&editCtx->geoCtx, split, &this->txtbox2);
}

void EnRoom_Draw(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	void* vg = editCtx->vg;
}