#ifndef __Z64SCENE_H__
#define __Z64SCENE_H__
#include "Theme.h"
#include "GeoGrid.h"
#include "Cursor.h"
#include "Actor.h"

#define ROOM_MAX 128

extern char* gBuild;
extern char* gHash;
extern s32 gSceneConfIndex;

typedef struct {
	s32 notoSansID;
} FontContext;

typedef struct EditorContext {
	AppInfo appInfo;
	InputContext   inputCtx;
	FontContext    fontCtx;
	GeoGridContext geoCtx;
	CursorContext  cursorCtx;
	struct NVGcontext* vg;
	Scene   scene;
	Room    room[ROOM_MAX];
	MemFile zobj;
	void*   gizmo;
	struct {
		char sceneName[512];
	} project;
} EditorContext;

extern EditorContext* gEditCtx;

void Editor_Draw(EditorContext* editCtx);
void Editor_Update(EditorContext* editCtx);
void Editor_Init(EditorContext* editCtx);

// EditorTask

void EnSceneView_Init(void* passArg, void* instance, Split* split);
void EnSceneView_Destroy(void* passArg, void* instance, Split* split);
void EnSceneView_Update(void* passArg, void* instance, Split* split);
void EnSceneView_Draw(void* passArg, void* instance, Split* split);

typedef struct {
	ViewContext viewCtx;
	Vec3s jointTable[256];
	Vec3s morphTable[256];
	SkelAnime skelAnime;
	bool  headerClick;
} EnSceneView;

void EnRoom_Init(void* passArg, void* instance, Split* split);
void EnRoom_Destroy(void* passArg, void* instance, Split* split);
void EnRoom_Update(void* passArg, void* instance, Split* split);
void EnRoom_Draw(void* passArg, void* instance, Split* split);

typedef struct {
	ElTextbox  sceneName;
	ElButton   leButton;
	ElCheckbox checkBox;
	
	ElButton   saveLayout;
	ElSlider   slider;
} EnRoom;

#define DefineTask(x) x ## _Init, \
	x ## _Destroy, \
	x ## _Update, \
	x ## _Draw, \
	sizeof(x)

#endif