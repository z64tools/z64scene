#include "Properties.h"

void Settings_Init(Editor* editor, Properties* this, Split* split);
void Settings_Destroy(Editor* editor, Properties* this, Split* split);
void Settings_Update(Editor* editor, Properties* this, Split* split);
void Settings_Draw(Editor* editor, Properties* this, Split* split);

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
    .init       = (void*)Settings_Init,
    .destroy    = (void*)Settings_Destroy,
    .update     = (void*)Settings_Update,
    .draw       = (void*)Settings_Draw,
    .saveConfig = SaveConfig,
    .loadConfig = LoadConfig,
    .size       = sizeof(Properties)
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
    
    Element_Separator(false);
    
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
    
    Element_Separator(false);
    
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

static void MenuActor_RefreshProperties(MenuActor* this, Actor* actor, bool set) {
    if (actor) {
        if (set) {
            actor->gizmo.refresh = true;
            
            info("refresh actor");
            
            UndoEvent* undo = Undo_New();
            Undo_Register(undo, &actor->pos, sizeof(Vec3f));
            Undo_Register(undo, &actor->rot, sizeof(Vec3s));
            Undo_Register(undo, &actor->id, sizeof(u16));
            Undo_Register(undo, &actor->param, sizeof(u16));
            Undo_Register(undo, &actor->gizmo.refresh, sizeof(u8));
            
            actor->id = shex(this->index.txt);
            actor->param = shex(this->variable.txt);
            actor->rot.x = shex(this->rotX.txt);
            actor->rot.y = shex(this->rotY.txt);
            actor->rot.z = shex(this->rotZ.txt);
            actor->pos.x = sint(this->posX.txt);
            actor->pos.y = sint(this->posY.txt);
            actor->pos.z = sint(this->posZ.txt);
        }
        
        Element_Textbox_SetText(&this->index, x_fmt("%04X", actor->id));
        Element_Textbox_SetText(&this->variable, x_fmt("%04X", actor->param));
        Element_Textbox_SetText(&this->rotX, x_fmt("%04X", (u32)(u16)actor->rot.x));
        Element_Textbox_SetText(&this->rotY, x_fmt("%04X", (u32)(u16)actor->rot.y));
        Element_Textbox_SetText(&this->rotZ, x_fmt("%04X", (u32)(u16)actor->rot.z));
        Element_Textbox_SetText(&this->posX, x_fmt("%d", (int)actor->pos.x));
        Element_Textbox_SetText(&this->posY, x_fmt("%d", (int)actor->pos.y));
        Element_Textbox_SetText(&this->posZ, x_fmt("%d", (int)actor->pos.z));
    }
}

static void MenuActor_SelectActor(Editor* editor, RoomHeader* room, Actor* actor) {
    Actor_UnselectAll(&editor->scene, room);
    Gizmo_UnselectAll(&editor->gizmo);
    
    if (!actor) return;
    
    Actor_Select(&editor->scene, actor);
    Actor_Focus(&editor->scene, actor);
    
    Gizmo_Select(&editor->gizmo, &actor->gizmo, &actor->pos, &actor->rot);
    Gizmo_Focus(&editor->gizmo, &actor->gizmo);
}

static const char* GetDictionaryName(Arli* list, size_t index) {
    DbDictionary* dict = Arli_At(list, index);
    
    if (!dict) return "Unknown";
    return dict->text;
}

