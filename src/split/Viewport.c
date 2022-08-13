#include "Viewport.h"
#include <float.h>

extern DataFile gSpot;

void Viewport_Init(Editor* editor, Viewport* this, Split* split);
void Viewport_Destroy(Editor* editor, Viewport* this, Split* split);
void Viewport_Update(Editor* editor, Viewport* this, Split* split);
void Viewport_Draw(Editor* editor, Viewport* this, Split* split);

SplitTask gViewportTask = {
	.taskName = "Viewport",
	.init     = (void*)Viewport_Init,
	.destroy  = (void*)Viewport_Destroy,
	.update   = (void*)Viewport_Update,
	.draw     = (void*)Viewport_Draw,
	.size     = sizeof(Viewport)
};

// # # # # # # # # # # # # # # # # # # # #
// #                                     #
// # # # # # # # # # # # # # # # # # # # #

static Gfx* gVOPAHead;
static Gfx* gVOPADisp;
static Gfx* gVXLUHead;
static Gfx* gVXLUDisp;
static s32 gInstance;

#define POLY_VOPA_DISP gVOPADisp
#define POLY_VXLU_DISP gVXLUDisp

static void Viewport_DrawSpot(Vec3f pos, f32 scale, NVGcolor color) {
	Matrix_Push();
	Matrix_Translate(UnfoldVec3(pos), MTXMODE_APPLY);
	Matrix_Scale(0.01 * scale, 0.01 * scale, 0.01 * scale, MTXMODE_APPLY);
	
	gSPSegment(POLY_OPA_DISP++, 6, (void*)gSpot.data);
	gDPSetEnvColor(POLY_OPA_DISP++, UnfoldNVGcolor(color), color.a * 255);
	gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
	gSPDisplayList(POLY_OPA_DISP++, 0x06007730);
	Matrix_Pop();
}

static void Viewport_CameraUpdate(Editor* editor, Viewport* this, Split* split) {
	Input* inputCtx = &editor->input;
	MouseInput* mouse = &inputCtx->mouse;
	Gizmo* gizmo = &this->gizmo;
	View3D* view = &this->view;
	
	if (Input_GetMouse(inputCtx, MOUSE_ANY)->hold && this->lockCameraAccess)
		return;
	this->lockCameraAccess = false;
	
	if (!View_CheckControlKeys(inputCtx) && !split->inputAccess)
		view->cameraControl = false;
	
	if (Input_GetMouse(inputCtx, MOUSE_ANY)->press && !split->inputAccess) {
		view->cameraControl = false;
		this->lockCameraAccess = true;
		
		return;
	}
	
	if (!view->cameraControl) {
		if (split->inputAccess) {
			if (View_CheckControlKeys(inputCtx))
				view->cameraControl = true;
		}
	}
	
	if (gizmo->lock.state)
		view->cameraControl = false;
	
	if (view->isControlled)
		view->cameraControl = true;
	
	if (split->mouseInSplit) {
		if (mouse->clickMid.press && Input_GetKey(inputCtx, KEY_LEFT_ALT)->hold) {
			Vec3f o;
			s32 r = 0;
			RayLine ray = View_GetCursorRayLine(&this->view);
			
			for (s32 i = 0; i < editor->scene.numRoom; i++) {
				RoomHeader* room = Scene_GetRoomHeader(&editor->scene, i);
				
				if (Col3D_LineVsTriBuffer(&ray, &room->mesh->triBuf, &o, NULL))
					r = true;
			}
			
			if (r)
				View_MoveTo(&this->view, o);
		}
		
		if (Input_GetKey(inputCtx, KEY_LEFT_CONTROL)->hold && mouse->clickL.press) {
			RayLine ray = View_GetCursorRayLine(&this->view);
			Room* room = Room_Raycast(&editor->scene, &ray, NULL);
			
			if (room)
				room->state ^= ROOM_SELECTED;
		}
	}
}

