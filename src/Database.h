#ifndef DATABASE_H
#define DATABASE_H

#include <nano_grid.h>

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

// WARNING

typedef enum {
	SCENE_FILE,
	SCENE_OPTIONS,
	SCENE_TITLE_ROOM_TIME,
	SCENE_TITLE_ROOM_ENV,
	SCENE_TITLE_LIGHTING,
	SCENE_TIME_GLOBAL,
	SCENE_TIME_SPEED,
	SCENE_ECHO,
	SCENE_ENABLE_SKYBOX,
	SCENE_ENABLE_SUNMOON,
	SCENE_BEHAVIOR_1,
	SCENE_BEHAVIOR_2,
	
	SCENE_DATA_MAX,
} ContextDatabase;

struct AppInfo;
struct Actor;
typedef struct DbActorSearchMenu
	DbActorSearchMenu;

const char* DatabaseArli_GetNameOfIndex(Arli* arli, size_t index);
int DatabaseArli_GetKeyOfIndex(Arli* arli, size_t index);
void DatabaseArli_SetKey(Arli* arli, size_t index);
int DatabaseArli_GetKey(Arli* arli);

Arli* DatabaseScene_GetList(ContextDatabase type);
const char* DatabaseScene_GetName(ContextDatabase type);
void DatabaseScene_SetKey(ContextDatabase type, int index);
int DatabaseScene_GetKey(ContextDatabase type);

void Database_Init();
void Database_Free();
void Database_Refresh();

const char* DbActor_Name(u16 index);
u16         DbActor_ObjectID(u16 index);
DbProperty* DbActor_Properties(u16 index);
int         DbActor_NumProperties(u16 index);
DbVariable* DbActor_Variables(u16 index);
int         DbActor_NumVariables(u16 index);

void ActorSearchContextMenu_New(Rect rect, u16 index);
int  ActorSearchContextMenu_State(int* ret);
void ActorSearchContextMenu_Free();

void LoadRomProject(const char* file);

typedef struct DbRender DbRender;

void DatabaseRender_Render(struct Actor*);

#endif
