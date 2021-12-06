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
#include "Gui.h"

typedef struct {
	s32 notoSansID;
} FontContext;

typedef struct Z64SceneContext {
	struct NVGcontext* vg;
	AppInfo appInfo;
	InputContext  inputCtx;
	ViewContext   viewCtx;
	LightContext  lightCtx;
	FontContext   fontCtx;
	ObjectContext objCtx;
	GuiContext    guiCtx;
} Z64SceneContext;

void z64scene_Init(Z64SceneContext* z64Ctx);
void z64scene_DrawGUI(Z64SceneContext* z64Ctx);
void z64scene_Draw3DViewport(Z64SceneContext* z64Ctx);

#endif