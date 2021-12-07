#ifndef __Z64SCENE_H__
#define __Z64SCENE_H__
#include <z64.h>
#include <nanovg.h>
#include "Theme.h"

struct Region;
typedef void (* RegionFunc)(struct EditorContext*, struct Region*);

typedef struct {
	s32 notoSansID;
} FontContext;

typedef struct {
	u8 cursorInRange : 1;
} RegionState;

typedef struct Region {
	RegionState state;
	Vec2i pos;
	Vec2i dim;
} Region;

typedef struct {
	Region     side;
	Region     bot;
	Region     view;
	RegionFunc actionFunc;
	Region*    actionRegion;
} RegionContext;

typedef struct EditorContext {
	struct NVGcontext* vg;
	AppInfo appInfo;
	InputContext  inputCtx;
	ViewContext   viewCtx;
	LightContext  lightCtx;
	FontContext   fontCtx;
	ObjectContext objCtx;
	RegionContext regionCtx;
} EditorContext;

void Editor_Draw(EditorContext* editorCtx);
void Editor_Update(EditorContext* editorCtx);
void Editor_Init(EditorContext* editorCtx);

#endif