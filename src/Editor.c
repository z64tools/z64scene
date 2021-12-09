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

#if 0
void Area_View(EditorContext* editorCtx, Area* panel) {
	MouseInput* mouse = &editorCtx->inputCtx.mouse;
	InputContext* inputCtx = &editorCtx->inputCtx;
	bool mouseMove = false;
	
	if (inputCtx->mouse.clickL.hold == false &&
	    inputCtx->mouse.clickR.hold == false &&
	    inputCtx->mouse.clickMid.hold == false &&
	    inputCtx->mouse.scrollY == 0) {
		editorCtx->regionCtx.actionFunc = NULL;
		
		return;
	}
	
	View_Camera_OrbitMode(&editorCtx->viewCtx, inputCtx);
	View_Camera_FlyMode(&editorCtx->viewCtx, inputCtx);
	
	if (mouse->pos.x < 0) {
		mouse->jumpVelComp.x = -panel->rect.w - 1;
		glfwSetCursorPos(editorCtx->appInfo.mainWindow, panel->rect.w, mouse->pos.y);
		mouse->pos.x = panel->rect.w;
	}
	
	if (mouse->pos.x > panel->rect.w) {
		mouse->jumpVelComp.x = panel->rect.w + 1;
		glfwSetCursorPos(editorCtx->appInfo.mainWindow, 0, mouse->pos.y);
		mouse->pos.x = 0;
	}
	
	if (mouse->pos.y < 0) {
		mouse->jumpVelComp.y = -panel->rect.h - 1;
		glfwSetCursorPos(editorCtx->appInfo.mainWindow, mouse->pos.x, panel->rect.h);
		mouse->pos.y = panel->rect.h;
	}
	
	if (mouse->pos.y > panel->rect.h) {
		mouse->jumpVelComp.y = panel->rect.h + 1;
		glfwSetCursorPos(editorCtx->appInfo.mainWindow, mouse->pos.x, 0);
		mouse->pos.y = 0;
	}
}

AreaFunc sRegionFuncs[] = {
	/* left  */ NULL,
	/* right */ NULL,
	/* top   */ NULL,
	/* bot   */ NULL,
	/* view  */ Area_View,
};

/* / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Editor_DrawArea_ViewPanel(EditorContext* editorCtx, Area* panel) {
	nvgBeginPath(editorCtx->vg);
	nvgRect(
		editorCtx->vg,
		0,
		0,
		editorCtx->appInfo.winDim.x,
		editorCtx->appInfo.winDim.y
	);
	nvgRoundedRect(
		editorCtx->vg,
		panel->rect.x + 3,
		panel->rect.y + 3,
		panel->rect.w - 6,
		panel->rect.h - 6,
		3.0f
	);
	nvgPathWinding(editorCtx->vg, NVG_HOLE);
	nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_SPLITTER));
	nvgFill(editorCtx->vg);
}

void Editor_DrawArea_LeftPanel(EditorContext* editorCtx, Area* panel) {
	Vec2s basePos = {
		panel->rect.x + 10,
		panel->rect.y + 10
	};
	Vec2s textPos = basePos;
	
	// BG
	nvgBeginPath(editorCtx->vg);
	nvgRect(
		editorCtx->vg,
		panel->rect.x,
		panel->rect.y,
		panel->rect.w,
		panel->rect.h
	);
	nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_SPLITTER));
	nvgFill(editorCtx->vg);
	
	nvgBeginPath(editorCtx->vg);
	nvgRoundedRect(
		editorCtx->vg,
		panel->rect.x + 3,
		panel->rect.y + 3,
		panel->rect.w - 6,
		panel->rect.h - 6,
		3.0f
	);
	nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_BASE_DARK));
	nvgFill(editorCtx->vg);
	
	// Text
	// nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_BASE_WHITE));
	// nvgFontSize(editorCtx->vg, 24.0f);
	// nvgFontFace(editorCtx->vg, "sans");
	// nvgTextAlign(editorCtx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	// nvgTextBox(editorCtx->vg, textPos.x, textPos.y, 150, "Z64SCENE", NULL);
	// textPos.y += 24;
	// nvgFontSize(editorCtx->vg, 16.0f);
	// nvgTextBox(editorCtx->vg, textPos.x, textPos.y, 150, "Modular and skawoUHHUH", NULL);
}

void Editor_DrawArea_BotPanel(EditorContext* editorCtx, Area* panel) {
	nvgBeginPath(editorCtx->vg);
	nvgRect(
		editorCtx->vg,
		panel->rect.x,
		panel->rect.y,
		panel->rect.w,
		panel->rect.h
	);
	nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_SPLITTER_DARKER));
	nvgFill(editorCtx->vg);
	
	nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_BASE_WHITE));
	nvgFontSize(editorCtx->vg, panel->rect.h - 16);
	nvgFontFace(editorCtx->vg, "sans");
	nvgTextAlign(editorCtx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	nvgText(editorCtx->vg, panel->rect.x + 5, panel->rect.y + 8, sBuild, NULL);
	
	nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_BASE_WHITE));
	nvgFontSize(editorCtx->vg, panel->rect.h - 16);
	nvgFontFace(editorCtx->vg, "sans");
	nvgTextAlign(editorCtx->vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
	nvgText(editorCtx->vg, panel->rect.w - 5, panel->rect.y + 8, sHash, NULL);
}
#endif
/* / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / */
/* / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Editor_Draw(EditorContext* editorCtx) {
	static s32 o;
	
	Gui_Draw(editorCtx);
}

void Editor_Update(EditorContext* editorCtx) {
	Gui_Update(editorCtx);
}

void Editor_Init(EditorContext* editorCtx) {
	GuiContext* regionCtx = &editorCtx->guiCtx;
	
	#ifndef NDEBUG
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
	Gui_Init(editorCtx);
	
	glfwSetWindowSizeLimits(
		editorCtx->appInfo.mainWindow,
		400,
		200,
		GLFW_DONT_CARE,
		GLFW_DONT_CARE
	);
}