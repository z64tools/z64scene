#include "Editor.h"

static EditorContext* editCtx;

int main(void) {
	Vec2s res = { 1400, 700 };
	FILE* file = fopen("Settings.json", "rb");
	
	#ifndef NDEBUG
	printf_SetPrefix("");
	printf_SetSuppressLevel(PSL_DEBUG);
	#endif
	
	editCtx = Lib_Calloc(0, sizeof(EditorContext));
	
	if (file != NULL) {
		fclose(file);
		res = GeoGrid_Layout_LoadJson(&editCtx->geoCtx, &editCtx->appInfo.winDim);
	}
	
	z64_Init(
		"z64scene",
		&editCtx->appInfo,
		&editCtx->inputCtx,
		&editCtx->objCtx,
		&editCtx->lightCtx,
		editCtx,
		(CallbackFunc)Editor_Update,
		(CallbackFunc)Editor_Draw,
		res.x,
		res.y
	);
	Editor_Init(editCtx);
	
	MemFile_LoadFile(&editCtx->objCtx.scene, "scene.zscene");
	MemFile_LoadFile(&editCtx->objCtx.room[0], "room_0.zmap");
	
	z64_Update();
	glfwTerminate();
}
