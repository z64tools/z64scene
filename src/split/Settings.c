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
    static EnvLightSettings copybuf;
    
    nested(void, AddDefaultLight, (s32 id)) {
        header->envList.entry[id].ambientColor[0] = 0x40;
        header->envList.entry[id].ambientColor[1] = 0x40;
        header->envList.entry[id].ambientColor[2] = 0x40;
        
        header->envList.entry[id].light1Color[0] = 0xC0;
        header->envList.entry[id].light1Color[1] = 0xC0;
        header->envList.entry[id].light1Color[2] = 0xC0;
        header->envList.entry[id].light1Dir[0] = 0x49;
        header->envList.entry[id].light1Dir[1] = 0x49;
        header->envList.entry[id].light1Dir[2] = 0x49;
        
        header->envList.entry[id].light2Color[0] = 0x20;
        header->envList.entry[id].light2Color[1] = 0x20;
        header->envList.entry[id].light2Color[2] = 0x20;
        header->envList.entry[id].light2Dir[0] = 0xB7;
        header->envList.entry[id].light2Dir[1] = 0xB7;
        header->envList.entry[id].light2Dir[2] = 0xB7;
        
        header->envList.entry[id].fogColor[0] = 0x80;
        header->envList.entry[id].fogColor[1] = 0x80;
        header->envList.entry[id].fogColor[2] = 0x80;
        header->envList.entry[id].fogNear = 0x3E0;
        header->envList.entry[id].fogFar = 0x3200;
    };
    
    switch (action) {
        case PROP_ADD:
            AddDefaultLight(header->envList.num);
            header->envList.num++;
            break;
            
        case PROP_INSERT:
            copybuf = header->envList.entry[prop->copyKey];
            AddDefaultLight(header->envList.num);
            header->envList.num++;
            arrmove_r(header->envList.entry, id, header->envList.num - id);
            
            if (prop->copy)
                header->envList.entry[id] = copybuf;
            break;
            
        case PROP_REMOVE:
            arrmove_l(header->envList.entry, id, header->envList.num - id);
            header->envList.num--;
            break;
            
        case PROP_DETACH:
            break;
            
        case PROP_RETACH:
            arrmove_l(header->envList.entry, prop->detachKey, header->envList.num - prop->detachKey);
            arrmove_r(header->envList.entry, id, header->envList.num - id);
            break;
            
        case PROP_DESTROY_DETACH:
            arrmove_l(header->envList.entry, prop->detachKey, header->envList.num - prop->detachKey);
            header->envList.num--;
            break;
    }
    
    return true;
}

void Settings_Init(Editor* editor, Settings* this, Split* split) {
    Scene* scene = &editor->scene;
    
    // SceneHeader* sceneHeader = Scene_GetSceneHeader(scene);
    // RoomHeader* roomHeader = Scene_GetRoomHeader(scene, scene->curRoom);
    
    Element_Name(&this->envAmbient, "Ambient");
    Element_Name(&this->envColA, "EnvA");
    Element_Name(&this->envColB, "EnvB");
    Element_Name(&this->envFogColor, "Fog");
    
    Element_Name(&this->buttonIndoor, "Indoor");
    Element_Name(&this->killScene, "Unload Scene");
    
    Element_Name(&this->buttonFPS, "Limit FPS");
    Element_Name(&this->buttonCulling, "Culling");
    Element_Name(&this->buttonFog, "Fog");
    Element_Name(&this->buttonColView, "Collision");
    
    Element_Name(&this->fogFar, "Far");
    Element_Name(&this->fogNear, "Near");
    
    Element_Slider_SetParams(&this->fogNear, 0, 1000, "int");
    Element_Slider_SetParams(&this->fogFar, 100, 32000, "int");
    
    Element_Button_SetValue(&this->buttonFPS, true, 1);
    Element_Button_SetValue(&this->buttonFog, true, scene->state & SCENE_DRAW_FOG);
    Element_Button_SetValue(&this->buttonCulling, true, scene->state & SCENE_DRAW_CULLING);
    Element_Button_SetValue(&this->buttonColView, true, scene->state & SCENE_DRAW_COLLISION);
    
    Element_Container_SetPropList(&this->cont, NULL, 6);
    this->cont.drag = true;
}

void Settings_Destroy(Editor* editor, Settings* this, Split* split) {
}

void Settings_Update(Editor* editor, Settings* this, Split* split) {
    Scene* scene = &editor->scene;
    SceneHeader* sceneHeader = Scene_GetSceneHeader(scene);
    EnvLightSettings* envSettings = &sceneHeader->envList.entry[scene->curEnv];
    
    Element_Header(split->taskCombo, 92);
    Element_Combo(split->taskCombo);
    
    Element_RowY(SPLIT_ELEM_X_PADDING * 2);
    
    Element_Condition(&this->buttonIndoor, scene->segment != NULL);
    Element_Condition(&this->killScene, scene->segment != NULL);
    Element_Condition(&this->cont, this->cont.prop != NULL);
    
    if (editor->scene.segment) {
        
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
    
    Element_Container_SetPropList(&this->cont, &sceneHeader->envList.prop, 6);
    PropList_SetOnChangeCallback(&sceneHeader->envList.prop, PropListCallback, scene, 0);
    
    Element_Box(BOX_START); {
        Element_Row(&this->cont, 1.0f);
        scene->curEnv = Element_Container(&this->cont);
        
        // Element_Row(NULL, 0.5f, &this->buttonIndoor, 0.5f);
        // Element_Button_SetValue(&this->buttonIndoor, true, scene->indoorLight);
        // Element_Checkbox(&this->buttonIndoor);
        
        Element_Row(&this->envAmbient, 1.0f);
        Element_DisplayName(&this->envAmbient, 0.5f);
        Element_Color(&this->envAmbient);
        
        Element_Row(&this->envColA, 1.0f);
        Element_Row(&this->envColB, 1.0f);
        Element_DisplayName(&this->envColA, 0.5f);
        Element_DisplayName(&this->envColB, 0.5f);
        Element_Color(&this->envColA);
        Element_Color(&this->envColB);
        
        Element_Box(BOX_START);
        Element_Row(&this->envFogColor, 1.0f);
        Element_DisplayName(&this->envFogColor, 0.5f);
        Element_Color(&this->envFogColor);
        
        Element_Row(&this->fogNear, 0.5f, &this->fogFar, 0.5f);
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
        Element_Row(Element_Text("Render"), 1.0);
        Element_Row( &this->buttonFPS, 0.5, &this->buttonCulling, 0.5);
        Element_Row( &this->buttonFog, 0.5, &this->buttonColView, 0.5);
        
        Element_Button(&this->buttonFPS);
        
        this->buttonFog.icon = &gVecGfx_EyeOpen;
        Element_Button_SetValue(&this->buttonFog, true, scene->state & SCENE_DRAW_FOG);
        Scene_SetState(scene, SCENE_DRAW_FOG, Element_Button(&this->buttonFog));
        
        Element_Button_SetValue(&this->buttonCulling, true, scene->state & SCENE_DRAW_CULLING);
        Scene_SetState(scene, SCENE_DRAW_CULLING, Element_Button(&this->buttonCulling));
        
        Element_Button_SetValue(&this->buttonColView, true, scene->state & SCENE_DRAW_COLLISION);
        Scene_SetState(scene, SCENE_DRAW_COLLISION, Element_Button(&this->buttonColView));
    } Element_Box(BOX_END);
    
    Element_Row(&this->killScene, 1.0);
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
