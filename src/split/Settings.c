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

#define SIDE_BUTTON_SIZE 24

///////////////////////////////////////////////////////////////////////////////

static void MenuDebug_Init(Editor* editor, void* __this, Split* split) {
    MenuDebug* this = __this;
    Scene* scene = &editor->scene;
    
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
}

static void MenuDebug_Update(Editor* editor, void* __this, Split* split) {
    MenuDebug* this = __this;
    Scene* scene = &editor->scene;
    SceneHeader* sceneHeader = Scene_GetSceneHeader(scene);
    EnvLightSettings* envSettings = Arli_At(&sceneHeader->envList, scene->curEnv);
    
    Element_Condition(&this->buttonIndoor, scene->segment != NULL);
    Element_Condition(&this->killScene, scene->segment != NULL);
    
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
    
    Element_Box(BOX_START); {
        
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

///////////////////////////////////////////////////////////////////////////////

static void MenuActor_Init(Editor* editor, void* __this, Split* split) {
    // MenuActor* this = __this;
}

static void MenuActor_Update(Editor* editor, void* __this, Split* split) {
    // MenuActor* this = __this;
    
}

///////////////////////////////////////////////////////////////////////////////

static struct {
    void  (*init)(Editor*, void*, Split*);
    void  (*update)(Editor*, void*, Split*);
    off_t offset;
} sSubMenuParam[] = {
    //crustify
    { MenuDebug_Init, MenuDebug_Update, offsetof(Settings, menuDebug) },
    { MenuActor_Init, MenuActor_Update, offsetof(Settings, menuActor) },
    { NULL,           MenuDebug_Update, offsetof(Settings, menuDebug) },
    { NULL,           MenuDebug_Update, offsetof(Settings, menuDebug) },
    //uncrustify
};

static void* GetArg(void* ptr, int index) {
    return ((u8*)ptr) + sSubMenuParam[index].offset;
}

static Rect GetSubRect(Split* split, int index) {
    Rect r;
    
    r.x = SPLIT_ELEM_X_PADDING / 2;
    r.y = SPLIT_ELEM_X_PADDING + (SIDE_BUTTON_SIZE + SPLIT_ELEM_X_PADDING) * index;
    r.w = SIDE_BUTTON_SIZE;
    r.h = SIDE_BUTTON_SIZE;
    
    return r;
}

///////////////////////////////////////////////////////////////////////////////

void Settings_Init(Editor* editor, Settings* this, Split* split) {
    for (int i = 0; i < ArrCount(sSubMenuParam); i++)
        if (sSubMenuParam[i].init)
            sSubMenuParam[i].init(editor, GetArg(this, i), split);
}

void Settings_Destroy(Editor* editor, Settings* this, Split* split) {
}

void Settings_Update(Editor* editor, Settings* this, Split* split) {
    Element_Header(split->taskCombo, 92);
    Element_Combo(split->taskCombo);
    
    Element_RowY(SPLIT_ELEM_X_PADDING * 2);
    Element_ShiftX(SIDE_BUTTON_SIZE);
    
    sSubMenuParam[this->subIndex].update(editor, GetArg(this, this->subIndex), split);
    
    if (!split->blockMouse && !editor->geo.state.blockElemInput) {
        Input* input = &editor->input;
        
        for (int i = 0; i < ArrCount(sSubMenuParam); i++) {
            Rect r = GetSubRect(split, i);
            
            if (Input_GetMouse(input, CLICK_L)->press)
                if (Split_CursorInRect(split, &r))
                    this->subIndex = i;
        }
    }
}

void Settings_Draw(Editor* editor, Settings* this, Split* split) {
    void* vg = editor->vg;
    Rect r = {
        0, 0, SIDE_BUTTON_SIZE + SPLIT_ELEM_X_PADDING, split->dispRect.h
    };
    Rect scissor = r;
    
    Gfx_DrawRounderRect(vg, r, Theme_GetColor(THEME_ELEMENT_DARK, 255, 1.25f));
    
    r.x += r.w - 1;
    r.w = 1;
    Gfx_DrawRounderRect(vg, r, Theme_GetColor(THEME_ELEMENT_LIGHT, 25, 1.25f));
    
    for (int i = 0; i < ArrCount(sSubMenuParam); i++) {
        Rect r = GetSubRect(split, i);
        
        if (this->subIndex == i) {
            r.w += SPLIT_ELEM_X_PADDING / 2;
            nvgScissor(vg, UnfoldRect(scissor));
            r.w += SPLIT_ELEM_X_PADDING;
            Gfx_DrawRounderOutline(vg, r, Theme_GetColor(THEME_ELEMENT_LIGHT, 25, 1.0f));
            Gfx_DrawRounderRect(vg, r, Theme_GetColor(THEME_BASE, 255, 1.0f));
            nvgResetScissor(vg);
        }
    }
}