static void Viewport_UpdateActors(Editor* editor, Viewport* this, Split* split) {
	RayLine ray = View_GetCursorRayLine(&this->view);
	ActorList* list = (void*)editor->scene.dataCtx.head[SCENE_CMD_ID_ACTOR_LIST];
	Input* input = &editor->input;
	Actor* selectedActor = NULL;
	Vec3f p;
	
	if (Gizmo_IsBusy(&this->gizmo) || !Input_GetMouse(input, MOUSE_L)->press)
		return;
	
	// Process rooms so that we do not grab actors behind walls
	if (Room_Raycast(&editor->scene, &ray, NULL))
		ray.nearest += 20.0f;
	
	while (list) {
		Actor* actor = list->head;
		for (s32 i = 0; i < list->num; i++, actor++) {
			if (actor->state & ACTOR_SELECTED)
				continue;
			
			veccpy(&actor->sph.pos, &actor->pos);
			actor->sph.pos.y += 10.0f;
			actor->sph.r = 20;
			
			if (Col3D_LineVsSphere(&ray, &actor->sph, &p))
				selectedActor = actor;
		}
		
		list = (void*)list->data.next;
	}
	
	if (!Input_GetKey(input, KEY_LEFT_SHIFT)->hold) {
		this->curActor = NULL;
		list = (void*)editor->scene.dataCtx.head[SCENE_CMD_ID_ACTOR_LIST];
		
		while (list) {
			Actor* actor = list->head;
			for (s32 i = 0; i < list->num; i++, actor++) {
				actor->state &= ~ACTOR_SELECTED;
				
				for (s32 j = 0; j < 3; j++)
					actor->pos.axis[j] = rint(actor->pos.axis[j]);
			}
			
			list = (void*)list->data.next;
		}
	}
	
	if (selectedActor) {
		this->curActor = selectedActor;
		selectedActor->state |= ACTOR_SELECTED;
		this->gizmo.initpos = this->gizmo.ppos = this->gizmo.pos = Math_Vec3f_New(UnfoldVec3(this->curActor->pos));
		this->gizmo.vel = Math_Vec3f_New(0, 0, 0);
	}
}

// # # # # # # # # # # # # # # # # # # # #
// #                                     #
// # # # # # # # # # # # # # # # # # # # #

void Viewport_Init(Editor* editor, Viewport* this, Split* split) {
	View_Init(&this->view, &editor->input);
	
	// MemFile_LoadFile(&gNora, "Nora.zobj");
	// SkelAnime_Init(&gNora, &this->skelAnime, 0x0600D978, 0x0600EF44);
	
	if (gInstance++ == 0) {
		gVOPAHead = SysAlloc(sizeof(Gfx) * 4096);
		gVXLUHead = SysAlloc(sizeof(Gfx) * 4096);
	}
}

void Viewport_Destroy(Editor* editor, Viewport* this, Split* split) {
	split->bg.useCustomBG = false;
	
	if (--gInstance == 0) {
		Free(gVOPAHead);
		Free(gVXLUHead);
	}
}

void Viewport_Update(Editor* editor, Viewport* this, Split* split) {
	SceneHeader* header;
	EnvLightSettings* env;
	MouseInput* mouse = &editor->input.mouse;
	Gizmo* gizmo = &this->gizmo;
	
	Element_Header(split, split->taskCombo, 128);
	Element_Combo(split->taskCombo);
	
	if (editor->scene.segment == NULL)
		return;
	
	Viewport_CameraUpdate(editor, this, split);
	header = Scene_GetSceneHeader(&editor->scene);
	env = header->lightList->env + editor->scene.curEnv;
	memcpy(split->bg.color.c, env->fogColor, 3);
	
	if (editor->scene.state & SCENE_DRAW_FOG)
		this->view.far = env->fogFar;
	else
		this->view.far = 12800.0 + 6000;
	
	// Cursor Wrapping
	if (Input_GetMouse(&editor->input, MOUSE_ANY)->press)
		return;
	
	if ((this->view.cameraControl && editor->input.mouse.click.hold) || gizmo->lock.state) {
		s16 xMin = split->rect.x;
		s16 xMax = split->rect.x + split->rect.w;
		s16 yMin = split->rect.y;
		s16 yMax = split->rect.y + split->rect.h;
		
		if (mouse->pos.x < xMin || mouse->pos.x > xMax)
			Input_SetMousePos(&editor->input, WrapS(mouse->pos.x, xMin, xMax), MOUSE_KEEP_AXIS);
		
		if (mouse->pos.y < yMin || mouse->pos.y > yMax)
			Input_SetMousePos(&editor->input, MOUSE_KEEP_AXIS, WrapS(mouse->pos.y, yMin, yMax));
	}
}

