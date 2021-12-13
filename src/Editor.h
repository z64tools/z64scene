#ifndef __Z64SCENE_H__
#define __Z64SCENE_H__
#include "Theme.h"
#include "FlexUI.h"
#include "FlexUI_Elements.h"
#include "Cursor.h"

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
	FlexUIContext flexCtx;
	CursorContext  cursorCtx;
} EditorContext;

void Editor_Draw(EditorContext* editorCtx);
void Editor_Update(EditorContext* editorCtx);
void Editor_Init(EditorContext* editorCtx);

#endif