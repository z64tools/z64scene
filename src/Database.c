#include "Database.h"
#include "Editor.h"

typedef struct {
    u16 index;
    u16 object;
    const char* name;
    
    DbVariable* variable;
    int numVariable;
    
    DbProperty* property;
    int numProperty;
} ActorPropertyEntry;

ActorPropertyEntry** gDatabaseActor;
int gDatabaseNum;

////////////////////////////////////////////////////////////////////////////////

static void FreeDbDict(DbDictionary* this, int num) {
    for (int i = 0; i < num; i++, this++)
        vfree(this->text);
}

////////////////////////////////////////////////////////////////////////////////

static void ActorPropEnt_ParseVariable(Toml* toml, ActorPropertyEntry* entry, int i) {
    entry->numVariable = Toml_ArrCount(toml, "actor[%d].variable", i);
    if (!entry->numVariable) return;
    
    DbVariable* v = entry->variable = new(DbVariable[entry->numVariable]);
    
    for (int k = 0; k < entry->numVariable; k++, v++) {
        v->variable = Toml_GetInt(toml, "actor[%d].variable[%d][0]", i, k);
        v->text = Toml_GetStr(toml, "actor[%d].variable[%d][1]", i, k);
    }
}

static void ActorPropEnt_ParsePropDict(Toml* toml, DbProperty* p, int i, int k) {
    p->numDict = Toml_ArrCount(toml, "actor[%d].property[%d].variable", i, k);
    if (!p->numDict) return;
    DbDictionary* de = p->dict = new(DbDictionary[p->numDict]);
    
    for (int j = 0; j < p->numDict; j++, de++) {
        de->val = Toml_GetInt(toml, "actor[%d].property[%d].variable[%d][0]", i, k, j);
        de->text = Toml_GetStr(toml, "actor[%d].property[%d].variable[%d][1]", i, k, j);
        info("%04X, %s", de->val, de->text);
    }
}

static void ActorPropEnt_ParseProperty(Toml* toml, ActorPropertyEntry* entry, int i) {
    entry->numProperty = Toml_ArrCount(toml, "actor[%d].property", i);
    if (!entry->numProperty) return;
    
    DbProperty* p = entry->property = new(DbProperty[entry->numProperty]);
    
    for (int k = 0; k < entry->numProperty; k++, p++) {
        const char* src;
        const struct {
            char* text;
            int   value;
        } sourceTbl[] = {
            { "var",   DB_VAR   },
            { "pos_x", DB_POSX  },
            { "pos_y", DB_POSY  },
            { "pos_z", DB_POSZ  },
            { "rot_x", DB_ROTX  },
            { "rot_y", DB_ROTY  },
            { "rot_z", DB_ROTZ  },
        };
        
        p->name = Toml_GetStr(toml, "actor[%d].property[%d].name", i, k);
        p->mask = Toml_GetInt(toml, "actor[%d].property[%d].mask", i, k);
        src = Toml_GetStr(toml, "actor[%d].property[%d].source", i, k);
        
        if (!p->mask) {
            errr(
                "" PRNT_REDD "D a t a b a s e   E r r o r !\n" PRNT_RSET
                "Null Mask"
                "\n"
                "[[" PRNT_BLUE "actor" PRNT_RSET "]]\n"
                "\tindex = 0x%04X\n"
                "\tname = \"%s\"\n"
                "\t[[" PRNT_BLUE "actor.property" PRNT_RSET "]]\n"
                "\t\tname = \"%s\"\n"
                "\t\tmask = 0x%04X\n"
                "\t\tsource = \"%s\"\n",
                entry->index, entry->name,
                p->name, p->mask, src);
        }
        
        for (int j = 0; src && j < ArrCount(sourceTbl); j++)
            if (streq(src, sourceTbl[j].text))
                p->source = sourceTbl[j].value;
        vfree(src);
        
        ActorPropEnt_ParsePropDict(toml, p, i, k);
    }
}

////////////////////////////////////////////////////////////////////////////////

typedef struct {
    const char*   expected;
    const char*   name;
    DbDictionary* dict;
    int  numDict;
    Arli arli;
} ScenePropertyEntry;

