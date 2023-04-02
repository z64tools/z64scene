#include "Properties.h"

void Settings_Init(Editor* editor, Properties* this, Split* split);
void Settings_Destroy(Editor* editor, Properties* this, Split* split);
void Settings_Update(Editor* editor, Properties* this, Split* split);
void Settings_Draw(Editor* editor, Properties* this, Split* split);

SplitTask gPropertiesTask = {
    .taskName = "Properties",
    .init     = (void*)Settings_Init,
    .destroy  = (void*)Settings_Destroy,
    .update   = (void*)Settings_Update,
    .draw     = (void*)Settings_Draw,
    .size     = sizeof(Properties)
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

static inline void MenuActor_RefreshProperties(MenuActor* this, Actor* actor, bool set) {
    if (actor) {
        if (set) {
            info("refresh actor");
            
            UndoEvent* undo = Undo_New();
            Undo_Register(undo, actor, sizeof(u16[2]) + sizeof(Vec3f) + sizeof(Vec3s));
            
            actor->id = shex(this->index.txt);
            actor->param = shex(this->variable.txt);
            actor->rot.x = shex(this->rotX.txt);
            actor->rot.y = shex(this->rotY.txt);
            actor->rot.z = shex(this->rotZ.txt);
            actor->pos.x = sint(this->posX.txt);
            actor->pos.y = sint(this->posY.txt);
            actor->pos.z = sint(this->posZ.txt);
            actor->gizmo.refresh = true;
        }
        
        Element_Textbox_SetText(&this->index, x_fmt("%04X", actor->id));
        Element_Textbox_SetText(&this->variable, x_fmt("%04X", actor->param));
        Element_Textbox_SetText(&this->rotX, x_fmt("%4X", (u32)(u16)actor->rot.x));
        Element_Textbox_SetText(&this->rotY, x_fmt("%4X", (u32)(u16)actor->rot.y));
        Element_Textbox_SetText(&this->rotZ, x_fmt("%4X", (u32)(u16)actor->rot.z));
        Element_Textbox_SetText(&this->posX, x_fmt("%d", (int)actor->pos.x));
        Element_Textbox_SetText(&this->posY, x_fmt("%d", (int)actor->pos.y));
        Element_Textbox_SetText(&this->posZ, x_fmt("%d", (int)actor->pos.z));
    }
}

static void MenuActor_Init(Editor* editor, void* __this, Split* split) {
    MenuActor* this = __this;
    RoomHeader* room = Scene_GetRoomHeader(&editor->scene, editor->scene.curRoom);
    
    Element_Combo_SetArli(&this->actorEntry, &room->actorList);
    
    Actor* actor = Arli_At(&room->actorList, room->actorList.cur);
    MenuActor_RefreshProperties(this, actor, false);
    
    this->index.align = ALIGN_RIGHT;
    this->variable.align = ALIGN_RIGHT;
    this->posX.align = ALIGN_RIGHT;
    this->posY.align = ALIGN_RIGHT;
    this->posZ.align = ALIGN_RIGHT;
    this->rotX.align = ALIGN_RIGHT;
    this->rotY.align = ALIGN_RIGHT;
    this->rotZ.align = ALIGN_RIGHT;
    
    Element_Name(&this->buttonAdd, "New");
    Element_Name(&this->buttonRem, "Del");
}

static void MenuActor_Update(Editor* editor, void* __this, Split* split) {
    MenuActor* this = __this;
    RoomHeader* room = Scene_GetRoomHeader(&editor->scene, editor->scene.curRoom);
    Arli* list = this->actorEntry.arlist;
    Actor* actor = Arli_At(list, list->cur);
    int set = 0;
    
    Element_Combo_SetArli(&this->actorEntry, &room->actorList);
    
    Element_Box(BOX_START);
    Element_Row(&this->actorEntry, 0.5f, &this->buttonAdd, 0.25f, &this->buttonRem, 0.25f);
    Element_Separator(true);
    Element_Row(Element_Text("ID"), 0.15f, &this->index, 0.35f, Element_Text("Var"), 0.15f, &this->variable, 0.35f);
    
    Element_Box(BOX_START);
    Element_Row(Element_Text("PosXYZ"), 0.5f, Element_Text("RotXYZ"), 0.5f);
    Element_Row(&this->posX, 0.5f, &this->rotX, 0.5f);
    Element_Row(&this->posY, 0.5f, &this->rotY, 0.5f);
    Element_Row(&this->posZ, 0.5f, &this->rotZ, 0.5f);
    Element_Box(BOX_END);
    Element_Box(BOX_END);
    
    Element_Condition(&this->actorEntry, actor != NULL);
    Element_Condition(&this->index, actor != NULL);
    Element_Condition(&this->variable, actor != NULL);
    Element_Condition(&this->rotX, actor != NULL);
    Element_Condition(&this->rotY, actor != NULL);
    Element_Condition(&this->rotZ, actor != NULL);
    Element_Condition(&this->posX, actor != NULL);
    Element_Condition(&this->posY, actor != NULL);
    Element_Condition(&this->posZ, actor != NULL);
    Element_Condition(&this->buttonAdd, actor != NULL);
    Element_Condition(&this->buttonRem, actor != NULL);
    
    set += !!Element_Textbox(&this->index);
    set += !!Element_Textbox(&this->variable);
    set += !!Element_Textbox(&this->rotX);
    set += !!Element_Textbox(&this->rotY);
    set += !!Element_Textbox(&this->rotZ);
    set += !!Element_Textbox(&this->posX);
    set += !!Element_Textbox(&this->posY);
    set += !!Element_Textbox(&this->posZ);
    set += !!Element_Button(&this->buttonAdd);
    set += !!Element_Button(&this->buttonRem);
    
    if (Element_Combo(&this->actorEntry)) {
        
    }
    
    MenuActor_RefreshProperties(this, actor, set);
}

///////////////////////////////////////////////////////////////////////////////

static struct {
    void  (*init)(Editor*, void*, Split*);
    void  (*update)(Editor*, void*, Split*);
    off_t offset;
} sSubMenuParam[] = {
    //crustify
    { MenuDebug_Init, MenuDebug_Update, offsetof(Properties, menuDebug) },
    { MenuActor_Init, MenuActor_Update, offsetof(Properties, menuActor) },
    { NULL,           MenuDebug_Update, offsetof(Properties, menuDebug) },
    { NULL,           MenuDebug_Update, offsetof(Properties, menuDebug) },
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

void Settings_Init(Editor* editor, Properties* this, Split* split) {
    for (int i = 0; i < ArrCount(sSubMenuParam); i++)
        if (sSubMenuParam[i].init)
            sSubMenuParam[i].init(editor, GetArg(this, i), split);
}

void Settings_Destroy(Editor* editor, Properties* this, Split* split) {
}

void Settings_Update(Editor* editor, Properties* this, Split* split) {
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

void Settings_Draw(Editor* editor, Properties* this, Split* split) {
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
