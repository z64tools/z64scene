#include "EnViewport.h"
#include <float.h>

#define INCBIN_PREFIX
#include <incbin.h>
INCBIN(gGizmo_, "assets/3D/Gizmo.zobj");
INCBIN(gSpot_, "assets/3D/Spot.zobj");

void EnViewport_Init(Editor* editor, EnViewport* this, Split* split);
void EnViewport_Destroy(Editor* editor, EnViewport* this, Split* split);
void EnViewport_Update(Editor* editor, EnViewport* this, Split* split);
void EnViewport_Draw(Editor* editor, EnViewport* this, Split* split);

SplitTask gEnViewportTask = DefineTask("Viewport", EnViewport);

// # # # # # # # # # # # # # # # # # # # #
// #                                     #
// # # # # # # # # # # # # # # # # # # # #

static void EnViewport_DrawSpot(Vec3f pos, f32 scale, NVGcolor color) {
	Matrix_Push();
	Matrix_Translate(UnfoldVec3(pos), MTXMODE_APPLY);
	
	Matrix_Push();
	Matrix_Scale(0.01 * scale, 0.01 * scale, 0.01 * scale, MTXMODE_APPLY);
	
	gSPSegment(POLY_OPA_DISP++, 6, (void*)gSpot_Data);
	gDPSetEnvColor(POLY_OPA_DISP++, UnfoldNVGcolor(color), color.a * 255);
	gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
	gSPDisplayList(POLY_OPA_DISP++, 0x06007730);
	
	Matrix_Pop();
	Matrix_Pop();
}

static void EnViewport_Gizmo(Editor* editor, EnViewport* this, Vec3f pos) {
	View3D* view = &this->view;
	Cylinder cyl[3];
	Vec3f mul[2] = {
		0, 100 * 100, 0,
		0, 240 * 100, 0,
	};
	
	for (s32 i = 0; i < 3; i++) {
		gSPSegment(POLY_OPA_DISP++, 6, (void*)gGizmo_Data);
		Matrix_Push(); {
			Matrix_Translate(UnfoldVec3(pos), MTXMODE_APPLY);
			Matrix_Push(); {
				NVGcolor color = nvgHSL(i / 3.0, 0.5, 0.5);
				f32 scale = Math_Vec3f_DistXYZ(pos, view->currentCamera->eye) * 0.00001f;
				Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
				
				if (i == 0)
					gDPSetEnvColor(POLY_OPA_DISP++, UnfoldNVGcolor(color), this->gfocus[i] * 0x80 + 0x7F);
				
				if (i == 1) {
					gDPSetEnvColor(POLY_OPA_DISP++, UnfoldNVGcolor(color), this->gfocus[i] * 0x80 + 0x7F);
					Matrix_RotateX_s(DegToBin(90), MTXMODE_APPLY);
				}
				if (i == 2) {
					gDPSetEnvColor(POLY_OPA_DISP++, UnfoldNVGcolor(color), this->gfocus[i] * 0x80 + 0x7F);
					Matrix_RotateZ_s(DegToBin(90), MTXMODE_APPLY);
				}
				
				Matrix_Push();
				Matrix_MultVec3f(&mul[0], &cyl[i].start);
				Matrix_MultVec3f(&mul[1], &cyl[i].end);
				cyl[i].r = Math_Vec3f_DistXYZ(pos, view->currentCamera->eye) * 0.02f;
				Matrix_Pop();
				
				gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
				gSPDisplayList(POLY_OPA_DISP++, 0x06000840);
			} Matrix_Pop();
		} Matrix_Pop();
	}
	
	for (s32 i = 0; i < 3; i++) {
		RayLine ray = this->rayLine;
		
		this->gfocus[i] = false;
		if (Col3D_LineVsCylinder(&ray, &cyl[i])) {
			printf_info("CYL HIT");
			this->gfocus[i] = true;
		}
	}
	
	for (s32 i = 0; i < 3; i++) {
		EnViewport_DrawSpot(cyl[i].start, cyl[i].r, Theme_GetColor(THEME_PRIM, 40 + 120 * this->gfocus[i], 1.0f));
		EnViewport_DrawSpot(cyl[i].end, cyl[i].r, Theme_GetColor(THEME_PRIM, 40 + 120 * this->gfocus[i], 1.25f));
	}
}

static Room* EnViewport_RayRooms(Scene* scene, RayLine* ray) {
	s32 id = -1;
	Vec3f r;
	
	for (s32 i = 0; i < scene->numRoom; i++) {
		RoomHeader* room = Scene_GetRoomHeader(scene, i);
		
		if (Col3D_LineVsTriBuffer(ray, &room->mesh->triBuf, &r, NULL))
			id = i;
	}
	
	if (id < 0)
		return NULL;
	
	return &scene->room[id];
}

