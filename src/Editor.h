#ifndef __Z64SCENE_H__
#define __Z64SCENE_H__
#include "Theme.h"
#include "GeoGrid.h"
#include "Cursor.h"

extern char* gBuild;
extern char* gHash;

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
	Room    room[32];
	MemFile zobj;
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
} SceneView;

#endif