#include "Properties.h"

void Properties_Init(Editor* editor, Properties* this, Split* split);
void Properties_Destroy(Editor* editor, Properties* this, Split* split);
void Properties_Update(Editor* editor, Properties* this, Split* split);
void Properties_Draw(Editor* editor, Properties* this, Split* split);

static void SaveConfig(void* __this, Split* split, Toml* toml, const char* prefix) {
    Properties* this = __this;
    
    Toml_SetVar(toml, x_fmt("%s.properties.tab_index", prefix), "%d", this->subIndex);
}

static void LoadConfig(void* __this, Split* split, Toml* toml, const char* prefix) {
    Properties* this = __this;
    
    this->subIndex = Toml_GetInt(toml, "%s.properties.tab_index", prefix);
}

SplitTask gPropertiesTask = {
    .taskName   = "Properties",
    .init       = (void*)Properties_Init,
    .destroy    = (void*)Properties_Destroy,
    .update     = (void*)Properties_Update,
    .draw       = (void*)Properties_Draw,
    .saveConfig = SaveConfig,
    .loadConfig = LoadConfig,
    .size       = sizeof(Properties)
};

void Properties_BasicHeader(Split* split) {
    Element_Header(split->taskCombo, 92);
    Element_Combo(split->taskCombo);
}

///////////////////////////////////////////////////////////////////////////////

static void MenuDebug_Init(Editor* editor, void* __this, Split* split) {
    MenuDebug* this = __this;
    Scene* scene = &editor->scene;
    
    Element_Name(&this->envAmbient, "Ambient");
    Element_Name(&this->envColA, "EnvA");
    Element_Name(&this->envColB, "EnvB");
    Element_Name(&this->envFogColor, "Fog");
    
    Element_Name(&this->buttonIndoor, "Indoor");
    
    Element_Name(&this->fogFar, "Far");
    Element_Name(&this->fogNear, "Near");
    
    Element_Slider_SetParams(&this->fogNear, 0, 1000, "int");
    Element_Slider_SetParams(&this->fogFar, 100, 32000, "int");
    
    Element_Button_SetProperties(&this->killScene, "Unload Scene", 0, 0);
    Element_Button_SetProperties(&this->buttonUnused,  "Unused",         true, false);
    Element_Button_SetProperties(&this->buttonFog,     "Fog",            true, scene->state & SCENE_DRAW_FOG);
    Element_Button_SetProperties(&this->buttonCulling, "Culling",        true, scene->state & SCENE_DRAW_CULLING);
    Element_Button_SetProperties(&this->buttonColView, "Collision View", true, scene->state & SCENE_DRAW_COLLISION);
    this->killScene.align =
        this->buttonUnused.align =
        this->buttonFog.align =
        this->buttonCulling.align =
        this->buttonColView.align = NVG_ALIGN_CENTER;
}

