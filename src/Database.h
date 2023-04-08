#ifndef DATABASE_H
#define DATABASE_H

#include <ext_geogrid.h>

typedef enum {
    DB_VAR,
    DB_POSX,
    DB_POSY,
    DB_POSZ,
    DB_ROTX,
    DB_ROTY,
    DB_ROTZ,
} DbSource;

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

typedef enum {
    MENUDATA_FILE,
    MENUDATA_OPTIONS,
    MENUDATA_BEHAVIOR_1,
    MENUDATA_BEHAVIOR_2,
    MENUDATA_MAX,
} ContextDatabase;

struct AppInfo;
typedef struct ActorSearchContextMenu
    ActorSearchContextMenu;

const char* DbDictionary_GetArliIndex(Arli* arli, size_t index);
void DbDictionary_SetArliIndex(Arli* arli, size_t index);

void Database_Init();
void Database_Free();
void Database_Refresh();

Arli* SceneDatabase_GetList(ContextDatabase type);
const char* SceneDatabase_GetName(ContextDatabase type);

const char* ActorDatabase_Name(u16 index);
u16         ActorDatabase_ObjectID(u16 index);
DbProperty* ActorDatabase_Properties(u16 index);
int         ActorDatabase_NumProperties(u16 index);
DbVariable* ActorDatabase_Variables(u16 index);
int         ActorDatabase_NumVariables(u16 index);

void ActorSearchContextMenu_New(Rect rect, u16 index);
int  ActorSearchContextMenu_State(int* ret);
void ActorSearchContextMenu_Free();

#endif