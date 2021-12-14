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

void SplitTask_3DViewport_Update(void* passArg, void* instance, Split* split) {
	EditorContext* editorCtx = passArg;
	InputContext* inputCtx = &editorCtx->inputCtx;
	MouseInput* mouse = &inputCtx->mouse;
	Vec2s dim = {
		split->rect.w,
		split->rect.h
	};
	u32 extGrabDist = SPLIT_GRAB_DIST * 1.5;
	
	View_SetProjectionDimensions(&editorCtx->viewCtx, &dim);
	
	editorCtx->viewCtx.cameraControl = false;
	if (split->blockMouse == false) {
		if (split->mouseInSplit && !split->mouseInHeader) {
			editorCtx->viewCtx.cameraControl = true;
		}
	}
	
	// Cursor Wrapping
	if (editorCtx->viewCtx.setCamMove == true && (mouse->vel.x || mouse->vel.y)) {
		s16 rel;
		if (mouse->pos.x < split->edge[EDGE_L]->pos + extGrabDist) {
			rel = mouse->pos.x - split->edge[EDGE_L]->pos - extGrabDist;
			
			glfwSetCursorPos(editorCtx->appInfo.mainWindow, split->edge[EDGE_R]->pos - extGrabDist, editorCtx->inputCtx.mouse.pos.y);
			mouse->jumpVelComp.x =
			    (split->edge[EDGE_L]->pos + extGrabDist) -
			    (split->edge[EDGE_R]->pos - extGrabDist) + rel;
			OsPrintfEx("%d", rel);
		}
		
		if (mouse->pos.x > split->edge[EDGE_R]->pos - extGrabDist) {
			rel = mouse->pos.x - split->edge[EDGE_R]->pos + extGrabDist;
			
			glfwSetCursorPos(editorCtx->appInfo.mainWindow, split->edge[EDGE_L]->pos + extGrabDist, editorCtx->inputCtx.mouse.pos.y);
			mouse->jumpVelComp.x =
			    (split->edge[EDGE_R]->pos - extGrabDist) -
			    (split->edge[EDGE_L]->pos + extGrabDist) + rel;
			OsPrintfEx("%d", rel);
		}
		
		if (mouse->pos.y < split->edge[EDGE_T]->pos + extGrabDist) {
			rel = mouse->pos.y - split->edge[EDGE_T]->pos - extGrabDist;
			
			glfwSetCursorPos(editorCtx->appInfo.mainWindow, editorCtx->inputCtx.mouse.pos.x, split->edge[EDGE_B]->pos - extGrabDist - SPLIT_BAR_HEIGHT);
			mouse->jumpVelComp.y =
			    (split->edge[EDGE_T]->pos + extGrabDist) -
			    (split->edge[EDGE_B]->pos - extGrabDist - SPLIT_BAR_HEIGHT) + rel;
			OsPrintfEx("%d", rel);
		}
		
		if (mouse->pos.y > split->edge[EDGE_B]->pos - extGrabDist - SPLIT_BAR_HEIGHT) {
			rel = mouse->pos.y - split->edge[EDGE_B]->pos + extGrabDist + SPLIT_BAR_HEIGHT;
			
			glfwSetCursorPos(editorCtx->appInfo.mainWindow, editorCtx->inputCtx.mouse.pos.x, split->edge[EDGE_T]->pos + extGrabDist);
			mouse->jumpVelComp.y =
			    (split->edge[EDGE_B]->pos - extGrabDist - SPLIT_BAR_HEIGHT) -
			    (split->edge[EDGE_T]->pos + extGrabDist) + rel;
			OsPrintfEx("%d", rel);
		}
	}
}

void SplitTask_3DViewport_Draw(void* passArg, void* instance, Split* split) {
	EditorContext* editorCtx = passArg;
	
	z64_Draw_SetScene(&editorCtx->objCtx.scene);
	z64_Draw_Room(&editorCtx->objCtx.room[0]);
}

SplitTask sTaskList[] = {
	{ SplitTask_3DViewport_Update, SplitTask_3DViewport_Draw, NULL, NULL }
};

/* / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Editor_Draw(EditorContext* editorCtx) {
	if (glfwGetWindowAttrib(editorCtx->appInfo.mainWindow, GLFW_ICONIFIED))
		return;
	
	GeoGrid_Draw(&editorCtx->geoGridCtx);
}

void Editor_Update(EditorContext* editorCtx) {
	if (glfwGetWindowAttrib(editorCtx->appInfo.mainWindow, GLFW_ICONIFIED))
		return;
	
	GeoGrid_Update(&editorCtx->geoGridCtx);
	Cursor_Update(&editorCtx->cursorCtx);
}

void Editor_Init(EditorContext* editorCtx) {
	sTaskList[0].passArg = editorCtx;
	editorCtx->geoGridCtx.taskTable = sTaskList;
	
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
	
	editorCtx->viewCtx.cameraControl = false;
	Theme_Init(0);
	GeoGrid_Init(
		&editorCtx->geoGridCtx,
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