#ifndef __Z64SCENE_H__
#define __Z64SCENE_H__
#include <z64.h>
#include <nanovg.h>
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

void Editor_Draw_2DElements(EditorContext* editorCtx);
void Editor_Draw_3DViewport(EditorContext* editorCtx);
void Editor_Update(EditorContext* editorCtx);
void Editor_Init(EditorContext* editorCtx);

#endif