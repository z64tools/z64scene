#include "Editor.h"

static EditorContext* editorCtx;

int main(void) {
	Vec2s res = { 1400, 700 };
	FILE* file = fopen("Settings.json", "rb");
	
	#ifndef NDEBUG
	printf_SetPrefix("");
	printf_SetSuppressLevel(PSL_DEBUG);
	#endif
	
	editorCtx = Lib_Calloc(0, sizeof(EditorContext));
	
	if (file != NULL) {
		fclose(file);
		res = GeoGrid_Layout_LoadJson(&editorCtx->geoCtx, &editorCtx->appInfo.winDim);
	}
	
	z64_Init(
		"z64scene",
		&editorCtx->appInfo,
		&editorCtx->inputCtx,
		&editorCtx->objCtx,
		&editorCtx->lightCtx,
		editorCtx,
		(CallbackFunc)Editor_Update,
		(CallbackFunc)Editor_Draw,
		res.x,
		res.y
	);
	Editor_Init(editorCtx);
	
	MemFile_LoadFile(&editorCtx->objCtx.scene, "scene.zscene");
	MemFile_LoadFile(&editorCtx->objCtx.room[0], "room_0.zmap");
	
	z64_Update();
	glfwTerminate();
}
