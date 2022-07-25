#include "EnRoom.h"

void EnRoom_Init(Editor* editor, EnRoom* this, Split* split);
void EnRoom_Destroy(Editor* editor, EnRoom* this, Split* split);
void EnRoom_Update(Editor* editor, EnRoom* this, Split* split);
void EnRoom_Draw(Editor* editor, EnRoom* this, Split* split);

SplitTask gEnRoomTask = DefineTask("Room", EnRoom);

void EnRoom_Init(Editor* editor, EnRoom* this, Split* split) {
	PropEnum* prop = PropEnum_AssignList(0, "Field", "Dungeon");
	
	Element_Combo_SetPropEnum(&this->comboBox, prop);
	
	Element_Name(&this->buttonDayLight, "Sun/Moon Light Dir");
	Element_Name(&this->buttonFog, "Fog");
	Element_Name(&this->comboBox, "Keep:");
	Element_Name(&this->envID, "EnvID:");
	
	Element_Button_SetValue(&this->buttonDayLight, true, false);
	Element_Button_SetValue(&this->buttonFog, true, true);
	
	// Element_Slider_SetParams(&this->envID, 0, 1, "int");
	// Element_Slider_SetValue(&this->envID, 0);
}

void EnRoom_Destroy(Editor* editor, EnRoom* this, Split* split) {
	PropEnum_Free(this->comboBox.prop);
}

void EnRoom_Update(Editor* editor, EnRoom* this, Split* split) {
	if (editor->scene.segment == NULL) {
		Element_Disable(&this->envID);
		Element_Disable(&this->buttonDayLight);
		Element_Disable(&this->buttonFog);
		Element_Disable(&this->comboBox);
	} else {
		Element_Enable(&this->envID);
		Element_Enable(&this->buttonDayLight);
		Element_Enable(&this->buttonFog);
		Element_Enable(&this->comboBox);
	}
	
	Element_RowY(0);
	Element_Row(split, &this->buttonDayLight, 0.5, &this->buttonFog, 0.5);
	editor->scene.useDaylight = Element_Button(&editor->geo, split, &this->buttonDayLight);
	editor->scene.useFog = Element_Button(&editor->geo, split, &this->buttonFog);
	
	Element_Row(split, &this->comboBox, 1.0);
	Element_DisplayName(&editor->geo, split, &this->comboBox);
	Element_Combo(&editor->geo, split, &this->comboBox);
	
	Element_Row(split, &this->envID, 1.0);
	Element_DisplayName(&editor->geo, split, &this->envID);
	Element_Combo_SetPropEnum(&this->envID, editor->interface.propEndID);
	editor->scene.setupEnv = Element_Combo(&editor->geo, split, &this->envID);
}

void EnRoom_Draw(Editor* editor, EnRoom* this, Split* split) {
}
