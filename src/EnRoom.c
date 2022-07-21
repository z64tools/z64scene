#include "Editor.h"

void EnRoom_Init(void* passArg, void* instance, Split* split) {
	// EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	
	this->buttonDayLight.txt = xFmt("Use Daylight");
	this->buttonDayLight.toggle = true;
	
	this->envIdSlider.isInt = true;
	this->envIdSlider.min = 0;
	this->envIdSlider.max = 255;
	
	this->envID.txt = "Env ID:";
	
	Element_Slider_SetValue(&this->envIdSlider, 0);
}

void EnRoom_Destroy(void* passArg, void* instance, Split* split) {
	// EditorContext* editCtx = passArg;
	// EnRoom* this = instance;
}

void EnRoom_Update(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	s32 x = SPLIT_ELEM_X_PADDING;
	s32 y = SPLIT_ELEM_X_PADDING;
	
	{
		Element_SetRect_Multiple(split, y, &this->buttonDayLight.rect, 1.0);
		editCtx->scene.useDaylight = Element_Button(&editCtx->geoGrid, split, &this->buttonDayLight);
		
		x = SPLIT_ELEM_X_PADDING;
		y += SPLIT_ELEM_Y_PADDING;
		
		this->envIdSlider.min = 0;
		this->envIdSlider.max = ClampMin(editCtx->scene.numEnv - 1, 0);
		this->envIdSlider.isInt = true;
		Element_SetRect_Multiple(split, y, &this->envID.rect, 0.25, &this->envIdSlider.rect, 0.75);
		Element_Text(&editCtx->geoGrid, split, &this->envID);
		editCtx->scene.setupEnv = Element_Slider(&editCtx->geoGrid, split, &this->envIdSlider);
	}
}

void EnRoom_Draw(void* passArg, void* instance, Split* split) {
	// EditorContext* editCtx = passArg;
	// EnRoom* this = instance;
	// void* vg = editCtx->vg;
}