static void MenuDebug_Update(Editor* editor, void* __this, Split* split) {
    MenuDebug* this = __this;
    Scene* scene = &editor->scene;
    SceneHeader* sceneHeader = Scene_GetSceneHeader(scene);
    EnvLightSettings* envSettings = sceneHeader ? Arli_At(&sceneHeader->envList, scene->curEnv) : NULL;
    
    Properties_BasicHeader(split);
    
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
    
    // Element_Row(NULL, 0.5f, &this->buttonIndoor, 0.5f);
    // Element_Button_SetProperties(&this->buttonIndoor, true, scene->indoorLight);
    // Element_Checkbox(&this->buttonIndoor);
    
    Element_Row(&this->envAmbient, 1.0f);
    Element_Color(&this->envAmbient);
    
    Element_Row(&this->envColA, 1.0f);
    Element_Row(&this->envColB, 1.0f);
    Element_Color(&this->envColA);
    Element_Color(&this->envColB);
    
    Element_Box(BOX_START);
    Element_Row(&this->envFogColor, 1.0f);
    Element_Color(&this->envFogColor);
    
    Element_Row(&this->fogNear, 0.5f, &this->fogFar, 0.5f);
    
    if (Element_Slider(&this->fogNear))
        if (envSettings) envSettings->fogNear = Element_Slider_GetValue(&this->fogNear);
    
    if (Element_Slider(&this->fogFar))
        if (envSettings) envSettings->fogFar = Element_Slider_GetValue(&this->fogFar);
    
    Element_Box(BOX_END);
    
    Element_Separator(false);
    
    Element_Row(Element_Text("Render"), 1.0);
    Element_Row( &this->buttonUnused, 0.5, &this->buttonCulling, 0.5);
    Element_Row( &this->buttonFog, 0.5, &this->buttonColView, 0.5);
    
    Element_Button_SetProperties(&this->buttonFog, NULL, true, scene->state & SCENE_DRAW_FOG);
    Element_Button_SetProperties(&this->buttonCulling, NULL, true, scene->state & SCENE_DRAW_CULLING);
    Element_Button_SetProperties(&this->buttonColView, NULL, true, scene->state & SCENE_DRAW_COLLISION);
    
    Element_Button(&this->buttonUnused);
    if (Element_Button(&this->buttonFog))
        Scene_SetState(scene, SCENE_DRAW_FOG, this->buttonFog.state);
    if (Element_Button(&this->buttonCulling))
        Scene_SetState(scene, SCENE_DRAW_CULLING, this->buttonCulling.state);
    if (Element_Button(&this->buttonColView))
        Scene_SetState(scene, SCENE_DRAW_COLLISION, this->buttonColView.state);
    
    Element_Separator(false);
    
    Element_Row(&this->killScene, 1.0);
    if (Element_Button(&this->killScene)) {
        Gizmo_UnselectAll(&editor->gizmo);
        Actor_UnselectAll(&editor->scene, NULL);
        Scene_Kill(&editor->scene);
        
        Element_Color_SetColor(&this->envAmbient, NULL);
        Element_Color_SetColor(&this->envColA, NULL);
        Element_Color_SetColor(&this->envColB, NULL);
        Element_Color_SetColor(&this->envFogColor, NULL);
    }
}

///////////////////////////////////////////////////////////////////////////////

void MenuActor_Init(Editor* editor, void* __this, Split* split);
void MenuActor_Update(Editor* editor, void* __this, Split* split);
void MenuRoomEnv_Init(Editor* editor, void* __this, Split* split);
void MenuRoomEnv_Update(Editor* editor, void* __this, Split* split);

///////////////////////////////////////////////////////////////////////////////

static struct {
    void  (*init)(Editor*, void*, Split*);
    void  (*update)(Editor*, void*, Split*);
    off_t offset;
    char* name;
} sSubMenuParam[] = {
    //crustify
    { MenuDebug_Init,   MenuDebug_Update,   offsetof(Properties, menuDebug),   "D" },
    { MenuActor_Init,   MenuActor_Update,   offsetof(Properties, menuActor),   "A" },
    { MenuRoomEnv_Init, MenuRoomEnv_Update, offsetof(Properties, menuRoomEnv), "R" },
    //uncrustify
};

#define SIDE_BUTTON_SIZE 24

static void* GetArg(void* ptr, int index) {
    return ((u8*)ptr) + sSubMenuParam[index].offset;
}

static Rect GetSubRect(Split* split, int index, int side) {
    Rect r;
    
    r.x = SPLIT_ELEM_X_PADDING / 2;
    r.y = SPLIT_ELEM_X_PADDING + (SIDE_BUTTON_SIZE + SPLIT_ELEM_X_PADDING) * index;
    r.w = SIDE_BUTTON_SIZE;
    r.h = SIDE_BUTTON_SIZE;
    
    if (side < 0)
        return Rect_FlipHori(r, split->dispRect);
    
    return r;
}

///////////////////////////////////////////////////////////////////////////////

static const char* GetBehaviourName(Arli* this, size_t index) {
    const char* text = Arli_At(this, index);
    
    if (!text) return "Unknown";
    return text;
}

const char sBehaviourNames[][24] = {
    "Overworld",
    "Dungeon",
    "House",
    "Unk (Dither?)",
    "Unk (Horse Cam?)",
    "Boss Room",
    
    "Default",
    "Cold",
    "Hot",
    "Hot (Timer)",
    "Comfy",
};

