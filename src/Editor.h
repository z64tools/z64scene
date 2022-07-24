#include <ExtLib.h>
#include <ExtGui/Interface.h>
#include "SkelAnime.h"
#include "Light.h"
#include "Scene.h"

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
	ViewContext view;
	s8 headerClick;
} EnSceneView;

typedef struct EnRoom {
	ElSlider envIdSlider;
	ElButton buttonDayLight;
	ElButton buttonFog;
	ElCombo  comboBox;
} EnRoom;

typedef struct EditorContext {
	AppInfo app;
	void*   vg;
	GeoGrid geo;
	Cursor  cursor;
	Input   input;
	
	EnRoom* tabRoom;
	EnSceneView* tabScene;
	
	Scene scene;
	
	struct {
		u32 drawBlock : 1;
	} state;
} EditorContext;

ActorEntry* Actor_Add(Room* room, u16 id, u16 param, Vec3s pos, Vec3s rot);
void Actor_Delete(Room* room, s32 actorIndex);
ObjectEntry* Object_Add(Room* room, u16 id);
void Object_Delete(Room* room, s32 objIndex);

void* NewMtx();

void Editor_DropCallback(GLFWwindow* window, s32 count, char* item[]);
void Editor_Update(EditorContext* editor);
void Editor_Draw(EditorContext* editor);

void EnSceneView_Init(void* passArg, void* instance, Split* split);
void EnSceneView_Destroy(void* passArg, void* instance, Split* split);
void EnSceneView_Update(void* passArg, void* instance, Split* split);
void EnSceneView_Draw(void* passArg, void* instance, Split* split);

void EnRoom_Init(void* passArg, void* instance, Split* split);
void EnRoom_Destroy(void* passArg, void* instance, Split* split);
void EnRoom_Update(void* passArg, void* instance, Split* split);
void EnRoom_Draw(void* passArg, void* instance, Split* split);

extern Gfx* gSetupDL;
#define gSetupDList(x) & gSetupDL[6 * x]