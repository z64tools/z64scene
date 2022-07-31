#include "EnViewport.h"
#include <UnProject.h>

#define INCBIN_PREFIX
#include <incbin.h>
INCBIN(gGizmo_, "assets/3D/Gizmo.zobj");

void EnViewport_Init(Editor* editor, EnViewport* this, Split* split);
void EnViewport_Destroy(Editor* editor, EnViewport* this, Split* split);
void EnViewport_Update(Editor* editor, EnViewport* this, Split* split);
void EnViewport_Draw(Editor* editor, EnViewport* this, Split* split);

SplitTask gEnViewportTask = DefineTask("Viewport", EnViewport);

void EnViewport_Draw_Empty(Editor*, EnViewport*, Split*);
void EnViewport_Draw_3DViewport(Editor*, EnViewport*, Split*);

/* ───────────────────────────────────────────────────────────────────────── */

// MemFile gNora;

static Gfx gPolyGuiHead[1024];
static Gfx* gPolyGuiDisp;
#define POLY_GUI_DISP gPolyGuiDisp

static void Gizmo_Draw(Editor* editor, ViewContext* view, Vec3f pos) {
	for (s32 i = 0; i < 3; i++) {
		gSPSegment(POLY_GUI_DISP++, 6, (void*)gGizmo_Data);
		Matrix_Push(); {
			Matrix_Translate(UnfoldVec3(pos), MTXMODE_APPLY);
			Matrix_Push(); {
				f32 scale = Math_Vec3f_DistXYZ(pos, view->currentCamera->eye) * 0.000015;
				scale = 0.01; // testing depth
				Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
				
				if (i == 0)
					gDPSetEnvColor(POLY_GUI_DISP++, 0xFF, 0x40, 0x40, 0xA0);
				
				if (i == 1) {
					gDPSetEnvColor(POLY_GUI_DISP++, 0x40, 0xFF, 0x40, 0xA0);
					Matrix_RotateX_s(DegToBin(90), MTXMODE_APPLY);
				}
				if (i == 2) {
					gDPSetEnvColor(POLY_GUI_DISP++, 0x40, 0x40, 0xFF, 0xA0);
					Matrix_RotateZ_s(DegToBin(90), MTXMODE_APPLY);
				}
				
				gSPMatrix(POLY_GUI_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
				gSPDisplayList(POLY_GUI_DISP++, 0x060011E0);
			} Matrix_Pop();
		} Matrix_Pop();
	}
}

void EnViewport_Init(Editor* editor, EnViewport* this, Split* split) {
	View_Init(&this->view, &editor->input);
	
	// MemFile_LoadFile(&gNora, "Nora.zobj");
	// SkelAnime_Init(&gNora, &this->skelAnime, 0x0600D978, 0x0600EF44);
}

void EnViewport_Destroy(Editor* editor, EnViewport* this, Split* split) {
	split->bg.useCustomBG = false;
}

void EnViewport_Update(Editor* editor, EnViewport* this, Split* split) {
	Input* inputCtx = &editor->input;
	MouseInput* mouse = &inputCtx->mouse;
	
	Element_Header(split, split->taskCombo, 128);
	Element_Combo(split->taskCombo);
	
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
		
		if (split->blockMouse == false && split->mouseInSplit)
			this->view.cameraControl = true;
	}
	
// Cursor Wrapping
	if (this->view.setCamMove && this->view.cameraControl) {
		s16 xMin = split->edge[EDGE_L]->pos;
		s16 xMax = split->edge[EDGE_R]->pos;
		s16 yMin = split->edge[EDGE_T]->pos;
		s16 yMax = split->edge[EDGE_B]->pos;
		
		if (mouse->pos.x < xMin || mouse->pos.x > xMax)
			Input_SetMousePos(&editor->input, WrapS(mouse->pos.x, xMin, xMax), MOUSE_KEEP_AXIS);
		
		if (mouse->pos.y < yMin || mouse->pos.y > yMax)
			Input_SetMousePos(&editor->input, MOUSE_KEEP_AXIS, WrapS(mouse->pos.y, yMin, yMax));
	}
	
	if (editor->scene.segment) {
		EnvLightSettings* env = editor->scene.env + editor->scene.setupEnv;
		memcpy(split->bg.color.c, env->fogColor, 3);
		this->view.far = env->fogFar;
	}
}

