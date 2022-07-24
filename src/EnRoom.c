#include "Editor.h"

void Update_PropMode(PropEnum* penum, s32 i, void* arg) {
	ElCombo* this = arg;
	
	this->key = i;
}

void EnRoom_Init(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnRoom* this = instance;
	
	Element_Combo_SetPropEnum(
		&this->comboBox,
		PropEnum_Init(
			Update_PropMode,
			&this->comboBox,
			1,
			(EnumItem[]) {
		{ 0, "Field" },
		{ 1, "Dungeon" },
	},
			2
		)
	);
	
	Element_Name(&this->buttonDayLight, "Sun/Moon Light Dir");
	Element_Name(&this->buttonFog, "Fog");
	Element_Name(&this->comboBox, "Keep:");
	Element_Name(&this->envIdSlider, "EnvID:");
	
	Element_Button_SetValue(&this->buttonDayLight, true, false);
	Element_Button_SetValue(&this->buttonFog, true, true);
	
	Element_Slider_SetParams(&this->envIdSlider, 0, 1, "int");
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
	
	if (editCtx->scene.segment == NULL) {
		Element_Disable(&this->envIdSlider);
		Element_Disable(&this->buttonDayLight);
		Element_Disable(&this->buttonFog);
		Element_Disable(&this->comboBox);
	} else {
		Element_Enable(&this->envIdSlider);
		Element_Enable(&this->buttonDayLight);
		Element_Enable(&this->buttonFog);
		Element_Enable(&this->comboBox);
		Element_Slider_SetParams(&this->envIdSlider, 0, ClampMin(editCtx->scene.numEnv - 1, 1), "int");
	}
	
	Element_RowY(0);
	Element_Row(split, &this->buttonDayLight, 0.5, &this->buttonFog, 0.5);
	editCtx->scene.useDaylight = Element_Button(&editCtx->geo, split, &this->buttonDayLight);
	editCtx->scene.useFog = Element_Button(&editCtx->geo, split, &this->buttonFog);
	
	Element_Row(split, &this->comboBox, 1.0);
	Element_DisplayName(&editCtx->geo, split, &this->comboBox);
	Element_Combo(&editCtx->geo, split, &this->comboBox);
	
	Element_Row(split, &this->envIdSlider, 1.0);
	Element_DisplayName(&editCtx->geo, split, &this->envIdSlider);
	editCtx->scene.setupEnv = Element_Slider(&editCtx->geo, split, &this->envIdSlider);
}

void EnRoom_Draw(void* passArg, void* instance, Split* split) {
	// EditorContext* editCtx = passArg;
	// EnRoom* this = instance;
	// void* vg = editCtx->vg;
}