static int MenuActor_Database(MenuActor* this, Actor* actor) {
    int numProp;
    DbProperty* listProp;
    
    if (!actor) return false;
    if (!(numProp = Database_NumPropertyList(actor->id))) return false;
    if (!(listProp = Database_PropertyList(actor->id))) return false;
    
    if (actor->id != this->prevIndex) {
        this->prevIndex = actor->id;
        
        for (int i = 0; i < this->num; i++)
            vfree(this->list[i].list, this->list[i].element);
        vfree(this->list);
        
        this->num = numProp;
        PropertyEntry* entry = this->list = new(PropertyEntry[this->num]);
        DbProperty* prop = listProp;
        
        for (int i = 0; i < this->num; i++, prop++, entry++) {
            entry->property = prop;
            
            if (prop->numDict) {
                entry->element = new(ElCombo);
                entry->type = PE_COMBO;
                
                Arli* list = entry->list = new(Arli);
                *list = Arli_New(DbDictionary);
                list->begin = (void*)prop->dict;
                list->max = list->num = prop->numDict;
                
                Arli_SetElemNameCallback(list, GetDictionaryName);
                Element_Combo_SetArli(entry->combo, list);
            } else {
                entry->element = new(ElTextbox);
                entry->type = PE_TEXT;
                entry->textBox->align = ALIGN_RIGHT;
                entry->textBox->size = pmask(prop->mask);
            }
            
            Element_Name(entry->el, prop->name);
        }
    }
    
    if (!this->num)
        return false;
    
    Element_Box(BOX_START);
    
    int r = 0;
    
    PropertyEntry* entry = this->list;
    for (int i = 0; i < this->num; i++, entry++) {
        if (!entry->element) continue;
        
        DbProperty* prop = entry->property;
        u16 val;
        Arli* list = entry->list;
        DbDictionary* dict;
        
        Element_Row(entry->el, 1.0f);
        Element_DisplayName(entry->el, 0.5f);
        
        switch (entry->type) {
            case PE_TEXT:
                if (Element_Textbox(entry->textBox)) {
                    r = true;
                    Actor_wmask(actor, prop->source, shex(entry->textBox->txt), prop->mask);
                }
                
                val = Actor_rmask(actor, prop->source, prop->mask);
                Element_Textbox_SetText(entry->textBox, x_fmt("%0*X", pmask(prop->mask), val));
                break;
                
            case PE_COMBO:
                if (Element_Combo(entry->combo)) {
                    r = true;
                    dict = Arli_At(list, list->cur);
                    Actor_wmask(actor, prop->source, dict->val, prop->mask);
                }
                
                val = Actor_rmask(actor, prop->source, prop->mask);
                Arli_Set(list, list->num + 1); // INVALID
                dict = prop->dict;
                for (int k = 0; k < prop->numDict; k++, dict++) {
                    if (dict->val == val) {
                        Arli_Set(list, k);
                        break;
                    }
                }
                break;
        }
    }
    
    Element_Box(BOX_END);
    
    return r;
}

static void MenuActor_Init(Editor* editor, void* __this, Split* split) {
    MenuActor* this = __this;
    RoomHeader* room = Scene_GetRoomHeader(&editor->scene, editor->scene.curRoom);
    
    Element_Combo_SetArli(&this->actorEntry, &room->actorList);
    Element_Container_SetArli(&this->objectContainer, &room->objectList, 8);
    this->objectContainer.drag = true;
    
    Actor* actor = Arli_At(&room->actorList, room->actorList.cur);
    MenuActor_RefreshProperties(this, actor, false);
    
    struct {
        ElTextbox*  box;
        const char* name;
    } boxTbl[] = {
        { &this->index,    "ID"    },
        { &this->variable, "Var"   },
        { &this->posX,     "X"     },
        { &this->posY,     "Y"     },
        { &this->posZ,     "Z"     },
        { &this->rotX,     "X"     },
        { &this->rotY,     "Y"     },
        { &this->rotZ,     "Z"     },
    };
    
    for (int i = 0; i < ArrCount(boxTbl); i++)
        boxTbl[i].box->align = ALIGN_RIGHT,
        boxTbl[i].box->size = 4,
        Element_Name(boxTbl[i].box, boxTbl[i].name);
    
    this->prevIndex = 0xFFFF;
    
    Element_Name(&this->buttonAdd, "New");
    Element_Name(&this->buttonRem, "Del");
}

