#include "Editor.h"

s32 gSceneConfIndex = 0;
s64 sFirstInit = 0;

void EnSceneView_Draw_Empty(EditorContext*, EnSceneView*, Split*);
void EnSceneView_Draw_3DViewport(EditorContext*, EnSceneView*, Split*);

void (*sDrawFunc[2])(EditorContext*, EnSceneView*, Split*) = {
	EnSceneView_Draw_Empty,
	EnSceneView_Draw_3DViewport,
};
u32 sDrawId;

/* ───────────────────────────────────────────────────────────────────────── */

void EnSceneView_Init(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnSceneView* this = instance;
	
	View_Init(&this->view, &editCtx->input);
	
	split->bg.useCustomBG = true;
	this->view.matchDrawDist = true;
}

void EnSceneView_Destroy(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnSceneView* this = instance;
	
	split->bg.useCustomBG = false;
}

void EnSceneView_Update(void* passArg, void* instance, Split* split) {
}

void EnSceneView_Draw(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnSceneView* this = instance;
	
	sDrawFunc[sDrawId](editCtx, this, split);
}

/* ───────────────────────────────────────────────────────────────────────── */

void EnSceneView_Draw_Empty(EditorContext* editCtx, EnSceneView* this, Split* split) {
	const char* txt = "z64scene";
	void* vg = editCtx->vg;
	f32 boundr[4];
	
	nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	nvgFontBlur(vg, 0.0);
	
	nvgFontFace(vg, "font-bold");
	nvgFontSize(vg, 35);
	nvgTextLetterSpacing(vg, 2.0f);
	nvgFillColor(vg, Theme_GetColor(THEME_ACCENT, 255, 1.0f));
	nvgText(
		vg,
		split->cect.w * 0.5,
		split->cect.h * 0.5,
		txt,
		NULL
	);
	
	nvgFontFace(vg, "font-basic");
	nvgFontSize(vg, 15);
	nvgTextLetterSpacing(vg, 0.0f);
	nvgFillColor(vg, nvgHSLA(0, 0, 0.35, 255));
	nvgText(
		vg,
		split->cect.w * 0.5,
		split->cect.h * 0.5 + 35 * 0.75f,
		"drop files here",
		NULL
	);
}

void EnSceneView_Draw_3DViewport(EditorContext* editCtx, EnSceneView* this, Split* split) {
}