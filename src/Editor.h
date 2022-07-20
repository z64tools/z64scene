#include <ExtLib.h>
#include <ExtGui/Global.h>
#include "z_skelanime.h"
#include "z_light.h"

typedef struct {
	u16   index;
	u16   param;
	Vec3s pos;
	Vec3s rot;
} ActorEntry;

typedef struct {
	u16 index;
} ObjectEntry;

typedef struct {
	u16 actorNum;
	u16 objectNum;
	MemFile     mesh;
	ActorEntry  actor[255];
	ObjectEntry object[255];
} Room;

typedef struct {
	s32 curRoom;
	
	MemFile file;
	Room    room[64];
} Scene;

typedef struct EditorContext {
	AppInfo app;
	void*   vg;
	GeoGrid geoGrid;
	CursorContext cursor;
	InputContext  input;
	
	Scene scene;
} EditorContext;

ActorEntry* Actor_Add(Room* room, u16 id, u16 param, Vec3s pos, Vec3s rot);
void Actor_Delete(Room* room, s32 actorIndex);
ObjectEntry* Object_Add(Room* room, u16 id);
void Object_Delete(Room* room, s32 objIndex);

void Editor_DropCallback(GLFWwindow* window, s32 count, char* item[]);
void Editor_Update(EditorContext* editor);
void Editor_Draw(EditorContext* editor);

void EnSceneView_Init(void* passArg, void* instance, Split* split);
void EnSceneView_Destroy(void* passArg, void* instance, Split* split);
void EnSceneView_Update(void* passArg, void* instance, Split* split);
void EnSceneView_Draw(void* passArg, void* instance, Split* split);

typedef struct {
	ViewContext view;
	SkelAnime   skelAnime;
	Vec3s jointTbl[65];
	Vec3s morphTbl[65];
	s8    headerClick;
	
	MemFile* zobj;
	u32 zobjCount;
} EnSceneView;

void EnRoom_Init(void* passArg, void* instance, Split* split);
void EnRoom_Destroy(void* passArg, void* instance, Split* split);
void EnRoom_Update(void* passArg, void* instance, Split* split);
void EnRoom_Draw(void* passArg, void* instance, Split* split);

typedef struct EnRoom {
	ElSlider   slider;
	ElButton   leButton;
	ElButton   saveLayout;
	ElTextbox  sceneName;
	ElCheckbox checkBox;
} EnRoom;