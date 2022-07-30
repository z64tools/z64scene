#include "EnRoom.h"

void EnRoom_Init(Editor* editor, EnRoom* this, Split* split);
void EnRoom_Destroy(Editor* editor, EnRoom* this, Split* split);
void EnRoom_Update(Editor* editor, EnRoom* this, Split* split);
void EnRoom_Draw(Editor* editor, EnRoom* this, Split* split);

SplitTask gEnRoomTask = DefineTask("Room", EnRoom);

void EnRoom_Init(Editor* editor, EnRoom* this, Split* split) {
	PropEnum* prop = PropEnum_InitList(0, "Field", "Dungeon");
	
	Element_Combo_SetPropEnum(&this->comboBox, prop);
	
	Element_Name(&this->buttonIndoor, "Indoor");
	Element_Name(&this->buttonFog, "Fog");
	Element_Name(&this->comboBox, "Keep");
	Element_Name(&this->envID, "EnvID");
	Element_Name(&this->buttonFPS, "Limit FPS");
	Element_Name(&this->slider, "FPS");
	
	Element_Button_SetValue(&this->buttonFPS, true, gLimitFPS);
	Element_Button_SetValue(&this->buttonIndoor, true, false);
	Element_Button_SetValue(&this->buttonFog, true, true);
	
	Element_Slider_SetParams(&this->slider, 5, 120, "int");
	Element_Slider_SetValue(&this->slider, 120);
}

void EnRoom_Destroy(Editor* editor, EnRoom* this, Split* split) {
	PropEnum_Free(this->comboBox.prop);
}

void EnRoom_Update(Editor* editor, EnRoom* this, Split* split) {
	if (editor->scene.segment == NULL) {
		// Element_Disable(&this->envID);
		// Element_Disable(&this->buttonIndoor);
		// Element_Disable(&this->buttonFog);
		// Element_Disable(&this->comboBox);
	} else {
		Element_Enable(&this->envID);
		Element_Enable(&this->buttonIndoor);
		Element_Enable(&this->buttonFog);
		Element_Enable(&this->comboBox);
	}
	
	Element_RowY(0);
	Element_Row(split, &this->buttonIndoor, 0.5, &this->buttonFog, 0.5);
	Element_Button_SetValue(&this->buttonIndoor, true, editor->scene.indoorLight);
	editor->scene.indoorLight = Element_Button(&this->buttonIndoor);
	editor->scene.useFog = Element_Button(&this->buttonFog);
	
	Element_Box(BOX_START);
	
	Element_Row(split, &this->comboBox, 1.0);
	Element_DisplayName(&this->comboBox);
	Element_Combo(&this->comboBox);
	
	// Element_Box(BOX_START);
	
	Element_Row(split, &this->envID, 1.0);
	Element_DisplayName(&this->envID);
	Element_Combo_SetPropEnum(&this->envID, editor->interface.propEndID);
	Element_Condition(&this->envID, this->envID.prop != NULL);
	editor->scene.setupEnv = Element_Combo(&this->envID);
	
	Element_Box(BOX_END);
	// Element_Box(BOX_END);
	
	Element_Row(split, &this->buttonFPS, 0.5);
	gLimitFPS = Element_Button(&this->buttonFPS);
	// Element_Condition(&this->slider, gLimitFPS);
	// gFPS = Element_Slider(&this->slider);
}

void EnRoom_Draw(Editor* editor, EnRoom* this, Split* split) {
}
