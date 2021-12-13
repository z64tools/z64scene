#include "Editor.h"
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

const char* sBuild = {
	"z64scene alpha commit[ "
	#ifndef __COMFLAG__
	GIT_COMMIT_MSG
	#endif
	" ]"
};

const char* sHash = {
	""
	#ifndef __COMFLAG__
	GIT_COMMIT_HASH
	#endif
};

/* / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Editor_Draw(EditorContext* editorCtx) {
	if (glfwGetWindowAttrib(editorCtx->appInfo.mainWindow, GLFW_ICONIFIED))
		return;
	
	GeoGui_Draw(editorCtx);
}

void Editor_Update(EditorContext* editorCtx) {
	if (glfwGetWindowAttrib(editorCtx->appInfo.mainWindow, GLFW_ICONIFIED))
		return;
	
	GeoGui_Update(editorCtx);
	Cursor_Update(&editorCtx->cursorCtx);
}

void Editor_Init(EditorContext* editorCtx) {
	GeoUIContext* regionCtx = &editorCtx->geoCtx;
	
	#if 0
	editorCtx->vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	#else
	editorCtx->vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
	#endif
	if (editorCtx->vg == NULL)
		printf_error("Could not init nanovg.");
	editorCtx->fontCtx.notoSansID = nvgCreateFont(editorCtx->vg, "sans", "Menlo-Regular.ttf");
	if (editorCtx->fontCtx.notoSansID < 0) {
		OsPrintfEx("Could not load Font");
	}
	
	editorCtx->viewCtx.cameraControl = false;
	GeoGui_Init(editorCtx);
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