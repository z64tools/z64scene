#ifndef DATABASE_H
#define DATABASE_H

#include <ext_type.h>

typedef struct {
    u16 variable;
    const char* text;
    
    u16 object[4];
    u8  numObject;
} DbVariable;

typedef struct {
    u16 val;
    const char* text;
} DbDictionary;

typedef struct {
    int source;
    
    const char* name;
    u16 mask;
    
    DbDictionary* dict;
    int numDict;
} DbProperty;

void Database_Init();
const char* Database_Name(u16 index);
u16 Database_ObjectIndex(u16 index);
DbProperty* Database_PropertyList(u16 index);
int Database_NumPropertyList(u16 index);
DbVariable* Database_VariableList(u16 index);
int Database_NumVariableList(u16 index);

#endif