#ifndef __Z64SCENE_H__
#define __Z64SCENE_H__
#include <zroom.h>
#include <n64.h>
#include <bigendian.h>
#include <assert.h>

#include <HermosauhuLib.h>
#include <Input.h>
#include <Matrix.h>
#include <View.h>
#include <Vector.h>
#include <Light.h>
#include <Object.h>
#include "GuiElement.h"

typedef struct {
	s32 notoSansID;
} FontContext;

typedef struct EditorContext {
	struct NVGcontext* vg;
	AppInfo appInfo;
	InputContext   inputCtx;
	ViewContext    viewCtx;
	LightContext   lightCtx;
	FontContext    fontCtx;
	ObjectContext  objCtx;
	ElementContext elemCtx;
} EditorContext;

void Editor_Init(EditorContext* editorCtx);
void Editor_Draw_2DElements(EditorContext* editorCtx);
void Editor_Draw_3DViewport(EditorContext* editorCtx);
void Editor_Draw(EditorContext* editorCtx);

#endif