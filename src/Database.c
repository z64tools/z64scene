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
} Entry;

Entry** gDatabaseActor;
int gDatabaseNum;

static void ParseVariable(Toml* toml, Entry* entry, int i) {
    entry->numVariable = Toml_ArrCount(toml, "actor[%d].variable", i);
    if (!entry->numVariable) return;
    
    DbVariable* v = entry->variable = new(DbVariable[entry->numVariable]);
    
    for (int k = 0; k < entry->numVariable; k++, v++) {
        v->variable = Toml_GetInt(toml, "actor[%d].variable[%d][0]", i, k);
        v->text = Toml_GetStr(toml, "actor[%d].variable[%d][1]", i, k);
    }
}

static void ParsePropDict(Toml* toml, DbProperty* p, int i, int k) {
    p->numDict = Toml_ArrCount(toml, "actor[%d].property[%d].dict", i, k);
    if (!p->numDict) return;
    DbDictionary* de = p->dict = new(DbDictionary[p->numDict]);
    
    for (int j = 0; j < p->numDict; j++, de++) {
        de->val = Toml_GetInt(toml, "actor[%d].property[%d].dict[%d][0]", i, k, j);
        de->text = Toml_GetStr(toml, "actor[%d].property[%d].dict[%d][1]", i, k, j);
        info("%04X, %s", de->val, de->text);
    }
}

static void ParseProperty(Toml* toml, Entry* entry, int i) {
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
        
        ParsePropDict(toml, p, i, k);
    }
}

////////////////////////////////////////////////////////////////////////////////

void Database_Init() {
    Toml toml = Toml_New();
    
    if (!sys_stat("resources/actor_property.toml"))
        return;
    
    Toml_Load(&toml, "resources/actor_property.toml");
    
    int num = Toml_ArrCount(&toml, "actor[]");
    
    for (int i = 0; i < num; i++) {
        int index = Toml_GetInt(&toml, "actor[%d].index", i);
        
        gDatabaseNum = Max(gDatabaseNum, index);
    }
    
    gDatabaseActor = new(Entry[gDatabaseNum]);
    
    for (int i = 0; i < num; i++) {
        Entry* entry = new(Entry);
        
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
        ParseVariable(&toml, entry, i);
        _log("actor[%d].property", i);
        ParseProperty(&toml, entry, i);
        
        gDatabaseActor[entry->index] = entry;
    }
    
    Toml_Free(&toml);
}

void Database_Free() {
    _log("Free");
    
    for (int i = 0; i < gDatabaseNum; i++) {
        Entry* e = gDatabaseActor[i];
        
        if (!e) continue;
        
        DbVariable* vari = e->variable;
        
        for (int j = 0; j < e->numVariable; j++, vari++)
            vfree(vari->text);
        
        DbProperty* prop = e->property;
        
        for (int j = 0; j < e->numProperty; j++, prop++) {
            DbDictionary* dict = prop->dict;
            
            for (int k = 0; k < prop->numDict; k++, dict++)
                vfree(dict->text);
            
            vfree(prop->name, prop->dict);
        }
        
        vfree(e->name, e->variable, e->property, e);
    }
    
    vfree(gDatabaseActor);
    gDatabaseNum = 0;
}

void Database_Refresh() {
    Database_Free();
    Database_Init();
}

////////////////////////////////////////////////////////////////////////////////

typedef struct DatabaseSearch {
    GeoGrid   geo;
    Split     split;
    ElTextbox textboxSearch;
    
    Entry* entryList;
    int    numEntry;
    int    initList;
    int    change;
    int    current;
    int    init;
} DatabaseSearch;

static void DatabaseSearch_Init(GeoGrid* __no_no, ContextMenu* contextMenu) {
    DatabaseSearch* this = contextMenu->prop;
    
    GeoGrid_Init(&this->geo, &GetEditor()->app, NULL);
    
    this->textboxSearch.clearIcon = true;
    this->textboxSearch.align = NVG_ALIGN_LEFT;
    this->entryList = new(Entry[gDatabaseNum]);
    
    contextMenu->rect.w = 128 + 64;
    contextMenu->rect.h = SPLIT_ELEM_X_PADDING * 3 + SPLIT_TEXT_H * 17;
}