static void ProfilerText(void* vg, s32 row, const char* msg, const char* fmt, f32 val, f32 dangerValue) {
	nvgFontSize(vg, 12);
	nvgFontFace(vg, "default");
	nvgFontBlur(vg, 1.0f);
	nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
	for (s32 i = 0; i < 2; i++)
		nvgText(vg, 8, 8 + SPLIT_TEXT_H * row, msg, NULL);
	
	nvgFontBlur(vg, 0.0f);
	nvgFillColor(vg, nvgRGBA(255, 255, 255, 225));
	nvgText(vg, 8, 8 + SPLIT_TEXT_H * row, msg, NULL);
	
	nvgFontFace(vg, "default-bold");
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

static void ProfilerText2(void* vg, s32 row, const char* msg, const char* fmt, ...) {
	char* txt;
	va_list va;
	
	va_start(va, fmt);
	vasprintf(&txt, fmt, va);
	va_end(va);
	
	nvgFontSize(vg, 12);
	nvgFontFace(vg, "default");
	nvgFontBlur(vg, 1.0f);
	nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
	for (s32 i = 0; i < 2; i++)
		nvgText(vg, 8, 8 + SPLIT_TEXT_H * row, msg, NULL);
	
	nvgFontBlur(vg, 0.0f);
	nvgFillColor(vg, nvgRGBA(255, 255, 255, 225));
	nvgText(vg, 8, 8 + SPLIT_TEXT_H * row, msg, NULL);
	
	nvgFontFace(vg, "default-bold");
	nvgFontBlur(vg, 1.0f);
	nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
	for (s32 i = 0; i < 2; i++)
		nvgText(vg, 8 + 120, 8 + SPLIT_TEXT_H * row, txt, NULL);
	
	nvgFontBlur(vg, 0.0f);
	nvgFillColor(vg, nvgRGBA(255, 255, 255, 225));
	nvgText(vg, 8 + 120, 8 + SPLIT_TEXT_H * row, txt, NULL);
	
	Free(txt);
}

static bool N64_CullingCallback(void* userData, const n64_cullingCallbackData* vtx, u32 numVtx) {
	View3D* view = userData;
	
	if (view->ortho)
		return false;
	
	for (s32 i = 0; i < numVtx; i++, vtx++) {
		if (!View_PointInScreen(userData, Math_Vec3f_New(vtx->x, vtx->y, vtx->z)))
			continue;
		
		return false;
	}
	
	return true;
}

static void Viewport_InitDraw(Editor* editor, Viewport* this, Split* split) {
	split->bg.useCustomBG = true;
	n64_graph_init();
	n64_set_culling(editor->scene.state & SCENE_DRAW_CULLING);
	
	View_Update(&this->view, &editor->input, split);
	
	n64_setMatrix_model(&this->view.modelMtx);
	n64_setMatrix_view(&this->view.viewMtx);
	n64_setMatrix_projection(&this->view.projMtx);
	n64_set_cullingCallbackFunc(&this->view, N64_CullingCallback);
}

static void Viewport_DrawInfo(Editor* editor, Viewport* this, Split* split) {
	const char* txt = "z64scene";
	void* vg = editor->vg;
	
	split->bg.useCustomBG = false;
	nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	nvgFontBlur(vg, 0.0);
	
	nvgFontFace(vg, "default-bold");
	nvgFontSize(vg, 35);
	nvgFillColor(vg, Theme_GetColor(THEME_TEXT, 255, 1.0f));
	nvgText(
		vg,
		split->rect.w * 0.5,
		split->rect.h * 0.5,
		txt,
		NULL
	);
	
	nvgFontFace(vg, "default");
	nvgFontSize(vg, 15);
	nvgFillColor(vg, Theme_GetColor(THEME_BASE, 255, 2.0f));
	nvgText(
		vg,
		split->rect.w * 0.5,
		split->rect.h * 0.5 + 35 * 0.75f,
		"drop files here",
		NULL
	);
}

static void Viewport_DrawViewport(Editor* editor, Viewport* this, Split* split) {
	Scene* scene = &editor->scene;
	
	Viewport_InitDraw(editor, this, split);
	
	gVOPADisp = gVOPAHead;
	gVXLUDisp = gVXLUHead;
	
	Profiler_I(2);
	if (this->curActor) Gizmo_Update(&this->gizmo, scene, &this->view, &editor->input);
	Viewport_UpdateActors(editor, this, split);
	Profiler_O(2);
	
	Profiler_I(0);
	Scene_Draw(&editor->scene, &this->view);
	Profiler_O(0);
	
	if (this->curActor) Gizmo_Draw(&this->gizmo, &this->view, &POLY_VOPA_DISP);
	
	gSPEndDisplayList(POLY_VOPA_DISP++);
	gSPEndDisplayList(POLY_VXLU_DISP++);
	n64_draw(gVOPAHead);
	n64_draw(gVXLUHead);
	
#if 0
	Matrix_Push(); {
		gxSPSegment(POLY_OPA_DISP++, 0x6, gNora.data);
		
		gDPSetEnvColor(POLY_OPA_DISP++, 0xFF, 0xFF, 0xFF, 0xFF);
		
		Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
		Matrix_Translate(0, 0, 0, MTXMODE_APPLY);
		
		gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
		
		SkelAnime_Update(&this->skelAnime);
		SkelAnime_Draw(&this->skelAnime, SKELANIME_FLEX);
	} Matrix_Pop();
	
	Matrix_Push(); {
		gSegment[6] = this->zobj[1].data;
		gSPSegment(POLY_OPA_DISP++, 0x6, this->zobj[1].data);
		
		Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
		gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
		
		gDPSetEnvColor(POLY_OPA_DISP++, 0xFF, 0xFF, 0xFF, 0xFF);
		
		gSPDisplayList(POLY_OPA_DISP++, 0x060017C0);
	} Matrix_Pop();
#endif
}

void Viewport_Draw(Editor* editor, Viewport* this, Split* split) {
	void* vg = editor->vg;
	
	if (editor->scene.segment == NULL)
		Viewport_DrawInfo(editor, this, split);
	else
		Viewport_DrawViewport(editor, this, split);
	
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	nvgFontSize(vg, 15);
	nvgTextLetterSpacing(vg, 0.0f);
	
	ProfilerText(vg, 0, "FPS:", "%.0f", 1 / Profiler_Time(PROFILER_FPS), 0);
	ProfilerText(vg, 1, "Total:", "%.2fms", Profiler_Time(0xF0) * 1000.0f, 16.0f);
	ProfilerText(vg, 2, "Scene Draw:", "%.2fms", Profiler_Time(0) * 1000.f, 16.0f);
	ProfilerText(vg, 3, "Gizmo Update:", "%.2fms", Profiler_Time(2) * 1000.f, 16.0f);
	ProfilerText(vg, 4, "n64:", "%.2fms", Profiler_Time(8) * 1000.f, 16.0f);
	ProfilerText(vg, 5, "Delta:", "%.2f", gDeltaTime, 0);
}
