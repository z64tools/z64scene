#include "EnRoom.h"

void EnRoom_Init(Editor* editor, EnRoom* this, Split* split);
void EnRoom_Destroy(Editor* editor, EnRoom* this, Split* split);
void EnRoom_Update(Editor* editor, EnRoom* this, Split* split);
void EnRoom_Draw(Editor* editor, EnRoom* this, Split* split);

SplitTask gEnRoomTask = DefineTask("Room", EnRoom);

void EnRoom_Init(Editor* editor, EnRoom* this, Split* split) {
	Scene* scene = &editor->scene;
	SceneHeader* sceneHeader = Scene_GetSceneHeader(scene);
	RoomHeader* roomHeader = Scene_GetRoomHeader(scene, scene->curRoom);
	
	PropEnum* prop = PropEnum_InitList(0, "Field", "Dungeon");
	
	Element_Combo_SetPropEnum(&this->comboBox, prop);
	
	Element_Name(&this->buttonIndoor, "Indoor");
	Element_Name(&this->comboBox, "Keep");
	Element_Name(&this->envID, "EnvID");
	Element_Name(&this->killScene, "Unload Scene");
	
	Element_Name(&this->buttonFPS, "Limit FPS");
	Element_Name(&this->buttonCulling, "Culling");
	Element_Name(&this->buttonFog, "Fog");
	Element_Name(&this->buttonColView, "Collision");
	
	Element_Button_SetValue(&this->buttonIndoor, true, roomHeader->indoorLight);
	Element_Button_SetValue(&this->buttonFPS, true, gLimitFPS);
	Element_Button_SetValue(&this->buttonFog, true, scene->state & SCENE_DRAW_FOG);
	Element_Button_SetValue(&this->buttonCulling, true, scene->state & SCENE_DRAW_CULLING);
	Element_Button_SetValue(&this->buttonColView, true, scene->state & SCENE_DRAW_COLLISION);
	
	if (sceneHeader->lightList)
		Element_Combo_SetPropEnum(&this->envID, sceneHeader->lightList->enumProp);
	else
		Element_Combo_SetPropEnum(&this->envID, NULL);
}

void EnRoom_Destroy(Editor* editor, EnRoom* this, Split* split) {
	PropEnum_Free(this->comboBox.prop);
}

void EnRoom_Update(Editor* editor, EnRoom* this, Split* split) {
	Scene* scene = &editor->scene;
	SceneHeader* sceneHeader = Scene_GetSceneHeader(scene);
	RoomHeader* roomHeader = Scene_GetRoomHeader(scene, scene->curRoom);
	
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
		scene->curEnv = Element_Combo(&this->envID);
		
		Element_Row(split, NULL, 0.25, &this->buttonIndoor, 0.75);
		Element_Button_SetValue(&this->buttonIndoor, true, roomHeader->indoorLight);
		roomHeader->indoorLight = Element_Button(&this->buttonIndoor);
	} Element_Box(BOX_END);
	
	Element_Box(BOX_START); {
		Element_Row(split, Element_Text("Render"), 1.0);
		Element_Row(split,  &this->buttonFPS, 0.5, &this->buttonCulling, 0.5);
		Element_Row(split,  &this->buttonFog, 0.5, &this->buttonColView, 0.5);
		
		Element_Button_SetValue(&this->buttonFog, true, scene->state & SCENE_DRAW_FOG);
		Scene_SetState(scene, SCENE_DRAW_FOG, Element_Button(&this->buttonFog));
		
		Element_Button_SetValue(&this->buttonFPS, true, gLimitFPS);
		gLimitFPS = Element_Button(&this->buttonFPS);
		
		Element_Button_SetValue(&this->buttonCulling, true, scene->state & SCENE_DRAW_CULLING);
		Scene_SetState(scene, SCENE_DRAW_CULLING, Element_Button(&this->buttonCulling));
		
		Element_Button_SetValue(&this->buttonColView, true, scene->state & SCENE_DRAW_COLLISION);
		Scene_SetState(scene, SCENE_DRAW_COLLISION, Element_Button(&this->buttonColView));
	} Element_Box(BOX_END);
	
	Element_Row(split, &this->killScene, 1.0);
	
	if (Element_Button(&this->killScene)) {
		Scene_Free(&editor->scene);
		Interface_MessageWindow(&editor->app, "Nice", "Destroyed Current Scene!");
	}
	
	if (sceneHeader->lightList)
		Element_Combo_SetPropEnum(&this->envID, sceneHeader->lightList->enumProp);
	else
		Element_Combo_SetPropEnum(&this->envID, NULL);
	
	Element_Condition(&this->envID, this->envID.prop != NULL);
}

void EnRoom_Draw(Editor* editor, EnRoom* this, Split* split) {
}