static void DatabaseSearch_Draw(GeoGrid* __no_no, ContextMenu* contextMenu) {
    DatabaseSearch* this = contextMenu->prop;
    GeoGrid* geo = &this->geo;
    void* vg = geo->vg;
    Input* input = geo->input;
    
    DummySplit_Push(geo, &this->split, contextMenu->rect);
    
    this->textboxSearch.element.rect = contextMenu->rect;
    this->textboxSearch.element.rect.h = SPLIT_TEXT_H;
    this->textboxSearch.element.rect.x += SPLIT_ELEM_X_PADDING;
    this->textboxSearch.element.rect.y += SPLIT_ELEM_X_PADDING;
    this->textboxSearch.element.rect.w -= SPLIT_ELEM_X_PADDING * 2;
    
    Element_Textbox(&this->textboxSearch);
    
    if (!this->init) {
        Element_SetActiveTextbox(&this->geo, &this->split, &this->textboxSearch);
        this->init = true;
    }
    
    if (this->textboxSearch.modified || !this->initList) {
        bool textMatch = strlen(this->textboxSearch.txt) ? true : false;
        
        this->initList = true;
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
    
    ScrollBar_Update(&contextMenu->scroll, input, input->cursor.pos, mainr);
    
    nvgScissor(vg, UnfoldRect(mainr));
    for (int i = 0; i < this->numEntry; i++) {
        Rect r = ScrollBar_GetRect(&contextMenu->scroll, i);
        Entry* entry = &this->entryList[i];
        
        if (!IsBetween(r.y, mainr.y - r.h, mainr.y + mainr.h))
            continue;
        
        if (Rect_PointIntersect(&r, UnfoldVec2(input->cursor.pos))) {
            if (Input_GetCursor(input, CLICK_L)->dual && entry->index == this->current)
                contextMenu->state.setCondition = true;
            else if (Input_SelectClick(input, CLICK_L))
                this->current = this->change = entry->index;
        }
        
        Gfx_DrawRounderRect(vg, r, Theme_GetColor(THEME_ELEMENT_BASE, 80, 1.0f));
        if (entry->index == this->current)
            Gfx_DrawRounderRect(vg, r, Theme_GetColor(THEME_PRIM, 200, 1.0f));
        Gfx_Text(vg, r, NVG_ALIGN_LEFT, Theme_GetColor(THEME_TEXT, 255, 1.0f), x_fmt("%04X: %s", entry->index, entry->name));
    }
    nvgResetScissor(vg);
    
    ScrollBar_Draw(&contextMenu->scroll, vg);
    
    DummySplit_Pop(geo, &this->split);
}

////////////////////////////////////////////////////////////////////////////////

void DatabaseSearch_New(Rect rect, u16 id) {
    Editor* editor = GetEditor();
    
    editor->dataContextMenu = new(DatabaseSearch);
    editor->dataContextMenu->change = -1;
    editor->dataContextMenu->current = id;
    ContextMenu_Custom(&editor->geo, editor->dataContextMenu, NULL, DatabaseSearch_Init, DatabaseSearch_Draw, NULL, rect);
}

int DatabaseSearch_State(int* ret) {
    Editor* editor = GetEditor();
    GeoGrid* geo = &editor->geo;
    DatabaseSearch* this = editor->dataContextMenu;
    
    if (geo->dropMenu.prop != this)
        return -1;
    if (this->change > -1) {
        *ret = this->change;
        this->change = -1;
        
        return 1;
    }
    return 9;
}

void DatabaseSearch_Free() {
    Editor* editor = GetEditor();
    DatabaseSearch* this = editor->dataContextMenu;
    
    info("clear");
    Element_ClearActiveTextbox(&this->geo);
    vfree(this->geo.elemState, this->entryList, editor->dataContextMenu);
    editor->dataContextMenu = NULL;
}

////////////////////////////////////////////////////////////////////////////////

const char* Database_Name(u16 index) {
    if (index < gDatabaseNum && gDatabaseActor[index])
        return gDatabaseActor[index]->name;
    return x_fmt("Unknown 0x%04X", index);
}

u16 Database_ObjectIndex(u16 index) {
    if (index < gDatabaseNum && gDatabaseActor[index])
        return gDatabaseActor[index]->object;
    return 0xFFFF;
}

DbProperty* Database_PropertyList(u16 index) {
    if (index < gDatabaseNum && gDatabaseActor[index])
        return gDatabaseActor[index]->property;
    return NULL;
}

int Database_NumPropertyList(u16 index) {
    if (index < gDatabaseNum && gDatabaseActor[index])
        return gDatabaseActor[index]->numProperty;
    return 0;
}

DbVariable* Database_VariableList(u16 index) {
    if (index < gDatabaseNum && gDatabaseActor[index])
        return gDatabaseActor[index]->variable;
    return NULL;
}

int Database_NumVariableList(u16 index) {
    if (index < gDatabaseNum && gDatabaseActor[index])
        return gDatabaseActor[index]->numVariable;
    return 0;
}
