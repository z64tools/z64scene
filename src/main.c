#include "Editor.h"

static EditorContext* editCtx;

void Editor_DropCallback(GLFWwindow* window, s32 count, char* file[]);

int main(void) {
	Vec2s res = { 1400, 700 };
	FILE* file = fopen("Settings.json", "rb");
	
	#ifndef NDEBUG
		printf_SetPrefix("");
		printf_SetSuppressLevel(PSL_DEBUG);
		printf_WinFix();
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
		editCtx,
		(CallbackFunc)Editor_Update,
		(CallbackFunc)Editor_Draw,
		Editor_DropCallback,
		res.x,
		res.y,
		2
	);
	Editor_Init(editCtx);
	
	z64_Update();
	glfwTerminate();
}
