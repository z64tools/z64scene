#include "Editor.h"

static EditorContext* editorCtx;

int main(void) {
	editorCtx = Lib_Calloc(0, sizeof(EditorContext));
	z64_Init(
		"z64scene",
		&editorCtx->appInfo,
		&editorCtx->inputCtx,
		&editorCtx->viewCtx,
		&editorCtx->objCtx,
		&editorCtx->lightCtx,
		editorCtx,
		(CallbackFunc)Editor_Update,
		(CallbackFunc)Editor_Draw
	);
	Editor_Init(editorCtx);
	
	MemFile_LoadFile(&editorCtx->objCtx.scene, "scene.zscene");
	MemFile_LoadFile(&editorCtx->objCtx.room[0], "room_0.zmap");
	
	z64_Update();
	glfwTerminate();
}