static void EnViewport_CameraUpdate(Editor* editor, EnViewport* this, Split* split) {
	Input* inputCtx = &editor->input;
	MouseInput* mouse = &inputCtx->mouse;
	
	if (this->view.setCamMove == false) {
		this->view.cameraControl = false;
		
		if (split->blockMouse == false && editor->geo.state.noClickInput == false && split->mouseInSplit)
			this->view.cameraControl = true;
	}
	
	if (mouse->clickMid.press && Input_GetKey(inputCtx, KEY_LEFT_ALT)->hold) {
		Vec3f o;
		s32 r = 0;
		RayLine ray = this->rayLine;
		
		for (s32 i = 0; i < editor->scene.numRoom; i++) {
			RoomHeader* room = Scene_GetRoomHeader(&editor->scene, i);
			
			if (Col3D_LineVsTriBuffer(&ray, &room->mesh->triBuf, &o, NULL))
				r = true;
		}
		
		if (r)
			View_MoveTo(&this->view, o);
	}
	
	if (Input_GetKey(inputCtx, KEY_LEFT_CONTROL)->hold && mouse->clickL.press) {
		RayLine ray = this->rayLine;
		Room* room = EnViewport_RayRooms(&editor->scene, &ray);
		
		if (room)
			room->state ^= ROOM_SELECTED;
	}
	
	// Cursor Wrapping
	if (this->view.setCamMove && this->view.cameraControl) {
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

static void EnViewport_UpdateActors(Editor* editor, EnViewport* this, Split* split) {
	RayLine ray = this->rayLine;
	ActorList* list = (void*)editor->scene.dataCtx.head[SCENE_CMD_ID_ACTOR_LIST];
	Input* input = &editor->input;
	Actor* selectedActor = NULL;
	
	if (!Input_GetMouse(input, MOUSE_R)->press)
		return;
	
	this->curActor = NULL;
	
	while (list) {
		Actor* actor = list->head;
		for (s32 i = 0; i < list->num; i++, actor++) {
			veccpy(&actor->sph.pos, &actor->pos);
			actor->sph.pos.y += 10.0f;
			actor->sph.r = 30;
			
			if (Col3D_LineVsSphere(&ray, &actor->sph))
				selectedActor = actor;
		}
		
		list = (void*)list->data.next;
	}
	
	if (!Input_GetKey(input, KEY_LEFT_SHIFT)->hold) {
		list = (void*)editor->scene.dataCtx.head[SCENE_CMD_ID_ACTOR_LIST];
		
		while (list) {
			Actor* actor = list->head;
			for (s32 i = 0; i < list->num; i++, actor++) {
				actor->state &= ~ACTOR_SELECTED;
			}
			
			list = (void*)list->data.next;
		}
	}
	
	if (selectedActor) {
		this->curActor = selectedActor;
		selectedActor->state |= ACTOR_SELECTED;
	}
}

// # # # # # # # # # # # # # # # # # # # #
// #                                     #
// # # # # # # # # # # # # # # # # # # # #

void EnViewport_Init(Editor* editor, EnViewport* this, Split* split) {
	View_Init(&this->view, &editor->input);
	
	// MemFile_LoadFile(&gNora, "Nora.zobj");
	// SkelAnime_Init(&gNora, &this->skelAnime, 0x0600D978, 0x0600EF44);
}

void EnViewport_Destroy(Editor* editor, EnViewport* this, Split* split) {
	split->bg.useCustomBG = false;
}

void EnViewport_Update(Editor* editor, EnViewport* this, Split* split) {
	SceneHeader* header;
	EnvLightSettings* env;
	
	Element_Header(split, split->taskCombo, 128);
	Element_Combo(split->taskCombo);
	
	if (editor->scene.segment == NULL)
		return;
	
	header = Scene_GetSceneHeader(&editor->scene);
	env = header->lightList->env + editor->scene.curEnv;
	memcpy(split->bg.color.c, env->fogColor, 3);
	
	if (editor->scene.state & SCENE_DRAW_FOG)
		this->view.far = env->fogFar;
	else
		this->view.far = 12800.0 + 6000;
	
	EnViewport_CameraUpdate(editor, this, split);
}

static void EnViewport_Draw_Empty(Editor* editor, EnViewport* this, Split* split) {
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

static void EnViewport_Draw_3DViewport(Editor* editor, EnViewport* this, Split* split) {
	Vec2s dim = {
		split->rect.w,
		split->rect.h
	};
	
	split->bg.useCustomBG = true;
	
	n64_graph_init();
	n64_set_culling(editor->scene.state & SCENE_DRAW_CULLING);
	
	View_SetProjectionDimensions(&this->view, &dim);
	View_Update(&this->view, &editor->input);
	this->rayLine = View_Raycast(&this->view, split->mousePos, split->dispRect);
	EnViewport_UpdateActors(editor, this, split);
	
	n64_setMatrix_model(&this->view.modelMtx);
	n64_setMatrix_view(&this->view.viewMtx);
	n64_setMatrix_projection(&this->view.projMtx);
	
	Profiler_I(0);
	if (editor->scene.segment)
		Scene_Draw(&editor->scene);
	Profiler_O(0);
	
	if (this->curActor) {
		n64_reset_buffers();
		EnViewport_Gizmo(editor, this, Math_Vec3f_New(UnfoldVec3(this->curActor->pos)));
		n64_draw_buffers();
	}
	
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
	ProfilerText(vg, 2, "Col3D:", "%.2fms", Profiler_Time(1) * 1000.f, 16.0f);
	ProfilerText(vg, 3, "Delta:", "%.2f", gDeltaTime, 0);
}
