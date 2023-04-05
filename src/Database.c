#include "Database.h"
#include <ext_lib.h>

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

////////////////////////////////////////////////////////////////////////////////

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