static void MenuActor_Update(Editor* editor, void* __this, Split* split) {
    MenuActor* this = __this;
    RoomHeader* room = Scene_GetRoomHeader(&editor->scene, editor->scene.curRoom);
    Arli* list = this->actorEntry.arlist;
    Actor* actor = Arli_At(list, list->cur);
    int set = 0;
    int objContIndex = 0;
    
    Element_Container_SetArli(&this->objectContainer, &room->objectList, 4);
    Element_Combo_SetArli(&this->actorEntry, &room->actorList);
    
    Element_Row(Element_Text("Object List"), 1.0f);
    Element_Row(&this->objectContainer, 1.0f);
    if ((objContIndex = Element_Container(&this->objectContainer)) > -1) {
        u16* obj = Arli_At(&room->objectList, objContIndex);
        
        if (obj) *obj = shex(this->objectContainer.textBox.txt);
    }
    
    Element_Separator(false);
    
    Element_Row(&this->actorEntry, 0.5f, &this->buttonAdd, 0.25f, &this->buttonRem, 0.25f);
    
    Element_Separator(false);
    
    Element_Row(&this->index, 0.5f, &this->variable, 0.5f);
    
    Element_Box(BOX_START);
    Element_Row(Element_Text("Position"), 0.5f, Element_Text("Rotation"), 0.5f);
    Element_Row(&this->posX, 0.5f, &this->rotX, 0.5f);
    Element_Row(&this->posY, 0.5f, &this->rotY, 0.5f);
    Element_Row(&this->posZ, 0.5f, &this->rotZ, 0.5f);
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
    Element_Condition(&this->buttonRem, actor != NULL);
    
    Element_DisplayName(&this->index, -1);
    Element_DisplayName(&this->variable, -1);
    Element_DisplayName(&this->posX, -1);
    Element_DisplayName(&this->posY, -1);
    Element_DisplayName(&this->posZ, -1);
    Element_DisplayName(&this->rotX, -1);
    Element_DisplayName(&this->rotY, -1);
    Element_DisplayName(&this->rotZ, -1);
    
    set += !!Element_Textbox(&this->index);
    set += !!Element_Textbox(&this->variable);
    set += !!Element_Textbox(&this->rotX);
    set += !!Element_Textbox(&this->rotY);
    set += !!Element_Textbox(&this->rotZ);
    set += !!Element_Textbox(&this->posX);
    set += !!Element_Textbox(&this->posY);
    set += !!Element_Textbox(&this->posZ);
    
    if (Element_Button(&this->buttonAdd) && !set) {
        Actor new = { .id = 0x0015 };
        
        actor = Arli_Insert(list, list->cur, 1, &new);
        MenuActor_SelectActor(editor, room, actor);
        set = false;
    }
    
    if (Element_Button(&this->buttonRem) && !set) {
        _log("remove");
        Arli_Remove(list, list->cur, 1);
        _log("at");
        actor = Arli_At(list, list->cur);
        
        MenuActor_SelectActor(editor, room, actor);
        set = false;
    }
    
    if (Element_Combo(&this->actorEntry)) {
        Actor* actor = Arli_At(list, list->cur);
        
        MenuActor_SelectActor(editor, room, actor);
    }
    
    profi_start(2);
    MenuActor_RefreshProperties(this, actor, set);
    MenuActor_Database(this, actor);
    profi_stop(2);
}

///////////////////////////////////////////////////////////////////////////////

static struct {
    void  (*init)(Editor*, void*, Split*);
    void  (*update)(Editor*, void*, Split*);
    off_t offset;
    char* name;
} sSubMenuParam[] = {
    //crustify
    { MenuDebug_Init, MenuDebug_Update, offsetof(Properties, menuDebug), "D" },
    { MenuActor_Init, MenuActor_Update, offsetof(Properties, menuActor), "A" },
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
    
    if (!split->blockCursor && !editor->geo.state.blockElemInput) {
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
    
    Gfx_DrawRounderRect(vg, r, Theme_GetColor(THEME_ELEMENT_DARK, 255, 1.1f));
    r.x += r.w - 1;
    r.w = 1;
    Gfx_DrawRounderRect(vg, r, Theme_GetColor(THEME_ELEMENT_LIGHT, 25, 1.25f));
    
    for (int i = 0; i < ArrCount(sSubMenuParam); i++) {
        Rect tr;
        Rect r;
        NVGcolor col = Theme_GetColor(THEME_TEXT, 255, 1.0f);
        
        tr = r = GetSubRect(split, i);
        tr.x += SPLIT_ELEM_X_PADDING / 2;
        
        if (this->subIndex == i) {
            r.w += SPLIT_ELEM_X_PADDING / 2;
            nvgScissor(vg, UnfoldRect(scissor));
            r.w += SPLIT_ELEM_X_PADDING;
            Gfx_DrawRounderOutline(vg, r, Theme_GetColor(THEME_ELEMENT_LIGHT, 25, 1.0f));
            Gfx_DrawRounderRect(vg, r, Theme_GetColor(THEME_BASE, 255, 1.0f));
            nvgResetScissor(vg);
            
            r.w -= SPLIT_ELEM_X_PADDING;
            
            col = Theme_GetColor(THEME_PRIM, 255, 1.0f);
        }
        
        if (sSubMenuParam[i].name)
            Gfx_Text(vg, tr, NVG_ALIGN_MIDDLE | NVG_ALIGN_LEFT, col, sSubMenuParam[i].name);
    }
}
