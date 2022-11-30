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

bool PropListCallback(PropList* prop, PropListChange action, s32 id) {
    Scene* scene = prop->udata1;
    SceneHeader* header = Scene_GetSceneHeader(scene);
    LightList* lightList = header->lightList;
    static EnvLightSettings detached;
    
    Block(void, AddDefaultLight, (s32 id)) {
        lightList->env[id].ambientColor[0] = 0x40;
        lightList->env[id].ambientColor[1] = 0x40;
        lightList->env[id].ambientColor[2] = 0x40;
        
        lightList->env[id].light1Color[0] = 0xC0;
        lightList->env[id].light1Color[1] = 0xC0;
        lightList->env[id].light1Color[2] = 0xC0;
        lightList->env[id].light1Dir[0] = 0x49;
        lightList->env[id].light1Dir[1] = 0x49;
        lightList->env[id].light1Dir[2] = 0x49;
        
        lightList->env[id].light2Color[0] = 0x20;
        lightList->env[id].light2Color[1] = 0x20;
        lightList->env[id].light2Color[2] = 0x20;
        lightList->env[id].light2Dir[0] = 0xB7;
        lightList->env[id].light2Dir[1] = 0xB7;
        lightList->env[id].light2Dir[2] = 0xB7;
        
        lightList->env[id].fogColor[0] = 0x80;
        lightList->env[id].fogColor[1] = 0x80;
        lightList->env[id].fogColor[2] = 0x80;
        lightList->env[id].fogNear = 0x3E0;
        lightList->env[id].fogFar = 0x3200;
    };
    
    switch (action) {
        case PROP_ADD:
            AddDefaultLight(lightList->num);
            lightList->num++;
            
            break;
        case PROP_INSERT:
            AddDefaultLight(lightList->num);
            lightList->num++;
            ArrMoveR(lightList->env, id, lightList->num - id);
            break;
        case PROP_REMOVE:
            ArrMoveL(lightList->env, id, lightList->num - id);
            lightList->num--;
            break;
        case PROP_DETACH:
            detached = lightList->env[id];
            ArrMoveL(lightList->env, id, lightList->num - id);
            lightList->num--;
            break;
        case PROP_RETACH:
            lightList->num++;
            ArrMoveR(lightList->env, id, lightList->num - id);
            lightList->env[id] = detached;
            break;
        case PROP_DESTROY_DETACH:
            break;
    }
    
    return true;
}

void Settings_Init(Editor* editor, Settings* this, Split* split) {
    Scene* scene = &editor->scene;
    // SceneHeader* sceneHeader = Scene_GetSceneHeader(scene);
    RoomHeader* roomHeader = Scene_GetRoomHeader(scene, scene->curRoom);
    
    Element_Name(&this->envAmbient, "Ambient");
    Element_Name(&this->envColA, "EnvA");
    Element_Name(&this->envColB, "EnvB");
    Element_Name(&this->envFogColor, "Fog");
    
    Element_Name(&this->buttonIndoor, "Indoor");
    Element_Name(&this->cont, "EnvID");
    Element_Name(&this->killScene, "Unload Scene");
    
    Element_Name(&this->buttonFPS, "Limit FPS");
    Element_Name(&this->buttonCulling, "Culling");
    Element_Name(&this->buttonFog, "Fog");
    Element_Name(&this->buttonColView, "Collision");
    
    Element_Name(&this->fogFar, "Far");
    Element_Name(&this->fogNear, "Near");
    
    Element_Slider_SetParams(&this->fogNear, 0, 1000, "int");
    Element_Slider_SetParams(&this->fogFar, 100, 32000, "int");
    
    Element_Button_SetValue(&this->buttonIndoor, true, roomHeader->indoorLight);
    Element_Button_SetValue(&this->buttonFPS, true, gLimitFPS);
    Element_Button_SetValue(&this->buttonFog, true, scene->state & SCENE_DRAW_FOG);
    Element_Button_SetValue(&this->buttonCulling, true, scene->state & SCENE_DRAW_CULLING);
    Element_Button_SetValue(&this->buttonColView, true, scene->state & SCENE_DRAW_COLLISION);
    
    Element_Container_SetPropList(&this->cont, NULL, 8);
}

void Settings_Destroy(Editor* editor, Settings* this, Split* split) {
}