ScenePropertyEntry sSceneEntry[MENUDATA_MAX] = {
    [MENUDATA_FILE].expected       = "file",
    [MENUDATA_OPTIONS].expected    = "options",
    [MENUDATA_BEHAVIOR_1].expected = "behaviour1",
    [MENUDATA_BEHAVIOR_2].expected = "behaviour2",
};

////////////////////////////////////////////////////////////////////////////////

const char* DbDictionary_GetArliIndex(Arli* arli, size_t index) {
    DbDictionary* dict = Arli_At(arli, index);
    
    _log("get index: %d", index);
    if (!dict) return "Unknown";
    return dict->text;
}

void DbDictionary_SetArliIndex(Arli* arli, size_t index) {
    DbDictionary* dict = Arli_Head(arli);
    DbDictionary* end = dict + arli->num;
    
    arli->cur = -1;
    
    for (; dict < end; dict++) {
        if (dict->val == index) {
            arli->cur = Arli_IndexOf(arli, dict);
            return;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Database_Init() {
    Toml toml = Toml_New();
    const char* actor_property_toml = "resources/actor_property.toml";
    const char* scene_property_toml = "resources/scene_property.toml";
    
    if (!sys_stat(scene_property_toml))
        errr("Missing Resource: %s", scene_property_toml);
    
    if (sys_stat(actor_property_toml)) {
        Toml_Load(&toml, actor_property_toml);
        
        int num = Toml_ArrCount(&toml, "actor[]");
        
        for (int i = 0; i < num; i++) {
            int index = Toml_GetInt(&toml, "actor[%d].index", i);
            
            gDatabaseNum = Max(gDatabaseNum, index);
        }
        
        gDatabaseActor = new(ActorPropertyEntry[gDatabaseNum]);
        
        for (int i = 0; i < num; i++) {
            ActorPropertyEntry* entry = new(ActorPropertyEntry);
            
            entry->index = Toml_GetInt(&toml, "actor[%d].index", i);
            entry->name = Toml_GetStr(&toml, "actor[%d].name", i);
            entry->object = 0xFFFF;
            
            _log(
                "[[" PRNT_BLUE "actor" PRNT_RSET "]]\n"
                "\tindex = 0x%04X\n"
                "\tname = \"%s\"\n",
                entry->index, entry->name);
            
            if (Toml_Var(&toml, "actor[%d].object", i))
                entry->object = Toml_GetInt(&toml, "actor[%d].object", i);
            
            _log("actor[%d].variable", i);
            ActorPropEnt_ParseVariable(&toml, entry, i);
            _log("actor[%d].property", i);
            ActorPropEnt_ParseProperty(&toml, entry, i);
            
            gDatabaseActor[entry->index] = entry;
        }
        
        Toml_Free(&toml);
    }
    
    ScenePropertyEntry* entry = sSceneEntry;
    Toml_Load(&toml, scene_property_toml);
    
    for (int i = 0; i < ArrCount(sSceneEntry); i++, entry++) {
        entry->name = Toml_GetStr(&toml, "%s.name", entry->expected);
        entry->numDict = Toml_ArrCount(&toml, "%s.variable", entry->expected);
        entry->arli = Arli_New(DbDictionary);
        
        DbDictionary* dict = entry->dict = new(DbDictionary[entry->numDict]);
        
        entry->arli.num = entry->numDict;
        entry->arli.begin = (void*)dict;
        entry->arli.elemSize = sizeof(DbDictionary);
        
        Arli_SetElemNameCallback(&entry->arli, DbDictionary_GetArliIndex);
        
        for (int k = 0; k < entry->numDict; k++, dict++) {
            dict->val = Toml_GetInt(&toml, "%s.variable[%d][0]", entry->expected, k);
            dict->text = Toml_GetStr(&toml, "%s.variable[%d][1]", entry->expected, k);
        }
    }
    
    Toml_Free(&toml);
}

void Database_Free() {
    _log("Free");
    
    for (int i = 0; i < gDatabaseNum; i++) {
        ActorPropertyEntry* e = gDatabaseActor[i];
        
        if (!e) continue;
        
        DbVariable* vari = e->variable;
        
        for (int j = 0; j < e->numVariable; j++, vari++)
            vfree(vari->text);
        
        DbProperty* prop = e->property;
        
        for (int j = 0; j < e->numProperty; j++, prop++) {
            FreeDbDict(prop->dict, prop->numDict);
            vfree(prop->name, prop->dict);
        }
        
        vfree(e->name, e->variable, e->property, e);
    }
    
    ScenePropertyEntry* entry = sSceneEntry;
    for (int i = 0; i < ArrCount(sSceneEntry); i++, entry++) {
        DbDictionary* dict = entry->dict;
        
        for (int k = 0; k < entry->numDict; k++, dict++)
            vfree(dict->text);
        vfree(entry->name, entry->dict);
        memset(&entry->arli, 0, sizeof(Arli));
    }
    
    vfree(gDatabaseActor);
    gDatabaseNum = 0;
}

void Database_Refresh() {
    Database_Free();
    Database_Init();
}

////////////////////////////////////////////////////////////////////////////////

typedef struct ActorSearchContextMenu {
    GeoGrid   geo;
    Split     split;
    ElTextbox textboxSearch;
    
    ActorPropertyEntry* entryList;
    int numEntry;
    int change;
    int current;
    int init;
} ActorSearchContextMenu;

static void ActorSearchContextMenu_Init(GeoGrid* __no_no, ContextMenu* contextMenu) {
    ActorSearchContextMenu* this = contextMenu->udata;
    
    GeoGrid_Init(&this->geo, &GetEditor()->app, NULL);
    
    this->textboxSearch.clearIcon = true;
    this->textboxSearch.align = NVG_ALIGN_LEFT;
    this->entryList = new(ActorPropertyEntry[gDatabaseNum]);
    
    contextMenu->rect.w = 128 + 64;
    contextMenu->rect.h = SPLIT_ELEM_X_PADDING * 3 + SPLIT_TEXT_H * 17;
}

static void ActorSearchContextMenu_Draw(GeoGrid* __no_no, ContextMenu* contextMenu) {
    ActorSearchContextMenu* this = contextMenu->udata;
    GeoGrid* geo = &this->geo;
    void* vg = geo->vg;
    Input* input = geo->input;
    int focusSlot = -1;
    
    DummySplit_Push(geo, &this->split, contextMenu->rect);
    
    this->textboxSearch.element.rect = contextMenu->rect;
    this->textboxSearch.element.rect.h = SPLIT_TEXT_H;
    this->textboxSearch.element.rect.x += SPLIT_ELEM_X_PADDING;
    this->textboxSearch.element.rect.y += SPLIT_ELEM_X_PADDING;
    this->textboxSearch.element.rect.w -= SPLIT_ELEM_X_PADDING * 2;
    
    Element_Textbox(&this->textboxSearch);
    
    if (this->textboxSearch.modified || !this->init) {
        bool textMatch = strlen(this->textboxSearch.txt) ? true : false;
        
        focusSlot = 0;
        this->numEntry = 0;
        
        for (int i = 0; i < gDatabaseNum; i++) {
            if (!gDatabaseActor[i])
                continue;
            
            if (!textMatch) {
                this->entryList[this->numEntry++] = *gDatabaseActor[i];
            } else {
                if (stristr(gDatabaseActor[i]->name, this->textboxSearch.txt))
                    this->entryList[this->numEntry++] = *gDatabaseActor[i];
            }
            
            if (!this->init && this->current == this->entryList[this->numEntry - 1].index)
                focusSlot = this->numEntry - 1;
        }
        
        ScrollBar_Init(&contextMenu->scroll, this->numEntry, SPLIT_TEXT_H);
    }
    
    Rect mainr = contextMenu->rect;
    
    mainr.x += SPLIT_ELEM_X_PADDING;
    mainr.w -= SPLIT_ELEM_X_PADDING * 2;
    mainr.y += SPLIT_ELEM_X_PADDING * 2 + SPLIT_TEXT_H;
    mainr.h = contextMenu->rect.h - SPLIT_ELEM_X_PADDING * 3 - SPLIT_TEXT_H;
    
    Gfx_DrawRounderOutline(vg, mainr, Theme_GetColor(THEME_ELEMENT_LIGHT, 255, 1.0f));
    Gfx_DrawRounderRect(vg, mainr, Theme_GetColor(THEME_ELEMENT_DARK, 255, 1.0f));
    
    ScrollBar_FocusSlot(&contextMenu->scroll, focusSlot);
    int busy = ScrollBar_Update(&contextMenu->scroll, input, input->cursor.pos, mainr);
    
    nvgScissor(vg, UnfoldRect(mainr));
    for (int i = 0; i < this->numEntry; i++) {
        Rect r = ScrollBar_GetRect(&contextMenu->scroll, i);
        ActorPropertyEntry* entry = &this->entryList[i];
        
        if (!IsBetween(r.y, mainr.y - r.h, mainr.y + mainr.h))
            continue;
        
        if (!busy && Rect_PointIntersect(&r, UnfoldVec2(input->cursor.pos))) {
            if (Input_GetCursor(input, CLICK_L)->dual && entry->index == this->current)
                contextMenu->state.setCondition = true;
            else if (Input_SelectClick(input, CLICK_L))
                this->current = this->change = entry->index;
        }
        
        Gfx_DrawRounderRect(vg, r, Theme_GetColor(THEME_ELEMENT_BASE, 80, 1.0f));
        if (entry->index == this->current)
            Gfx_DrawRounderRect(vg, r, Theme_GetColor(THEME_PRIM, 200, 1.0f));
        Gfx_Text(vg, r, NVG_ALIGN_LEFT, Theme_GetColor(THEME_TEXT, 255, 1.0f), entry->name);
    }
    nvgResetScissor(vg);
    
    ScrollBar_Draw(&contextMenu->scroll, vg);
    
    DummySplit_Pop(geo, &this->split);
    this->init = true;
}

////////////////////////////////////////////////////////////////////////////////

void ActorSearchContextMenu_New(Rect rect, u16 id) {
    Editor* editor = GetEditor();
    
    if (!gDatabaseNum) return;
    
    editor->searchMenu = new(ActorSearchContextMenu);
    editor->searchMenu->change = -1;
    editor->searchMenu->current = id;
    ContextMenu_Custom(&editor->geo, editor->searchMenu, NULL, ActorSearchContextMenu_Init, ActorSearchContextMenu_Draw, NULL, rect);
}

int ActorSearchContextMenu_State(int* ret) {
    Editor* editor = GetEditor();
    GeoGrid* geo = &editor->geo;
    ActorSearchContextMenu* this = editor->searchMenu;
    
    if (geo->dropMenu.udata != this)
        return -1;
    if (this->change > -1) {
        *ret = this->change;
        this->change = -1;
        
        return 1;
    }
    return 9;
}

void ActorSearchContextMenu_Free() {
    Editor* editor = GetEditor();
    ActorSearchContextMenu* this = editor->searchMenu;
    
    Element_ClearActiveTextbox(&this->geo);
    vfree(this->geo.elemState, this->entryList, editor->searchMenu);
}

////////////////////////////////////////////////////////////////////////////////

Arli* SceneDatabase_GetList(ContextDatabase type) {
    _assert(type < MENUDATA_MAX);
    return &sSceneEntry[type].arli;
}

const char* SceneDatabase_GetName(ContextDatabase type) {
    _assert(type < MENUDATA_MAX);
    return sSceneEntry[type].name;
}

////////////////////////////////////////////////////////////////////////////////

const char* ActorDatabase_Name(u16 index) {
    if (index < gDatabaseNum && gDatabaseActor[index])
        return gDatabaseActor[index]->name;
    return x_fmt("Unknown 0x%04X", index);
}

u16 ActorDatabase_ObjectID(u16 index) {
    if (index < gDatabaseNum && gDatabaseActor[index])
        return gDatabaseActor[index]->object;
    return 0xFFFF;
}

DbProperty* ActorDatabase_Properties(u16 index) {
    if (index < gDatabaseNum && gDatabaseActor[index])
        return gDatabaseActor[index]->property;
    return NULL;
}

int ActorDatabase_NumProperties(u16 index) {
    if (index < gDatabaseNum && gDatabaseActor[index])
        return gDatabaseActor[index]->numProperty;
    return 0;
}

DbVariable* ActorDatabase_Variables(u16 index) {
    if (index < gDatabaseNum && gDatabaseActor[index])
        return gDatabaseActor[index]->variable;
    return NULL;
}

int ActorDatabase_NumVariables(u16 index) {
    if (index < gDatabaseNum && gDatabaseActor[index])
        return gDatabaseActor[index]->numVariable;
    return 0;
}
