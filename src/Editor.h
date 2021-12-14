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
	ViewContext    viewCtx;
	LightContext   lightCtx;
	FontContext    fontCtx;
	ObjectContext  objCtx;
	GeoGridContext geoGridCtx;
	CursorContext  cursorCtx;
	struct NVGcontext* vg;
} EditorContext;

void Editor_Draw(EditorContext* editorCtx);
void Editor_Update(EditorContext* editorCtx);
void Editor_Init(EditorContext* editorCtx);

#endif