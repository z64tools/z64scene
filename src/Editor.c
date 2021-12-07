#include "Editor.h"
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

/* / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Region_View(EditorContext* editorCtx, Region* panel) {
	MouseInput* mouse = &editorCtx->inputCtx.mouse;
	InputContext* inputCtx = &editorCtx->inputCtx;
	
	View_Camera_OrbitMode(&editorCtx->viewCtx, inputCtx);
	View_Camera_FlyMode(&editorCtx->viewCtx, inputCtx);
	
	if (inputCtx->mouse.clickL.hold == false &&
	    inputCtx->mouse.clickR.hold == false &&
	    inputCtx->mouse.clickMid.hold == false &&
	    inputCtx->mouse.scrollY == 0)
		editorCtx->regionCtx.actionFunc = NULL;
}

/* / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Editor_Region_UpdateSplitter(AppInfo* appInfo, RegionContext* regionCtx) {
	Region* botRegion = &regionCtx->bot;
	Region* sideRegion = &regionCtx->side;
	Region* viewRegion = &regionCtx->view;
	
	botRegion->pos.x = 0;
	botRegion->pos.y = appInfo->winDim.y - 32;
	botRegion->dim.x = appInfo->winDim.x;
	botRegion->dim.y = 32;
	
	sideRegion->dim.y = appInfo->winDim.y - botRegion->dim.y;
	sideRegion->pos.x = appInfo->winDim.x - sideRegion->dim.x;
	sideRegion->pos.y = 0;
	
	viewRegion->pos.x = 0;
	viewRegion->pos.y = botRegion->dim.y;
	viewRegion->dim.x = sideRegion->pos.x;
	viewRegion->dim.y = appInfo->winDim.y - botRegion->dim.y;
}

void Editor_Region_SetSplitter(AppInfo* appInfo, RegionContext* regionCtx, f32 x, f32 y) {
	Region* sideRegion = &regionCtx->side;
	Region* viewRegion = &regionCtx->view;
	
	sideRegion->dim.x = CLAMP(
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
	
	if (mouse->pos.x >= panel->pos.x && mouse->pos.x < (panel->pos.x + panel->dim.x)) {
		if (mouse->pos.y >= panel->pos.y && mouse->pos.y < (panel->pos.y + panel->dim.y)) {
			if (inputCtx->mouse.clickL.press ||
			    inputCtx->mouse.clickMid.press ||
			    inputCtx->mouse.clickR.press ||
			    inputCtx->mouse.scrollY) {
				regionCtx->actionRegion = panel;
				regionCtx->actionFunc = Region_View;
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
			editorCtx->regionCtx.side.dim.x,
			0
		);
		
		return;
	}
	
	if (!regionCtx->actionFunc) {
		// Editor_Region_SetCursorState(&editorCtx->inputCtx, regionCtx, &regionCtx->side);
		// Editor_Region_SetCursorState(&editorCtx->inputCtx, regionCtx, &regionCtx->bot);
		Editor_Region_SetCursorState(&editorCtx->inputCtx, regionCtx, &regionCtx->view);
	}
	
	if (regionCtx->actionFunc) {
		regionCtx->actionFunc(editorCtx, regionCtx->actionRegion);
	}
	
	Editor_Region_UpdateSplitter(&editorCtx->appInfo, &editorCtx->regionCtx);
}

void Editor_Region_Draw(EditorContext* editorCtx, Region* panel) {
	nvgBeginPath(editorCtx->vg);
	nvgRect(
		editorCtx->vg,
		panel->pos.x,
		panel->pos.y,
		panel->dim.x,
		panel->dim.y
	);
	nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_SPLITTER));
	nvgFill(editorCtx->vg);
	
	nvgBeginPath(editorCtx->vg);
	nvgRoundedRect(
		editorCtx->vg,
		panel->pos.x + 3,
		panel->pos.y + 3,
		panel->dim.x - 6,
		panel->dim.y - 6,
		4.0f
	);
	nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_BASE_DARK));
	nvgFill(editorCtx->vg);
	
	if (panel->state.cursorInRange) {
		nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_BASE_WHITE));
		nvgFontSize(editorCtx->vg, 17.5f);
		nvgFontFace(editorCtx->vg, "sans");
		nvgTextAlign(editorCtx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
		nvgTextBox(
			editorCtx->vg,
			panel->pos.x + 2.0f,
			panel->pos.y + 2.0f,
			550,
			"Focus",
			NULL
		);
	}
}

/* / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Editor_Draw_2DElements(EditorContext* editorCtx) {
	nvgBeginFrame(editorCtx->vg, editorCtx->appInfo.winDim.x, editorCtx->appInfo.winDim.y, 1.0f);
	
	Editor_Region_Draw(editorCtx, &editorCtx->regionCtx.side);
	Editor_Region_Draw(editorCtx, &editorCtx->regionCtx.bot);
	
	nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_BASE_WHITE));
	nvgFontSize(editorCtx->vg, 17.5f);
	nvgFontFace(editorCtx->vg, "sans");
	nvgTextAlign(editorCtx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	
	char txt[2048];
	
	sprintf(txt, "M: x%4d y%4d", editorCtx->inputCtx.mouse.pos.x, editorCtx->inputCtx.mouse.pos.y);
	nvgTextBox(editorCtx->vg, 10.0f, 10.0f, 150, txt, NULL);
	sprintf(txt, "W: x%4d y%4d", (s32)editorCtx->appInfo.winDim.x, (s32)editorCtx->appInfo.winDim.y);
	nvgTextBox(editorCtx->vg, 10.0f, 10.0f + 17.5f, 150, txt, NULL);
	
	nvgEndFrame(editorCtx->vg);
}

void Editor_Draw_3DViewport(EditorContext* editorCtx) {
	z64_Draw_SetScene(&editorCtx->objCtx.scene);
	z64_Draw_Room(&editorCtx->objCtx.room[0]);
}

void Editor_Draw(EditorContext* editorCtx) {
	glViewport(
		editorCtx->regionCtx.view.pos.x,
		editorCtx->regionCtx.view.pos.y,
		editorCtx->regionCtx.view.dim.x,
		editorCtx->regionCtx.view.dim.y
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
	Editor_Region_Update(editorCtx);
	View_SetProjectionDimensions(&editorCtx->viewCtx, &editorCtx->regionCtx.view.dim);
}

void Editor_Init(EditorContext* editorCtx) {
	editorCtx->vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	if (editorCtx->vg == NULL)
		printf_error("Could not init nanovg.");
	editorCtx->fontCtx.notoSansID = nvgCreateFont(editorCtx->vg, "sans", "NotoSans-Regular.ttf");
	
	Editor_Region_SetSplitter(&editorCtx->appInfo, &editorCtx->regionCtx, editorCtx->appInfo.winDim.x * 0.25f, 0.0f);
	
	editorCtx->viewCtx.cameraControl = false;
}