#include "Editor.h"
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

char* gBuild = {
	"z64scene alpha commit[ "
	#ifndef __COMFLAG__
	GIT_COMMIT_MSG
	#endif
	" ]"
};

char* gHash = {
	""
	#ifndef __COMFLAG__
	GIT_COMMIT_HASH
	#endif
};

SplitTask sTaskTable[] = {
	{
		"None"
	},
	{
		"3DViewport",
		EnSceneView_Init, EnSceneView_Destroy, EnSceneView_Update, EnSceneView_Draw,
		sizeof(SceneView)
	}
};

/* / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Editor_Draw(EditorContext* editorCtx) {
	if (glfwGetWindowAttrib(editorCtx->appInfo.mainWindow, GLFW_ICONIFIED))
		return;
	
	GeoGrid_Draw(&editorCtx->geoCtx);
}

void Editor_Update(EditorContext* editorCtx) {
	if (glfwGetWindowAttrib(editorCtx->appInfo.mainWindow, GLFW_ICONIFIED))
		return;
	
	GeoGrid_Update(&editorCtx->geoCtx);
	Cursor_Update(&editorCtx->cursorCtx);
}

void Editor_Init(EditorContext* editorCtx) {
	editorCtx->geoCtx.passArg = editorCtx;
	editorCtx->geoCtx.taskTable = sTaskTable;
	editorCtx->geoCtx.taskTableNum = ArrayCount(sTaskTable);
	
	#if 0
	editorCtx->vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	#else
	editorCtx->vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
	#endif
	if (editorCtx->vg == NULL)
		printf_error("Could not init nanovg.");
	editorCtx->fontCtx.notoSansID = nvgCreateFont(editorCtx->vg, "sans", "NotoSans-Regular.ttf");
	if (editorCtx->fontCtx.notoSansID < 0) {
		OsPrintfEx("Could not load Font");
	}
	
	Theme_Init(0);
	GeoGrid_Init(
		&editorCtx->geoCtx,
		&editorCtx->appInfo.winDim,
		&editorCtx->inputCtx.mouse,
		editorCtx->vg
	);
	Cursor_Init(&editorCtx->cursorCtx);
	
	for (s32 i = -4; i < 22; i++) {
		s32 o = Lib_Wrap(i, -8, 11);
		OsPrintf("%d - %d", i, o);
	}
	
	glfwSetWindowSizeLimits(
		editorCtx->appInfo.mainWindow,
		400,
		200,
		GLFW_DONT_CARE,
		GLFW_DONT_CARE
	);
}