void Settings_Update(Editor* editor, Settings* this, Split* split) {
    Scene* scene = &editor->scene;
    SceneHeader* sceneHeader = Scene_GetSceneHeader(scene);
    u32 envNum = scene->curEnv;
    EnvLightSettings* envSettings = sceneHeader && sceneHeader->lightList &&
        sceneHeader->lightList->env ? &sceneHeader->lightList->env[envNum] : NULL;
    
    Element_Header(split, split->taskCombo, 128);
    Element_Combo(split->taskCombo);
    
    Element_RowY(SPLIT_ELEM_X_PADDING * 2);
    
    Element_Condition(&this->buttonIndoor, editor->scene.segment != NULL);
    Element_Condition(&this->killScene, editor->scene.segment != NULL);
    
    PropList* envList = NULL;
    
    if (envSettings) {
        envList = &sceneHeader->lightList->list;
        PropList_SetOnChangeCallback(envList, PropListCallback, &editor->scene, 0);
        
        Element_Color_SetColor(&this->envAmbient, envSettings->ambientColor);
        Element_Color_SetColor(&this->envColA, envSettings->light1Color);
        Element_Color_SetColor(&this->envColB, envSettings->light2Color);
        Element_Color_SetColor(&this->envFogColor, envSettings->fogColor);
        
        Element_Slider_SetValue(&this->fogNear, envSettings->fogNear & 0x3FF);
        Element_Slider_SetValue(&this->fogFar, envSettings->fogFar);
    } else {
        Element_Color_SetColor(&this->envAmbient, NULL);
        Element_Color_SetColor(&this->envColA, NULL);
        Element_Color_SetColor(&this->envColB, NULL);
        Element_Color_SetColor(&this->envFogColor, NULL);
    }
    
    Element_Container_SetPropList(&this->cont, envList, 8);
    Element_Condition(&this->cont, this->cont.prop != NULL);
    
    Element_Box(BOX_START); {
        Element_Row(split, &this->cont, 1.0f);
        Element_DisplayName(&this->cont, 0.25f);
        scene->curEnv = Element_Container(&this->cont);
        
        Element_Row(split, NULL, 0.25f, &this->buttonIndoor, 0.75f);
        Element_Button_SetValue(&this->buttonIndoor, true, scene->indoorLight);
        scene->indoorLight = Element_Button(&this->buttonIndoor);
        
        Element_Row(split, &this->envAmbient, 1.0f);
        Element_DisplayName(&this->envAmbient, 0.25f);
        Element_Color(&this->envAmbient);
        
        Element_Row(split, &this->envColA, 0.5f, &this->envColB, 0.5f);
        Element_DisplayName(&this->envColA, 0.5f);
        Element_DisplayName(&this->envColB, 0.5f);
        Element_Color(&this->envColA);
        Element_Color(&this->envColB);
        
        Element_Box(BOX_START);
        Element_Row(split, &this->envFogColor, 1.0);
        Element_DisplayName(&this->envFogColor, 0.15f);
        Element_Color(&this->envFogColor);
        
        Element_Row(split, &this->fogNear, 0.5f, &this->fogFar, 0.5f);
        Element_DisplayName(&this->fogNear, 0.3f);
        Element_DisplayName(&this->fogFar, 0.25f);
        
        if (envSettings) {
            envSettings->fogNear = Element_Slider(&this->fogNear);
            envSettings->fogFar = Element_Slider(&this->fogFar);
        } else {
            Element_Slider(&this->fogNear);
            Element_Slider(&this->fogFar);
        }
        Element_Box(BOX_END);
    } Element_Box(BOX_END);
    
    Element_Box(BOX_START); {
        Element_Row(split, Element_Text("Render"), 1.0);
        Element_Row(split,  &this->buttonFPS, 0.5, &this->buttonCulling, 0.5);
        Element_Row(split,  &this->buttonFog, 0.5, &this->buttonColView, 0.5);
        
        this->buttonFog.icon = &gVecGfx_EyeOpen;
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
        Scene_Kill(&editor->scene);
        
        Element_Color_SetColor(&this->envAmbient, NULL);
        Element_Color_SetColor(&this->envColA, NULL);
        Element_Color_SetColor(&this->envColB, NULL);
        Element_Color_SetColor(&this->envFogColor, NULL);
    }
}

void Settings_Draw(Editor* editor, Settings* this, Split* split) {
}