static void ProfilerText(void* vg, s32 row, const char* msg, const char* fmt, f32 val, f32 dangerValue) {
	nvgFontSize(vg, 12);
	nvgFontFace(vg, "dejavu");
	nvgFontBlur(vg, 1.0f);
	nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
	for (s32 i = 0; i < 2; i++)
		nvgText(vg, 8, 8 + SPLIT_TEXT_H * row, msg, NULL);
	
	nvgFontBlur(vg, 0.0f);
	nvgFillColor(vg, nvgRGBA(255, 255, 255, 225));
	nvgText(vg, 8, 8 + SPLIT_TEXT_H * row, msg, NULL);
	
	nvgFontFace(vg, "dejavu-bold");
	nvgFontBlur(vg, 1.0f);
	nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
	for (s32 i = 0; i < 2; i++)
		nvgText(vg, 8 + 120, 8 + SPLIT_TEXT_H * row, xFmt(fmt, val), NULL);
	
	nvgFontBlur(vg, 0.0f);
	if (dangerValue)
		nvgFillColor(vg, nvgHSLA(SQ(Clamp(val / dangerValue, 0, 1)) * 0.5f + 0.5f, 0.6, 0.6, 225));
	else
		nvgFillColor(vg, nvgRGBA(255, 255, 255, 225));
	nvgText(vg, 8 + 120, 8 + SPLIT_TEXT_H * row, xFmt(fmt, val), NULL);
}

void EnViewport_Draw(Editor* editor, EnViewport* this, Split* split) {
	void* vg = editor->vg;
	
	Profiler_O(4);
	Profiler_I(4);
	
	if (editor->scene.segment == NULL)
		EnViewport_Draw_Empty(editor, this, split);
	else
		EnViewport_Draw_3DViewport(editor, this, split);
	
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	nvgFontSize(vg, 15);
	nvgTextLetterSpacing(vg, 0.0f);
	
	ProfilerText(vg, 0, "FPS:", "%.0f", 1 / Profiler_Time(4), 0);
	ProfilerText(vg, 1, "N64 Render:", "%.2fms", Profiler_Time(0) * 1000.f, 16.0f);
	ProfilerText(vg, 2, "View:", "%.2fms", Profiler_Time(1) * 1000.f, 16.0f);
	ProfilerText(vg, 3, "Delta:", "%.2f", gDeltaTime, 0);
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
	
	Log("Draw");
	split->bg.useCustomBG = true;
	n64_graph_init();
	
	Profiler_I(1);
	View_SetProjectionDimensions(&this->view, &dim);
	View_Update(&this->view, &editor->input);
	Profiler_O(1);
	
	n64_setMatrix_model(&this->view.modelMtx);
	n64_setMatrix_view(&this->view.viewMtx);
	n64_setMatrix_projection(&this->view.projMtx);
	
	if (editor->scene.segment)
		Scene_Draw(&editor->scene);
	
	// Matrix_Push(); {
	// 	gxSPSegment(POLY_OPA_DISP++, 0x6, gNora.data);
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
	
	Profiler_I(0);
	gSPEndDisplayList(POLY_OPA_DISP++);
	gSPEndDisplayList(POLY_XLU_DISP++);
	Assert(POLY_OPA_DISP < &gPolyOpaHead[4096]);
	Assert(POLY_XLU_DISP < &gPolyXluHead[4096]);
	n64_draw(gPolyOpaHead);
	n64_draw(gPolyXluHead);
	n64_set_culling(editor->render.culling);
	
	// Draw gizmo at mouse position in 3D space
	{
		Input* inputCtx = &editor->input;
		MouseInput* mouse = &inputCtx->mouse;
		GLfloat x = mouse->pos.x;
		GLfloat y = dim.y - mouse->pos.y + 27; // TODO GeoGrid offsets
		GLfloat z;
		Vec3f result;
		int viewport[] = { 0, 0, dim.x, dim.y };
		MtxF modelview;
		
		Matrix_MtxFMtxFMult(&this->view.modelMtx, &this->view.viewMtx, &modelview);
		
		glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
		glhUnProjectf(x, y, z, (float*)&modelview, (float*)&this->view.projMtx, viewport, (float*)&result);
		
		gPolyGuiDisp = gPolyGuiHead;
		Gizmo_Draw(editor, &this->view, result);
		gSPEndDisplayList(POLY_GUI_DISP++);
		n64_draw(gPolyGuiHead);
	}
	
	Profiler_O(0);
}