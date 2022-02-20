#include "Editor.h"

static EditorContext* editCtx;

void Editor_DropCallback(GLFWwindow* window, s32 count, char* file[]);

int main(void) {
	Vec2s res = { 820, 560 };
	
	#ifndef NDEBUG
		printf_SetPrefix("");
		printf_SetSuppressLevel(PSL_DEBUG);
		printf_WinFix();
	#endif
	
	editCtx = Calloc(0, sizeof(EditorContext));
	
	if (Stat("z64scene.json")) {
		res = GeoGrid_Layout_LoadJson(&editCtx->geoCtx, &editCtx->appInfo.winDim);
	}
	
	Zelda64_Init(
		"z64scene",
		&editCtx->appInfo,
		&editCtx->inputCtx,
		editCtx,
		(CallbackFunc)Editor_Update,
		(CallbackFunc)Editor_Draw,
		Editor_DropCallback,
		res.x,
		res.y,
		2
	);
	Editor_Init(editCtx);
	
	Zelda64_Update();
	glfwTerminate();
}
