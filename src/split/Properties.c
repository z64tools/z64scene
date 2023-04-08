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

static void BasicHeader(Split* split) {
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
    
    BasicHeader(split);
    
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
    int r = 0;
    
    if (!actor || !(actor->state & ACTOR_SELECTED)) return false;
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
            } else if (smask_bit(prop->mask) == 1) {
                entry->element = new(ElCheckbox);
                entry->type = PE_CHECK;
            } else {
                entry->element = new(ElTextbox);
                entry->type = PE_TEXT;
                entry->textBox->size = smask_byte(prop->mask);
            }
            
            Element_SetNameLerp(entry->check, 0.5f);
            Element_Name(entry->check, prop->name);
        }
    }
    
    if (!this->num)
        return false;
    
    PropertyEntry* entry = this->list;
    
    for (int i = 0; i < this->num; i++, entry++) {
        if (!entry->element) continue;
        
        DbProperty* prop = entry->property;
        u16 val;
        Arli* list = entry->list;
        DbDictionary* dict;
        
        Element_Row(entry->check, 1.0f);
        
        switch (entry->type) {
            case PE_TEXT:
                if (Element_Textbox(entry->textBox)) {
                    r = true;
                    Actor_wmask(actor, prop->source, shex(entry->textBox->txt), prop->mask);
                }
                
                val = Actor_rmask(actor, prop->source, prop->mask);
                Element_Textbox_SetText(entry->textBox, x_fmt("%0*X", smask_byte(prop->mask), val));
                break;
                
            case PE_COMBO:
                if (Element_Combo(entry->combo)) {
                    r = true;
                    dict = Arli_At(list, list->cur);
                    Actor_wmask(actor, prop->source, dict->val, prop->mask);
                }
                
                val = Actor_rmask(actor, prop->source, prop->mask);
                list->cur = -1; // Invalid Entry
                dict = prop->dict;
                
                for (int k = 0; k < prop->numDict; k++, dict++) {
                    if (dict->val == val) {
                        Arli_Set(list, k);
                        break;
                    }
                }
                break;
                
            case PE_CHECK:
                if (Element_Checkbox(entry->check))
                    Actor_wmask(actor, prop->source, entry->check->element.toggle, prop->mask);
                
                entry->check->element.toggle = Actor_rmask(actor, prop->source, prop->mask);
        }
    }
    
    return r;
}

static void MenuActor_Init(Editor* editor, void* __this, Split* split) {
    MenuActor* this = __this;
    RoomHeader* room = Scene_GetRoomHeader(&editor->scene, editor->scene.curRoom);
    
    Element_Combo_SetArli(&this->actorEntry, room ? &room->actorList : NULL);
    
    Actor* actor = room ? Arli_At(&room->actorList, room->actorList.cur) : NULL;
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
        boxTbl[i].box->size = 4,
        Element_Name(boxTbl[i].box, boxTbl[i].name);
    
    this->prevIndex = 0xFFFF;
    this->actorEntry.showDecID = true;
    this->buttonAdd.element.colOvrdLight = THEME_NEW;
    this->buttonRem.element.colOvrdLight = THEME_DELETE;
    
    this->buttonAdd.align = NVG_ALIGN_CENTER;
    this->buttonRem.align = NVG_ALIGN_CENTER;
    this->refreshDatabase.align = NVG_ALIGN_CENTER;
    Element_Button_SetProperties(&this->buttonAdd, "New", 0, 0);
    Element_Button_SetProperties(&this->buttonRem, "Del", 0, 0);
    Element_Button_SetProperties(&this->refreshDatabase, "Refresh Database", 0, 0);
}