void Properties_Init(Editor* editor, Properties* this, Split* split) {
    this->menuRoomEnv.behaviour1 = Arli_New(char[24]);
    this->menuRoomEnv.behaviour2 = Arli_New(char[24]);
    
    Arli_Add(&this->menuRoomEnv.behaviour1, sBehaviourNames[0]);
    Arli_Add(&this->menuRoomEnv.behaviour1, sBehaviourNames[1]);
    Arli_Add(&this->menuRoomEnv.behaviour1, sBehaviourNames[2]);
    Arli_Add(&this->menuRoomEnv.behaviour1, sBehaviourNames[3]);
    Arli_Add(&this->menuRoomEnv.behaviour1, sBehaviourNames[4]);
    Arli_Add(&this->menuRoomEnv.behaviour1, sBehaviourNames[5]);
    
    Arli_Add(&this->menuRoomEnv.behaviour2, sBehaviourNames[6]);
    Arli_Add(&this->menuRoomEnv.behaviour2, sBehaviourNames[7]);
    Arli_Add(&this->menuRoomEnv.behaviour2, sBehaviourNames[8]);
    Arli_Add(&this->menuRoomEnv.behaviour2, sBehaviourNames[9]);
    Arli_Add(&this->menuRoomEnv.behaviour2, sBehaviourNames[10]);
    
    Arli_SetElemNameCallback(&this->menuRoomEnv.behaviour1, GetBehaviourName);
    Arli_SetElemNameCallback(&this->menuRoomEnv.behaviour2, GetBehaviourName);
    
    for (int i = 0; i < ArrCount(sSubMenuParam); i++)
        if (sSubMenuParam[i].init)
            sSubMenuParam[i].init(editor, GetArg(this, i), split);
}

void Properties_Destroy(Editor* editor, Properties* this, Split* split) {
    Arli_Free(&this->menuRoomEnv.behaviour1);
    Arli_Free(&this->menuRoomEnv.behaviour2);
}

void Properties_Update(Editor* editor, Properties* this, Split* split) {
    int xSplit = RectW(split->rect) - split->rect.w / 2;
    
    this->side = (xSplit < (editor->app.wdim.x / 2) ? -1 : 1);
    
    Element_RowY(SPLIT_ELEM_X_PADDING * 2);
    Element_ShiftX(SIDE_BUTTON_SIZE * this->side);
    
    if (!split->blockCursor && !editor->geo.state.blockElemInput) {
        Input* input = &editor->input;
        
        for (int i = 0; i < ArrCount(sSubMenuParam); i++) {
            Rect r = GetSubRect(split, i, this->side);
            
            if (Input_SelectClick(input, CLICK_L))
                if (Split_CursorInRect(split, &r))
                    this->subIndex = i;
        }
    }
    
    sSubMenuParam[this->subIndex].update(editor, GetArg(this, this->subIndex), split);
}

void Properties_Draw(Editor* editor, Properties* this, Split* split) {
    void* vg = editor->vg;
    Rect r = {
        0, 0, SIDE_BUTTON_SIZE + SPLIT_ELEM_X_PADDING, split->dispRect.h
    };
    
    if (this->side < 0)
        r = Rect_FlipHori(r, split->dispRect);
    Rect scissor = r;
    
    Gfx_DrawRounderRect(vg, r, Theme_GetColor(THEME_ELEMENT_DARK, 255, 1.1f));
    r.x += r.w - 1;
    r.w = 1;
    Gfx_DrawRounderRect(vg, r, Theme_GetColor(THEME_ELEMENT_LIGHT, 25, 1.25f));
    
    for (int i = 0; i < ArrCount(sSubMenuParam); i++) {
        Rect tr;
        Rect r;
        NVGcolor col = Theme_GetColor(THEME_TEXT, 255, 1.0f);
        
        tr = r = GetSubRect(split, i, this->side);
        tr.x += SPLIT_ELEM_X_PADDING / 2;
        
        if (this->subIndex == i) {
            r = Rect_WidenTo(r, (SPLIT_CTXM_DIST / 2) * this->side);
            nvgScissor(vg, UnfoldRect(scissor));
            r = Rect_WidenTo(r, SPLIT_CTXM_DIST * this->side);
            Gfx_DrawRounderOutline(vg, r, Theme_GetColor(THEME_ELEMENT_LIGHT, 25, 1.0f));
            Gfx_DrawRounderRect(vg, r, Theme_GetColor(THEME_BASE, 255, 1.0f));
            nvgResetScissor(vg);
            
            col = Theme_GetColor(THEME_PRIM, 255, 1.0f);
        }
        
        if (sSubMenuParam[i].name)
            Gfx_Text(vg, tr, NVG_ALIGN_MIDDLE | NVG_ALIGN_LEFT, col, sSubMenuParam[i].name);
    }
}
