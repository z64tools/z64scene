#include "Editor.h"

void EnRoom_Init(void* passArg, void* instance, Split* split) {
	// EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	
	this->buttonDayLight.txt = "Sun/Moon Light Dir";
	this->buttonDayLight.toggle = true;
	
	this->buttonFog.txt = "Fog";
	this->buttonFog.toggle = 2;
	this->buttonFog.state = 1;
	
	this->envIdSlider.isInt = true;
	this->envIdSlider.min = 0;
	this->envIdSlider.max = 1;
	
	this->envID.txt = "Env ID:";
	
	Element_Slider_SetValue(&this->envIdSlider, 0);
}

void EnRoom_Destroy(void* passArg, void* instance, Split* split) {
	// EditorContext* editCtx = passArg;
	// EnRoom* this = instance;
	printf("YEY");
}

void EnRoom_Update(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	
	Element_RowY(0);
	Element_Row(split, &this->buttonDayLight.rect, 0.5, &this->buttonFog.rect, 0.5);
	editCtx->scene.useDaylight = Element_Button(&editCtx->geo, split, &this->buttonDayLight);
	editCtx->scene.useFog = Element_Button(&editCtx->geo, split, &this->buttonFog);
	
	this->envIdSlider.min = 0;
	this->envIdSlider.max = ClampMin((f32)(editCtx->scene.numEnv) - 1.0f, 1.0f);
	this->envIdSlider.isInt = true;
	f32 w = Element_Text(&editCtx->geo, split, &this->envID) + SPLIT_ELEM_X_PADDING * 3;
	
	w /= split->rect.w;
	Element_Row(split, &this->envID.rect, w, &this->envIdSlider.rect, 1.0 - w);
	editCtx->scene.setupEnv = Element_Slider(&editCtx->geo, split, &this->envIdSlider);
}

void EnRoom_Draw(void* passArg, void* instance, Split* split) {
	// EditorContext* editCtx = passArg;
	// EnRoom* this = instance;
	// void* vg = editCtx->vg;
}