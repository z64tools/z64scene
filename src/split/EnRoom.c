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
	Element_Name(&this->buttonCulling, "Culling");
	Element_Name(&this->killScene, "Unload Scene");
	
	Element_Button_SetValue(&this->buttonFPS, true, gLimitFPS);
	Element_Button_SetValue(&this->buttonCulling, true, editor->render.culling);
	Element_Button_SetValue(&this->buttonIndoor, true, false);
	Element_Button_SetValue(&this->buttonFog, true, true);
	
	Element_Combo_SetPropEnum(&this->envID, editor->interface.propEndID);
}

void EnRoom_Destroy(Editor* editor, EnRoom* this, Split* split) {
	PropEnum_Free(this->comboBox.prop);
}

void EnRoom_Update(Editor* editor, EnRoom* this, Split* split) {
	Element_Header(split, split->taskCombo, 128);
	Element_Combo(split->taskCombo);
	
	Element_RowY(SPLIT_ELEM_X_PADDING * 2);
	
	Element_Condition(&this->buttonIndoor, editor->scene.segment != NULL);
	Element_Condition(&this->comboBox, editor->scene.segment != NULL);
	Element_Condition(&this->killScene, editor->scene.segment != NULL);
	
	Element_Box(BOX_START); {
		Element_Row(split, &this->comboBox, 1.0);
		Element_DisplayName(&this->comboBox);
		Element_Combo(&this->comboBox);
		
		Element_Row(split, &this->envID, 1.0);
		Element_DisplayName(&this->envID);
		editor->scene.setupEnv = Element_Combo(&this->envID);
		
		Element_Row(split, NULL, 0.25, &this->buttonIndoor, 0.75);
		Element_Button_SetValue(&this->buttonIndoor, true, editor->scene.indoorLight);
		editor->scene.indoorLight = Element_Button(&this->buttonIndoor);
	} Element_Box(BOX_END);
	
	Element_Box(BOX_START); {
		Element_Row(split, Element_Text("Render"), 1.0);
		
		Element_Row(split, &this->buttonFog, 0.33333333, &this->buttonFPS, 0.33333333, &this->buttonCulling, 0.33333333);
		editor->scene.useFog = Element_Button(&this->buttonFog);
		gLimitFPS = Element_Button(&this->buttonFPS);
		editor->render.culling = Element_Button(&this->buttonCulling);
	} Element_Box(BOX_END);
	
	Element_Row(split, &this->killScene, 1.0);
	if (Element_Button(&this->killScene)) {
		Log("Free PROP");
		PropEnum_Free(editor->interface.propEndID);
		editor->interface.propEndID = NULL;
		Log("Free Scene");
		Scene_Free(&editor->scene);
	}
	
	Element_Combo_SetPropEnum(&this->envID, editor->interface.propEndID);
	Element_Condition(&this->envID, this->envID.prop != NULL);
}

void EnRoom_Draw(Editor* editor, EnRoom* this, Split* split) {
}
