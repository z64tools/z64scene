#include "Editor.h"
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

void Editor_Subscreen_Update(AppInfo* appInfo, f32 x, f32 y) {
	PosDim* sidePanel = &appInfo->subscreen.sidePanel;
	PosDim* view3D = &appInfo->subscreen.view3D;
	
	sidePanel->dim.x = x;
	sidePanel->dim.y = appInfo->winDim.y;
	sidePanel->pos.x = appInfo->winDim.x - sidePanel->dim.x;
	sidePanel->pos.y = 0;
	view3D->pos.x = 0;
	view3D->pos.y = 0;
	view3D->dim.x = sidePanel->pos.x;
	view3D->dim.y = appInfo->winDim.y;
}

/* / / / / / */

void Editor_Draw_SidePanel(EditorContext* editorCtx) {
	NVGpaint gradient;
	PosDim* sidePanel = &editorCtx->appInfo.subscreen.sidePanel;
	PosDim* view3D = &editorCtx->appInfo.subscreen.view3D;
	
	gradient = nvgLinearGradient(
		editorCtx->vg,
		sidePanel->pos.x,
		0,
		sidePanel->pos.x + 2,
		0,
		Element_GetColor(GUICOL_BASE_WHITE),
		Element_GetColor(GUICOL_BASE_DARK)
	);
	
	nvgBeginPath(editorCtx->vg);
	nvgRect(editorCtx->vg, sidePanel->pos.x, sidePanel->pos.y, sidePanel->dim.x, sidePanel->dim.y);
	nvgFillPaint(editorCtx->vg, gradient);
	nvgFill(editorCtx->vg);
	
	nvgFillColor(editorCtx->vg, Element_GetColor(GUICOL_BASE_WHITE));
	nvgFontSize(editorCtx->vg, 17.5f);
	nvgFontFace(editorCtx->vg, "sans");
	nvgTextAlign(editorCtx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	nvgTextBox(
		editorCtx->vg,
		sidePanel->pos.x + 10.0f,
		sidePanel->pos.y + 10.0f,
		150,
		"[Side Panel]",
		NULL
	);
}

void Editor_Draw_2DElements(EditorContext* editorCtx) {
	nvgBeginFrame(editorCtx->vg, editorCtx->appInfo.winDim.x, editorCtx->appInfo.winDim.y, 1.0f);
	
	Editor_Draw_SidePanel(editorCtx);
	
	Element_UpdateElements(editorCtx, &editorCtx->elemCtx);
	Element_DrawElements(editorCtx, &editorCtx->elemCtx);
	
	nvgFillColor(editorCtx->vg, Element_GetColor(GUICOL_BASE_WHITE));
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

void Editor_Update(EditorContext* editorCtx) {
	PosDim* sidePanel = &editorCtx->appInfo.subscreen.sidePanel;
	PosDim* view3D = &editorCtx->appInfo.subscreen.view3D;
	MouseInput* mouse = &editorCtx->inputCtx.mouse;
	s32 winDimDiff = editorCtx->appInfo.winDim.x - editorCtx->appInfo.prevWinDim.x;
	bool controlHold = (editorCtx->inputCtx.mouse.clickL.hold || editorCtx->inputCtx.mouse.clickMid.hold);
	bool controlPress = (editorCtx->inputCtx.mouse.clickL.press || editorCtx->inputCtx.mouse.clickMid.press);
	static bool lockResizeState;
	static bool lockWrapState;
	
	if (editorCtx->appInfo.isCallback == true) {
		Editor_Subscreen_Update(&editorCtx->appInfo, sidePanel->dim.x, 0);
	} else {
		if (!controlHold) {
			lockResizeState = false;
			lockWrapState = false;
		}
		
		if (ABS(mouse->pos.x - sidePanel->pos.x) < 5 || lockResizeState == true) {
			if (editorCtx->inputCtx.mouse.clickL.press || lockResizeState == true) {
				Editor_Subscreen_Update(
					&editorCtx->appInfo,
					CLAMP(
						editorCtx->appInfo.winDim.x - mouse->pos.x,
						100,
						editorCtx->appInfo.winDim.x - 100
					),
					0
				);
				lockResizeState = true;
			}
		}
		
		if (mouse->pos.x < view3D->dim.x - 5 && mouse->pos.x > 5) {
			editorCtx->viewCtx.cameraControl = true;
			if (controlPress) {
				lockWrapState = true;
			}
		} else {
			if (lockWrapState == true) {
				if (mouse->pos.x > view3D->dim.x * 0.5) {
					mouse->jumpVelComp.x = mouse->pos.x - 6;
					glfwSetCursorPos(
						editorCtx->appInfo.mainWindow,
						6,
						mouse->pos.y
					);
				} else {
					mouse->jumpVelComp.x = -(view3D->dim.x - 6 - 5);
					glfwSetCursorPos(
						editorCtx->appInfo.mainWindow,
						view3D->dim.x - 6,
						mouse->pos.y
					);
				}
			} else {
				editorCtx->viewCtx.cameraControl = false;
			}
		}
	}
	
	view3D->pos.x = 0;
	view3D->pos.y = 0;
	view3D->dim.x = sidePanel->pos.x;
	view3D->dim.y = editorCtx->appInfo.winDim.y;
}

void Editor_Init(EditorContext* editorCtx) {
	editorCtx->vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	if (editorCtx->vg == NULL)
		printf_error("Could not init nanovg.");
	editorCtx->fontCtx.notoSansID = nvgCreateFont(editorCtx->vg, "sans", "NotoSans-Regular.ttf");
	
	Editor_Subscreen_Update(&editorCtx->appInfo, 250.0f, 0.0f);
}