static void MenuActor_Update(Editor* editor, void* __this, Split* split) {
    MenuActor* this = __this;
    Input* input = &editor->input;
    RoomHeader* room = Scene_GetRoomHeader(&editor->scene, editor->scene.curRoom);
    
    Element_Combo_SetArli(&this->actorEntry, room ? &room->actorList : NULL);
    Arli* list = this->actorEntry.arlist;
    Actor* actor = list ? Arli_At(list, list->cur) : NULL;
    int set = 0;
    
    Element_Header(split->taskCombo, 92, &this->refreshDatabase, 120);
    Element_Combo(split->taskCombo);
    if (Element_Button(&this->refreshDatabase)) {
        Database_Refresh();
        this->prevIndex = 0xFFFF;
    }
    
    Element_Separator(false);
    
    Element_Row(&this->actorEntry, 0.5f, &this->buttonAdd, 0.25f, &this->buttonRem, 0.25f);
    
    Element_Separator(false);
    
    Element_Row(&this->index, 0.5f, &this->variable, 0.5f);
    
    if (Element_Box(BOX_START, &this->panelPosRot, "Transforms")) {
        Element_Row(&this->posX, 0.5f, &this->rotX, 0.5f);
        Element_Row(&this->posY, 0.5f, &this->rotY, 0.5f);
        Element_Row(&this->posZ, 0.5f, &this->rotZ, 0.5f);
        
        Element_Condition(&this->rotX, actor != NULL && actor->state & ACTOR_SELECTED);
        Element_Condition(&this->rotY, actor != NULL && actor->state & ACTOR_SELECTED);
        Element_Condition(&this->rotZ, actor != NULL && actor->state & ACTOR_SELECTED);
        Element_Condition(&this->posX, actor != NULL && actor->state & ACTOR_SELECTED);
        Element_Condition(&this->posY, actor != NULL && actor->state & ACTOR_SELECTED);
        Element_Condition(&this->posZ, actor != NULL && actor->state & ACTOR_SELECTED);
        
        set += !!Element_Textbox(&this->rotX);
        set += !!Element_Textbox(&this->rotY);
        set += !!Element_Textbox(&this->rotZ);
        set += !!Element_Textbox(&this->posX);
        set += !!Element_Textbox(&this->posY);
        set += !!Element_Textbox(&this->posZ);
        
    }
    Element_Box(BOX_END, &this->panelPosRot);
    
    Element_Condition(&this->buttonAdd,  list != NULL);
    Element_Condition(&this->actorEntry, actor != NULL);
    Element_Condition(&this->buttonRem,  actor != NULL && actor->state & ACTOR_SELECTED);
    Element_Condition(&this->index,      actor != NULL && actor->state & ACTOR_SELECTED);
    Element_Condition(&this->variable,   actor != NULL && actor->state & ACTOR_SELECTED);
    
    if (editor->dataContextMenu) {
        int ret;
        Element_Disable(&this->index);
        
        switch (DatabaseSearch_State(&ret)) {
            case 1:
                strcpy(this->index.txt, x_fmt("%04X", ret));
                set++;
                break;
                
            case -1:
                DatabaseSearch_Free();
                break;
                
            default:
                break;
        }
    }
    
    if (Element_Textbox(&this->index)) {
        set++;
        
    } else if (Element_Operatable(&this->index)) {
        info("operatable");
        if (Input_SelectClick(input, CLICK_R))
            DatabaseSearch_New(Rect_AddPos(this->index.element.rect, split->dispRect), actor->id);
    }
    
    set += !!Element_Textbox(&this->variable);
    
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
    
    MenuActor_RefreshProperties(this, actor, set);
    if (Element_Box(BOX_START, &this->panelProperties, "Properties"))
        MenuActor_Database(this, actor);
    Element_Box(BOX_END, &this->panelProperties);
}

///////////////////////////////////////////////////////////////////////////////

static void MenuRoomEnv_Init(Editor* editor, void* __this, Split* split) {
    MenuRoomEnv* this = __this;
    
    Element_Name(&this->sliderTimeSpeed, "Speed");
    Element_Name(&this->sliderEcho, "Echo");
    Element_Name(&this->comboBeha1, "Env 1");
    Element_Name(&this->comboBeha2, "Env 2");
    Element_Button_SetProperties(&this->buttonGlTime, "Global Time", true, false);
    Element_Button_SetProperties(&this->buttonSkybox, "Skybox", true, false);
    Element_Button_SetProperties(&this->buttonSunMoon, "Sun & Moon", true, false);
    
    this->textboxHour.size = 2;
    this->textboxMin.size = 2;
    this->buttonGlTime.align = NVG_ALIGN_CENTER;
    this->textboxMin.align = NVG_ALIGN_LEFT;
    this->buttonSkybox.align = NVG_ALIGN_CENTER;
    this->buttonSunMoon.align = NVG_ALIGN_CENTER;
    
    Element_Slider_SetParams(&this->sliderTimeSpeed, 0, 0xFE, "int");
    Element_Slider_SetParams(&this->sliderEcho, 0, 0xFF, "int");
    
    Element_Combo_SetArli(&this->comboBeha1, &this->behaviour1);
    Element_Combo_SetArli(&this->comboBeha2, &this->behaviour2);
    
    Element_SetNameLerp(&this->sliderTimeSpeed, 0.25f);
    Element_SetNameLerp(&this->sliderEcho, 0.25f);
    Element_SetNameLerp(&this->comboBeha1, 0.25f);
    Element_SetNameLerp(&this->comboBeha2, 0.25f);
}

