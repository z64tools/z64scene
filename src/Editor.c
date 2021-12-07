#include "Editor.h"
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

/* / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Region_View(EditorContext* editorCtx, Region* panel) {
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

RegionFunc sRegionFuncs[] = {
	NULL,
	NULL,
	Region_View
};

/* / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Editor_DrawRegion_ViewPanel(EditorContext* editorCtx, Region* panel) {
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

void Editor_DrawRegion_SidePanel(EditorContext* editorCtx, Region* panel) {
	Vec2i basePos = {
		panel->rect.x + 10,
		panel->rect.y + 10
	};
	Vec2i textPos = basePos;
	
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

const char* sBuild = {
	"z64scene alpha commit[ " GIT_COMMIT_MSG " ]"
};

const char* sHash = {
	GIT_COMMIT_HASH
};

void Editor_DrawRegion_BotPanel(EditorContext* editorCtx, Region* panel) {
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

/* / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Editor_Region_UpdateSplitter(AppInfo* appInfo, RegionContext* regionCtx) {
	Region* botRegion = &regionCtx->bot;
	Region* sideRegion = &regionCtx->side;
	Region* viewRegion = &regionCtx->view;
	
	botRegion->rect.x = 0;
	botRegion->rect.y = appInfo->winDim.y - 32;
	botRegion->rect.w = appInfo->winDim.x;
	botRegion->rect.h = 32;
	
	sideRegion->rect.h = appInfo->winDim.y - botRegion->rect.h;
	sideRegion->rect.x = appInfo->winDim.x - sideRegion->rect.w;
	sideRegion->rect.y = 0;
	
	viewRegion->rect.x = 0;
	viewRegion->rect.y = 0;
	viewRegion->rect.w = appInfo->winDim.x - sideRegion->rect.w;
	viewRegion->rect.h = appInfo->winDim.y - botRegion->rect.h;
}

void Editor_Region_SetSplitter(AppInfo* appInfo, RegionContext* regionCtx, f32 x, f32 y) {
	Region* sideRegion = &regionCtx->side;
	Region* viewRegion = &regionCtx->view;
	
	sideRegion->rect.w = CLAMP(
		x,
		200,
		floorf(appInfo->winDim.x * 0.5)
	);
	Editor_Region_UpdateSplitter(appInfo, regionCtx);
}

void Editor_Region_SetCursorState(InputContext* inputCtx, RegionContext* regionCtx, Region* panel) {
	MouseInput* mouse = &inputCtx->mouse;
	
	if (regionCtx->actionFunc != NULL) {
		return;
	}
	
	if (mouse->pos.x >= panel->rect.x && mouse->pos.x < (panel->rect.x + panel->rect.w)) {
		if (mouse->pos.y >= panel->rect.y && mouse->pos.y < (panel->rect.y + panel->rect.h)) {
			if (inputCtx->mouse.clickL.press ||
			    inputCtx->mouse.clickMid.press ||
			    inputCtx->mouse.clickR.press ||
			    inputCtx->mouse.scrollY) {
				regionCtx->actionRegion = panel;
				regionCtx->actionFunc = sRegionFuncs[panel->id];
			}
			
			return;
		}
	}
}

void Editor_Region_Update(EditorContext* editorCtx) {
	MouseInput* mouse = &editorCtx->inputCtx.mouse;
	RegionContext* regionCtx = &editorCtx->regionCtx;
	
	if (editorCtx->appInfo.isResizeCallback == true) {
		Editor_Region_SetSplitter(
			&editorCtx->appInfo,
			&editorCtx->regionCtx,
			editorCtx->regionCtx.side.rect.w,
			0
		);
		
		return;
	}
	
	if (regionCtx->actionFunc) {
		regionCtx->actionFunc(editorCtx, regionCtx->actionRegion);
	}
	
	if (!regionCtx->actionFunc) {
		Editor_Region_SetCursorState(&editorCtx->inputCtx, regionCtx, &regionCtx->side);
		Editor_Region_SetCursorState(&editorCtx->inputCtx, regionCtx, &regionCtx->bot);
		Editor_Region_SetCursorState(&editorCtx->inputCtx, regionCtx, &regionCtx->view);
	}
	
	Editor_Region_UpdateSplitter(&editorCtx->appInfo, &editorCtx->regionCtx);
}

/* / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Editor_Draw_2DElements(EditorContext* editorCtx) {
	nvgBeginFrame(editorCtx->vg, editorCtx->appInfo.winDim.x, editorCtx->appInfo.winDim.y, 1.0f);
	
	Editor_DrawRegion_ViewPanel(editorCtx, &editorCtx->regionCtx.view);
	Editor_DrawRegion_SidePanel(editorCtx, &editorCtx->regionCtx.side);
	Editor_DrawRegion_BotPanel(editorCtx, &editorCtx->regionCtx.bot);
	
	nvgEndFrame(editorCtx->vg);
}

void Editor_Draw_3DViewport(EditorContext* editorCtx) {
	z64_Draw_SetScene(&editorCtx->objCtx.scene);
	z64_Draw_Room(&editorCtx->objCtx.room[0]);
}

void Editor_Draw(EditorContext* editorCtx) {
	glViewport(
		editorCtx->regionCtx.view.rect.x,
		-editorCtx->regionCtx.view.rect.y + editorCtx->appInfo.winDim.y - editorCtx->regionCtx.view.rect.h,
		editorCtx->regionCtx.view.rect.w,
		editorCtx->regionCtx.view.rect.h
	);
	Editor_Draw_3DViewport(editorCtx);
	glViewport(
		0,
		0,
		editorCtx->appInfo.winDim.x,
		editorCtx->appInfo.winDim.y
	);
	Editor_Draw_2DElements(editorCtx);
}

void Editor_Update(EditorContext* editorCtx) {
	Vec2i projDim;
	f64 x;
	f64 y;
	
	glfwGetCursorPos(editorCtx->appInfo.mainWindow, &x, &y);
	editorCtx->inputCtx.mouse.pos.x = x;
	editorCtx->inputCtx.mouse.pos.y = y;
	
	Editor_Region_Update(editorCtx);
	projDim = (Vec2i) {
		editorCtx->regionCtx.view.rect.w,
		editorCtx->regionCtx.view.rect.h
	};
	View_SetProjectionDimensions(&editorCtx->viewCtx, &projDim);
}

void Editor_Init(EditorContext* editorCtx) {
	editorCtx->vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	if (editorCtx->vg == NULL)
		printf_error("Could not init nanovg.");
	editorCtx->fontCtx.notoSansID = nvgCreateFont(editorCtx->vg, "sans", "NotoSans-Regular.ttf");
	
	Editor_Region_SetSplitter(&editorCtx->appInfo, &editorCtx->regionCtx, editorCtx->appInfo.winDim.x * 0.25f, 0.0f);
	
	editorCtx->viewCtx.cameraControl = false;
	editorCtx->regionCtx.side.id = 0;
	editorCtx->regionCtx.bot.id = 1;
	editorCtx->regionCtx.view.id = 2;
}