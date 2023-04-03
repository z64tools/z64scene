#include "Database.h"
#include <ext_lib.h>

typedef enum {
    PROP_SOURCE_VARIABLE,
    PROP_SOURCE_ROT_X,
    PROP_SOURCE_ROT_Y,
    PROP_SOURCE_ROT_Z,
    PROP_SOURCE_POS_X,
    PROP_SOURCE_POS_Y,
    PROP_SOURCE_POS_Z,
} Source;

typedef struct {
    u16 variable;
    const char* text;
    
    u16 object[4];
    u8  numObject;
} Variable;

typedef struct {
    u16 val;
    const char* text;
} DropEntry;

typedef struct {
    Source      source;
    const char* name;
    u16 mask;
    
    DropEntry* drop;
    int numDrop;
} Property;

typedef struct {
    u16 index;
    u16 object;
    const char* name;
    Variable*   variable;
    int numVariable;
} Entry;

Entry** gDatabaseActor;

void Database_Init() {
    Toml toml = Toml_New();
    
    Toml_Load(&toml, "database/actor_names.toml");
    
    int num = Toml_ArrCount(&toml, "actor[]");
    int maxIndex = 0;
    
    for (int i = 0; i < num; i++) {
        int index = Toml_GetInt(&toml, "actor[%d].index", i);
        
        maxIndex = Max(maxIndex, index);
    }
    
    gDatabaseActor = new(Entry[maxIndex]);
    
    for (int i = 0; i < num; i++) {
        Entry* entry = new(Entry);
        
        entry->index = Toml_GetInt(&toml, "actor[%d].index", i);
        entry->name = Toml_GetStr(&toml, "actor[%d].name", i);
        entry->object = 0xFFFF;
        
        if (Toml_Var(&toml, "actor[%d].object", i))
            entry->object = Toml_GetInt(&toml, "actor[%d].object", i);
        
        entry->numVariable = Toml_ArrCount(&toml, "actor[%d].var", i);
        if (entry->numVariable) {
            Variable* v = entry->variable = new(Variable[entry->numVariable]);
            
            for (int k = 0; k < entry->numVariable; k++, v++) {
                v->variable = Toml_GetInt(&toml, "actor[%d].var[%d][0]", i, k);
                v->text = Toml_GetStr(&toml, "actor[%d].var[%d][1]", i, k);
            }
        }
        
        gDatabaseActor[entry->index] = entry;
    }
    
    Toml_Free(&toml);
}

const char* Database_Name(u16 index) {
    if (gDatabaseActor[index])
        return gDatabaseActor[index]->name;
    return x_fmt("Unknown 0x%04X", index);
}

u16 Database_ObjectIndex(u16 index) {
    if (gDatabaseActor[index])
        return gDatabaseActor[index]->object;
    return 0xFFFF;
}
