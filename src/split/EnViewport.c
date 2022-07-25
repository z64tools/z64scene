#include "EnViewport.h"

void EnViewport_Init(Editor* editor, EnViewport* this, Split* split);
void EnViewport_Destroy(Editor* editor, EnViewport* this, Split* split);
void EnViewport_Update(Editor* editor, EnViewport* this, Split* split);
void EnViewport_Draw(Editor* editor, EnViewport* this, Split* split);

SplitTask gEnViewportTask = DefineTask("Viewport", EnViewport);

void EnViewport_Draw_Empty(Editor*, EnViewport*, Split*);
void EnViewport_Draw_3DViewport(Editor*, EnViewport*, Split*);

/* ───────────────────────────────────────────────────────────────────────── */

void EnViewport_Init(Editor* editor, EnViewport* this, Split* split) {
	View_Init(&this->view, &editor->input);
}

void EnViewport_Destroy(Editor* editor, EnViewport* this, Split* split) {
	split->bg.useCustomBG = false;
}

void EnViewport_Update(Editor* editor, EnViewport* this, Split* split) {
	Input* inputCtx = &editor->input;
	MouseInput* mouse = &inputCtx->mouse;
	
	if (editor->scene.segment == NULL)
		return;
	
	if (split->mouseInHeader && mouse->click.press) {
		this->headerClick = true;
	}
	
	if (this->headerClick == true && mouse->cursorAction == false) {
		this->headerClick = false;
	}
	
	if (this->view.setCamMove == false) {
		this->view.cameraControl = false;
		if (split->blockMouse == false) {
			if (split->mouseInSplit && !split->mouseInHeader && !this->headerClick) {
				this->view.cameraControl = true;
			}
		}
	}
	
	// Cursor Wrapping
	if (this->view.setCamMove && this->view.cameraControl) {
		s16 xMin = split->edge[EDGE_L]->pos;
		s16 xMax = split->edge[EDGE_R]->pos;
		s16 yMin = split->edge[EDGE_T]->pos;
		s16 yMax = split->edge[EDGE_B]->pos - SPLIT_BAR_HEIGHT;
		
		Cursor_ForceCursor(CURSOR_DEFAULT);
		
		if (mouse->pos.x < xMin || mouse->pos.x > xMax) {
			s16 newPos = WrapS(mouse->pos.x, xMin, xMax);
			
			Input_SetMousePos(&editor->input, newPos, MOUSE_KEEP_AXIS);
		}
		
		if (mouse->pos.y < yMin || mouse->pos.y > yMax) {
			s16 newPos = WrapS(mouse->pos.y, yMin, yMax);
			
			Input_SetMousePos(&editor->input, MOUSE_KEEP_AXIS, newPos);
		}
	}
	
	if (editor->scene.segment) {
		EnvLightSettings* env = editor->scene.env + editor->scene.setupEnv;
		memcpy(split->bg.color.c, env->fogColor, 3);
		this->view.far = env->fogFar;
	}
}

void EnViewport_Draw(Editor* editor, EnViewport* this, Split* split) {
	if (editor->scene.segment == NULL)
		EnViewport_Draw_Empty(editor, this, split);
	else
		EnViewport_Draw_3DViewport(editor, this, split);
}

/* ───────────────────────────────────────────────────────────────────────── */

void EnViewport_Draw_Empty(Editor* editor, EnViewport* this, Split* split) {
	const char* txt = "z64scene";
	void* vg = editor->vg;
	
	split->bg.useCustomBG = false;
	nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	nvgFontBlur(vg, 0.0);
	
	nvgFontFace(vg, "dejavu-light");
	nvgFontSize(vg, 35);
	nvgTextLetterSpacing(vg, 2.0f);
	nvgFillColor(vg, Theme_GetColor(THEME_TEXT, 255, 1.0f));
	nvgText(
		vg,
		split->rect.w * 0.5,
		split->rect.h * 0.5,
		txt,
		NULL
	);
	
	nvgFontFace(vg, "dejavu");
	nvgFontSize(vg, 15);
	nvgTextLetterSpacing(vg, 0.0f);
	nvgFillColor(vg, nvgHSLA(0, 0, 0.35, 255));
	nvgText(
		vg,
		split->rect.w * 0.5,
		split->rect.h * 0.5 + 35 * 0.75f,
		"drop files here",
		NULL
	);
}

void EnViewport_Draw_3DViewport(Editor* editor, EnViewport* this, Split* split) {
	Vec2s dim = {
		split->rect.w,
		split->rect.h
	};
	
	split->bg.useCustomBG = true;
	n64_graph_init();
	
	View_SetProjectionDimensions(&this->view, &dim);
	View_Update(&this->view, &editor->input);
	
	n64_setMatrix_model(&this->view.modelMtx);
	n64_setMatrix_view(&this->view.viewMtx);
	n64_setMatrix_projection(&this->view.projMtx);
	
	if (editor->scene.segment)
		Scene_Draw(&editor->scene);
	
	// Matrix_Push(); {
	// 	gxSPSegment(POLY_OPA_DISP++, 0x6, this->zobj[0].data);
	//
	// 	gDPSetEnvColor(POLY_OPA_DISP++, 0xFF, 0xFF, 0xFF, 0xFF);
	//
	// 	Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
	// 	Matrix_Translate(0, 0, 0, MTXMODE_APPLY);
	//
	// 	gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
	//
	// 	SkelAnime_Update(&this->skelAnime);
	// 	SkelAnime_Draw(&this->skelAnime, SKELANIME_FLEX);
	// } Matrix_Pop();
	//
	// Matrix_Push(); {
	// 	gSegment[6] = this->zobj[1].data;
	// 	gSPSegment(POLY_OPA_DISP++, 0x6, this->zobj[1].data);
	//
	// 	Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
	// 	gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
	//
	// 	gDPSetEnvColor(POLY_OPA_DISP++, 0xFF, 0xFF, 0xFF, 0xFF);
	//
	// 	gSPDisplayList(POLY_OPA_DISP++, 0x060017C0);
	// } Matrix_Pop();
	
	gSPEndDisplayList(POLY_OPA_DISP++);
	n64_draw(gPolyOpaHead);
}