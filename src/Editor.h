#ifndef __Z64SCENE_H__
#define __Z64SCENE_H__
#include "Theme.h"
#include "GUI.h"
#include "Cursor.h"

struct Area;
typedef void (* AreaFunc)(struct EditorContext*, struct Area*);

typedef struct {
	s32 notoSansID;
} FontContext;

typedef struct EditorContext {
	struct NVGcontext* vg;
	AppInfo appInfo;
	InputContext  inputCtx;
	ViewContext   viewCtx;
	LightContext  lightCtx;
	FontContext   fontCtx;
	ObjectContext objCtx;
	GuiContext    guiCtx;
	CursorContext cursorCtx;
} EditorContext;

void Editor_Draw(EditorContext* editorCtx);
void Editor_Update(EditorContext* editorCtx);
void Editor_Init(EditorContext* editorCtx);

#endif