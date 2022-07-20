#include "Editor.h"

void EnSceneView_Draw_Empty(EditorContext*, EnSceneView*, Split*);
void EnSceneView_Draw_3DViewport(EditorContext*, EnSceneView*, Split*);

/* ───────────────────────────────────────────────────────────────────────── */

void EnSceneView_Init(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnSceneView* this = instance;
	
	View_Init(&this->view, &editCtx->input);
	
	Calloc(this->zobj, sizeof(MemFile) * 2);
	
	if (MemFile_LoadFile(&this->zobj[0], "Nora.zobj"))
		printf_error("NO!");
	
	// 0x06001290
	if (MemFile_LoadFile(&this->zobj[1], "object.zobj"))
		printf_error("NO!");
	
	Log("SkelAnime Init");
	
	gSegment[6] = this->zobj[0].data;
	SkelAnime_Init(&this->zobj[0], &this->skelAnime, 0x0600D930, 0x0600FAC4);
	split->bg.useCustomBG = true;
	split->bg.color = (RGB8) { 15, 15, 15 };
}

void EnSceneView_Destroy(void* passArg, void* instance, Split* split) {
	// EditorContext* editCtx = passArg;
	EnSceneView* this = instance;
	
	SkelAnime_Free(&this->skelAnime);
	split->bg.useCustomBG = false;
}

void EnSceneView_Update(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnSceneView* this = instance;
	InputContext* inputCtx = &editCtx->input;
	MouseInput* mouse = &inputCtx->mouse;
	
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
			
			Input_SetMousePos(newPos, MOUSE_KEEP_AXIS);
		}
		
		if (mouse->pos.y < yMin || mouse->pos.y > yMax) {
			s16 newPos = WrapS(mouse->pos.y, yMin, yMax);
			
			Input_SetMousePos(MOUSE_KEEP_AXIS, newPos);
		}
	}
}

void EnSceneView_Draw(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnSceneView* this = instance;
	
	EnSceneView_Draw_3DViewport(editCtx, this, split);
}

/* ───────────────────────────────────────────────────────────────────────── */

void EnSceneView_Draw_Empty(EditorContext* editCtx, EnSceneView* this, Split* split) {
	const char* txt = "z64scene";
	void* vg = editCtx->vg;
	
	nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	nvgFontBlur(vg, 0.0);
	
	nvgFontFace(vg, "dejavu-light");
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
	
	nvgFontFace(vg, "dejavu");
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
	Vec2s dim = {
		split->rect.w,
		split->rect.h
	};
	Ambient amb = {};
	Light light = {};
	
	light.dir.col[0] = 0xFF;
	light.dir.col[1] = 0xFF;
	light.dir.col[2] = 0xFF;
	light.dir.dir[1] = __INT8_MAX__;
	
	amb.l.col[0] = 0xFF;
	amb.l.col[1] = 0xFF;
	amb.l.col[2] = 0xFF;
	
	Log("GraphInit");
	n64_graph_init();
	Log("BindLight");
	n64_bind_light(&light, &amb);
	Log("SetFog");
	z64_Light_SetFog(0x3E1, 1000, (RGB8) { 0xFF, 0xFF, 0xFF });
	
	Log("View");
	View_SetProjectionDimensions(&this->view, &dim);
	View_Update(&this->view, &editCtx->input);
	
	Log("Mtx");
	n64_setMatrix_model(&this->view.modelMtx);
	n64_setMatrix_view(&this->view.viewMtx);
	n64_setMatrix_projection(&this->view.projMtx);
	Log("OK!");
	
	Matrix_Push(); {
		gxSPSegment(POLY_OPA_DISP++, 0x6, this->zobj[0].data);
		
		gDPSetEnvColor(POLY_OPA_DISP++, 0xFF, 0xFF, 0xFF, 0xFF);
		
		Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
		Matrix_Translate(0, 0, 0, MTXMODE_APPLY);
		
		gSPMatrix(POLY_OPA_DISP++, Matrix_ToMtx(n64_graph_alloc(sizeof(Mtx*))), G_MTX_MODELVIEW | G_MTX_LOAD);
		
		SkelAnime_Update(&this->skelAnime);
		SkelAnime_Draw(&this->skelAnime, SKELANIME_FLEX);
	} Matrix_Pop();
	
	Matrix_Push(); {
		gSegment[6] = this->zobj[1].data;
		gSPSegment(POLY_OPA_DISP++, 0x6, this->zobj[1].data);
		
		Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
		gSPMatrix(POLY_OPA_DISP++, Matrix_ToMtx(n64_graph_alloc(sizeof(Mtx*))), G_MTX_MODELVIEW | G_MTX_LOAD);
		
		gDPSetEnvColor(POLY_OPA_DISP++, 0xFF, 0xFF, 0xFF, 0xFF);
		
		gSPDisplayList(POLY_OPA_DISP++, 0x060017C0);
	} Matrix_Pop();
	
	gSPEndDisplayList(POLY_OPA_DISP++);
	n64_draw(gPolyOpaHead);
}