static void MenuRoomEnv_Update(Editor* editor, void* __this, Split* split) {
    MenuRoomEnv* this = __this;
    RoomHeader* room = Scene_GetRoomHeader(&editor->scene, editor->scene.curRoom);
    
    BasicHeader(split);
    
    if (Element_Box(BOX_START, &this->panelTime, "Time")) {
        Element_Row(&this->textboxHour, 0.25f, &this->textboxMin, 0.25f, &this->buttonGlTime, 0.5f);
        Element_Row(&this->sliderTimeSpeed, 1.0f);
        
        Element_Condition(&this->buttonGlTime,  room != NULL);
        
        if (room) {
            Element_Button_SetProperties(&this->buttonGlTime, NULL, true, room->timeGlobal);
            Element_Textbox_SetText(&this->textboxHour, x_fmt("%d", room->timeHour));
            Element_Textbox_SetText(&this->textboxMin, x_fmt("%d", room->timeMinute));
            Element_Slider_SetValue(&this->sliderTimeSpeed, room->timeSpeed);
        }
        
        if (Element_Button(&this->buttonGlTime))
            room->timeGlobal = this->buttonGlTime.state;
        
        Element_Condition(&this->textboxHour,     room != NULL && !room->timeGlobal);
        Element_Condition(&this->textboxMin,      room != NULL && !room->timeGlobal);
        Element_Condition(&this->sliderTimeSpeed, room != NULL);
        
        if (Element_Textbox(&this->textboxHour))
            room->timeHour = clamp(sint(this->textboxHour.txt), 0, 24);
        if (Element_Textbox(&this->textboxMin))
            room->timeMinute = clamp(sint(this->textboxMin.txt), 0, 59);
        if (Element_Slider(&this->sliderTimeSpeed))
            room->timeSpeed = Element_Slider_GetValue(&this->sliderTimeSpeed);
        
    }
    Element_Box(BOX_END, &this->panelTime);
    
    if (Element_Box(BOX_START, &this->panelEnvironment, "Environment")) {
        Element_Row(&this->buttonSkybox, 0.5f, &this->buttonSunMoon, 0.5f);
        Element_Row(&this->comboBeha1, 1.0f);
        Element_Row(&this->comboBeha2, 1.0f);
        Element_Row(&this->sliderEcho, 1.0f);
        
        if (room) {
            Element_Button_SetProperties(&this->buttonSkybox, NULL, true, !room->skyBox.disableSky);
            Element_Button_SetProperties(&this->buttonSunMoon, NULL, true, !room->skyBox.disableSunMoon);
            Element_Slider_SetValue(&this->sliderEcho, room->echo);
            
            Arli_Set(&this->behaviour1, room->behaviour.val1);
            Arli_Set(&this->behaviour2, room->behaviour.val2);
        }
        
        Element_Condition(&this->comboBeha1,    room != NULL);
        Element_Condition(&this->comboBeha2,    room != NULL);
        Element_Condition(&this->buttonSkybox,  room != NULL);
        Element_Condition(&this->buttonSunMoon, room != NULL);
        Element_Condition(&this->sliderEcho,    room != NULL);
        
        if (Element_Button(&this->buttonSkybox))
            room->skyBox.disableSky = !this->buttonSkybox.state;
        if (Element_Button(&this->buttonSunMoon))
            room->skyBox.disableSunMoon = !this->buttonSunMoon.state;
        if (Element_Slider(&this->sliderEcho))
            room->echo = Element_Slider_GetValue(&this->sliderEcho);
        
        if (Element_Combo(&this->comboBeha1))
            room->behaviour.val1 = this->behaviour1.cur;
        if (Element_Combo(&this->comboBeha2))
            room->behaviour.val2 = this->behaviour2.cur;
    }
    Element_Box(BOX_END, &this->panelEnvironment);
}

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

void Settings_Init(Editor* editor, Properties* this, Split* split) {
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

void Settings_Destroy(Editor* editor, Properties* this, Split* split) {
    Arli_Free(&this->menuRoomEnv.behaviour1);
    Arli_Free(&this->menuRoomEnv.behaviour2);
}

void Settings_Update(Editor* editor, Properties* this, Split* split) {
    Element_RowY(SPLIT_ELEM_X_PADDING * 2);
    Element_ShiftX(SIDE_BUTTON_SIZE);
    
    if (!split->blockCursor && !editor->geo.state.blockElemInput) {
        Input* input = &editor->input;
        
        for (int i = 0; i < ArrCount(sSubMenuParam); i++) {
            Rect r = GetSubRect(split, i);
            
            if (Input_SelectClick(input, CLICK_L))
                if (Split_CursorInRect(split, &r))
                    this->subIndex = i;
        }
    }
    
    sSubMenuParam[this->subIndex].update(editor, GetArg(this, this->subIndex), split);
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
