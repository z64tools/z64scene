#include "Settings.h"

void Settings_Init(Editor* editor, Settings* this, Split* split);
void Settings_Destroy(Editor* editor, Settings* this, Split* split);
void Settings_Update(Editor* editor, Settings* this, Split* split);
void Settings_Draw(Editor* editor, Settings* this, Split* split);

SplitTask gSettingsTask = {
    .taskName = "Properties",
    .init     = (void*)Settings_Init,
    .destroy  = (void*)Settings_Destroy,
    .update   = (void*)Settings_Update,
    .draw     = (void*)Settings_Draw,
    .size     = sizeof(Settings)
};

void Settings_Init(Editor* editor, Settings* this, Split* split) {
    Scene* scene = &editor->scene;
    SceneHeader* sceneHeader = Scene_GetSceneHeader(scene);
    RoomHeader* roomHeader = Scene_GetRoomHeader(scene, scene->curRoom);
    
    Element_Combo_SetPropList(&this->comboBox, PropList_InitList(0, "Field", "Dungeon"));
    
    Element_Name(&this->envColA, "EnvA");
    Element_Name(&this->envColB, "EnvB");
    
    Element_Name(&this->buttonIndoor, "Indoor");
    Element_Name(&this->comboBox, "Keep");
    Element_Name(&this->cont, "EnvID");
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
        Element_Container_SetPropList(&this->cont, sceneHeader->lightList->enumProp, 4);
    else
        Element_Container_SetPropList(&this->cont, NULL, 4);
}

void Settings_Destroy(Editor* editor, Settings* this, Split* split) {
    PropList_Free(this->comboBox.prop);
}

void Settings_Update(Editor* editor, Settings* this, Split* split) {
    Scene* scene = &editor->scene;
    SceneHeader* sceneHeader = Scene_GetSceneHeader(scene);
    PropList* lightEnum = NULL;
    
    Element_Header(split, split->taskCombo, 128);
    Element_Combo(split->taskCombo);
    
    Element_RowY(SPLIT_ELEM_X_PADDING * 2);
    
    Element_Condition(&this->buttonIndoor, editor->scene.segment != NULL);
    Element_Condition(&this->comboBox, editor->scene.segment != NULL);
    Element_Condition(&this->killScene, editor->scene.segment != NULL);
    if (sceneHeader && sceneHeader->lightList) {
        u32 envNum = scene->curEnv;
        
        Element_Color_SetColor(&this->envColA, sceneHeader->lightList->env[envNum].light1Color);
        Element_Color_SetColor(&this->envColB, sceneHeader->lightList->env[envNum].light2Color);
    } else {
        Element_Color_SetColor(&this->envColA, NULL);
        Element_Color_SetColor(&this->envColB, NULL);
    }
    
    Element_Box(BOX_START); {
        Element_Row(split, &this->comboBox, 1.0);
        Element_DisplayName(&this->comboBox, 0.25f);
        Element_Combo(&this->comboBox);
        
        Element_Row(split, &this->cont, 1.0);
        Element_DisplayName(&this->cont, 0.25f);
        scene->curEnv = Element_Container(&this->cont);
        
        Element_Row(split, NULL, 0.25, &this->buttonIndoor, 0.75);
        Element_Button_SetValue(&this->buttonIndoor, true, scene->indoorLight);
        scene->indoorLight = Element_Button(&this->buttonIndoor);
        
        Element_Row(split, &this->envColA, 0.5, &this->envColB, 0.5);
        Element_DisplayName(&this->envColA, 0.35f);
        Element_Color(&this->envColA);
        Element_DisplayName(&this->envColB, 0.35f);
        Element_Color(&this->envColB);
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
        lightEnum = sceneHeader->lightList->enumProp;
    
    Element_Container_SetPropList(&this->cont, lightEnum, 6);
    
    Element_Condition(&this->cont, this->cont.prop != NULL);
}

void Settings_Draw(Editor* editor, Settings* this, Split* split) {
}
