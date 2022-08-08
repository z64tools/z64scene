#include "Viewport.h"
#include <float.h>

#define INCBIN_PREFIX
#include <incbin.h>
INCBIN(gGizmo_, "assets/3D/Gizmo.zobj");
INCBIN(gSpot_, "assets/3D/Spot.zobj");

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
	
	gSPSegment(POLY_OPA_DISP++, 6, (void*)gSpot_Data);
	gDPSetEnvColor(POLY_OPA_DISP++, UnfoldNVGcolor(color), color.a * 255);
	gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
	gSPDisplayList(POLY_OPA_DISP++, 0x06007730);
	Matrix_Pop();
}

static Room* Viewport_RayRooms(Scene* scene, RayLine* ray, Vec3f* out) {
	s32 id = -1;
	
	for (s32 i = 0; i < scene->numRoom; i++) {
		RoomHeader* room = Scene_GetRoomHeader(scene, i);
		
		if (Col3D_LineVsTriBuffer(ray, &room->mesh->triBuf, out, NULL))
			id = i;
	}
	
	if (id < 0)
		return NULL;
	
	return &scene->room[id];
}

static void ResetGizmo(Viewport* this) {
	Gizmo* gizmo = &this->gizmo;
	
	gizmo->lock.state = GIZMO_AXIS_ALL_FALSE;
	gizmo->moveLock = false;
	gizmo->ppos = gizmo->pos;
}

static void Viewport_Gizmo(Editor* editor, Viewport* this, Split* split) {
	Vec3f up = { 0, 1, 0 };
	Vec3f right = { 1, 0, 0 };
	Vec3f front = { 0, 0, 1 };
	Gizmo* gizmo = &this->gizmo;
	View3D* view = &this->view;
	Cylinder cyl[3];
	Vec3f mul[2] = {
		0, 100 * 100, 0,
		0, 230 * 100, 0,
	};
	
	for (s32 i = 0; i < 3; i++) {
		if (!gizmo->lock.state) {
			gSPSegment(POLY_VOPA_DISP++, 6, (void*)gGizmo_Data);
			Matrix_Push(); {
				Matrix_Translate(UnfoldVec3(gizmo->pos), MTXMODE_APPLY);
				Matrix_Push(); {
					NVGcolor color = nvgHSL(i / 3.0, 0.5 + 0.2 * gizmo->focus.axis[i], 0.5 + 0.2 * gizmo->focus.axis[i]);
					f32 scale = Math_Vec3f_DistXYZ(gizmo->pos, view->currentCamera->eye) * 0.00001f;
					Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
					
					if (i == 1) {
						gDPSetEnvColor(POLY_VOPA_DISP++, UnfoldNVGcolor(color), 0xFF);
					}
					
					if (i == 2) {
						gDPSetEnvColor(POLY_VOPA_DISP++, UnfoldNVGcolor(color), 0xFF);
						Matrix_RotateX_d(90, MTXMODE_APPLY);
					}
					if (i == 0) {
						gDPSetEnvColor(POLY_VOPA_DISP++, UnfoldNVGcolor(color), 0xFF);
						Matrix_RotateZ_d(90, MTXMODE_APPLY);
					}
					
					Matrix_Push();
					Matrix_MultVec3f(&mul[0], &cyl[i].start);
					Matrix_MultVec3f(&mul[1], &cyl[i].end);
					cyl[i].r = Math_Vec3f_DistXYZ(gizmo->pos, view->currentCamera->eye) * 0.02f;
					Matrix_Pop();
					
					gSPMatrix(POLY_VOPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
					gSPDisplayList(POLY_VOPA_DISP++, 0x060006D0);
				} Matrix_Pop();
			} Matrix_Pop();
		}
	}
	
	if (gizmo->lock.state == 0) {
		u8 oneHit = 0;
		RayLine ray = View_GetCursorRayLine(&this->view);
		
		for (s32 i = 0; i < 3; i++) {
			Vec3f p;
			gizmo->focus.axis[i] = false;
			
			if (Col3D_LineVsCylinder(&ray, &cyl[i], &p)) {
				gizmo->focus.axis[i] = true;
				oneHit = true;
			}
		}
		
		if (Input_GetKey(&editor->input, KEY_G)->press) {
			gizmo->moveLock = true;
			gizmo->focus.state = GIZMO_AXIS_ALL_TRUE;
			oneHit = true;
		}
		
		if (!oneHit)
			return;
		
		if (!gizmo->moveLock && Input_GetMouse(&editor->input, MOUSE_L)->press == false)
			return;
	} else {
		if (Input_GetKey(&editor->input, KEY_X)->press) {
			if (gizmo->focus.y || gizmo->focus.z) {
				gizmo->focus.state = GIZMO_AXIS_ALL_FALSE;
				gizmo->focus.x = true;
			} else
				gizmo->focus.state = GIZMO_AXIS_ALL_TRUE;
		}
		if (Input_GetKey(&editor->input, KEY_Y)->press) {
			if (gizmo->focus.x || gizmo->focus.z) {
				gizmo->focus.state = GIZMO_AXIS_ALL_FALSE;
				gizmo->focus.y = true;
			} else
				gizmo->focus.state = GIZMO_AXIS_ALL_TRUE;
		}
		if (Input_GetKey(&editor->input, KEY_Z)->press) {
			if (gizmo->focus.x || gizmo->focus.y) {
				gizmo->focus.state = GIZMO_AXIS_ALL_FALSE;
				gizmo->focus.z = true;
			} else
				gizmo->focus.state = GIZMO_AXIS_ALL_TRUE;
		}
	}
	
	if (gizmo->moveLock) {
		if (Input_GetMouse(&editor->input, MOUSE_ANY)->press) {
			ResetGizmo(this);
			
			return;
		}
	} else {
		if (Input_GetMouse(&editor->input, MOUSE_L)->hold == false) {
			ResetGizmo(this);
			
			return;
		}
	}
	
	gizmo->lock = gizmo->focus;
	
	if (Input_GetKey(&editor->input, KEY_LEFT_CONTROL)->hold) {
		RayLine r = View_GetCursorRayLine(&this->view);
		Vec3f p;
		
		if (Viewport_RayRooms(&editor->scene, &r, &p)) {
			gizmo->pos.x = rintf(p.x);
			gizmo->pos.y = rintf(p.y);
			gizmo->pos.z = rintf(p.z);
		}
	} else {
		MouseInput* m = &editor->input.mouse;
		f32 dist = View_DepthFactor(view, &gizmo->pos) / View_DimFactor(view) * 1.65;
		Vec3f mvel = Math_Vec3f_New(-m->vel.x, -m->vel.y, 0);
		mvel = View_OrientDirToView(view, &mvel);
		
		if (gizmo->lock.state == GIZMO_AXIS_ALL_TRUE) {
			this->gizmo.pos = Math_Vec3f_Add(this->gizmo.pos, Math_Vec3f_MulVal(mvel, dist));
		} else {
			if (gizmo->lock.x) {
				mvel = Math_Vec3f_Project(mvel, right);
				this->gizmo.pos = Math_Vec3f_Add(this->gizmo.pos, Math_Vec3f_MulVal(mvel, dist));
			}
			
			if (gizmo->lock.y) {
				mvel = Math_Vec3f_Project(mvel, up);
				this->gizmo.pos = Math_Vec3f_Add(this->gizmo.pos, Math_Vec3f_MulVal(mvel, dist));
			}
			
			if (gizmo->lock.z) {
				mvel = Math_Vec3f_Project(mvel, front);
				this->gizmo.pos = Math_Vec3f_Add(this->gizmo.pos, Math_Vec3f_MulVal(mvel, dist));
			}
		}
	}
	
	gizmo->vel = Math_Vec3f_Sub(gizmo->pos, gizmo->ppos);
	
	if (gizmo->vel.y || gizmo->vel.x || gizmo->vel.z) {
		ActorList* head = (void*)editor->scene.dataCtx.head[SCENE_CMD_ID_ACTOR_LIST];
		
		while (head) {
			Actor* actor = head->head;
			
			for (s32 i = 0; i < head->num; i++, actor++) {
				if (!(actor->state & ACTOR_SELECTED))
					continue;
				
				actor->pos.x += gizmo->vel.x;
				actor->pos.y += gizmo->vel.y;
				actor->pos.z += gizmo->vel.z;
			}
			
			head = (void*)head->data.next;
		}
	}
	
	gizmo->ppos = gizmo->pos;
	
	if (gizmo->lock.state && gizmo->lock.state != GIZMO_AXIS_ALL_TRUE) {
		void* vg = editor->vg;
		Rect r = split->rect;
		
		r.x = r.y = 16;
		r.w -= 32;
		r.h -= 32;
		
		if (gizmo->lock.x) {
			Vec3f a = Math_Vec3f_Add(gizmo->pos, Math_Vec3f_MulVal(right, 100.0f));
			Vec3f b = Math_Vec3f_Add(gizmo->pos, Math_Vec3f_MulVal(right, -100.0f));
			Vec2f a2d = View_Vec3fToScreenSpace(view, &a);
			Vec2f b2d = View_Vec3fToScreenSpace(view, &b);
			
			nvgBeginPath(vg);
			nvgStrokeColor(vg, nvgHSLA(0.000, 0.5, 0.5, 120));
			nvgStrokeWidth(vg, 1.0f);
			nvgMoveTo(vg, UnfoldVec2(a2d));
			nvgLineTo(vg, UnfoldVec2(b2d));
			nvgStroke(vg);
			printf_info("%.0f %.0f", a.x, a.y);
		}
		
		if (gizmo->lock.y) {
			Vec3f a = Math_Vec3f_Add(gizmo->pos, Math_Vec3f_MulVal(up, 100.0f));
			Vec3f b = Math_Vec3f_Add(gizmo->pos, Math_Vec3f_MulVal(up, -100.0f));
			Vec2f a2d = View_Vec3fToScreenSpace(view, &a);
			Vec2f b2d = View_Vec3fToScreenSpace(view, &b);
			
			nvgBeginPath(vg);
			nvgStrokeColor(vg, nvgHSLA(0.333, 0.5, 0.5, 120));
			nvgStrokeWidth(vg, 1.0f);
			nvgMoveTo(vg, UnfoldVec2(a2d));
			nvgLineTo(vg, UnfoldVec2(b2d));
			nvgStroke(vg);
			printf_info("%.0f %.0f", a.x, a.y);
		}
		
		if (gizmo->lock.z) {
			Vec3f a = Math_Vec3f_Add(gizmo->pos, Math_Vec3f_MulVal(front, 100.0f));
			Vec3f b = Math_Vec3f_Add(gizmo->pos, Math_Vec3f_MulVal(front, -100.0f));
			Vec2f a2d = View_Vec3fToScreenSpace(view, &a);
			Vec2f b2d = View_Vec3fToScreenSpace(view, &b);
			
			nvgBeginPath(vg);
			nvgStrokeColor(vg, nvgHSLA(0.666, 0.5, 0.5, 120));
			nvgStrokeWidth(vg, 1.0f);
			nvgMoveTo(vg, UnfoldVec2(a2d));
			nvgLineTo(vg, UnfoldVec2(b2d));
			nvgStroke(vg);
			printf_info("%.0f %.0f", a.x, a.y);
		}
	}
}

static void Viewport_CameraUpdate(Editor* editor, Viewport* this, Split* split) {
	Input* inputCtx = &editor->input;
	MouseInput* mouse = &inputCtx->mouse;
	Gizmo* gizmo = &this->gizmo;
	View3D* view = &this->view;
	s32 cond = split->blockMouse == false && editor->geo.state.noClickInput == false && split->mouseInSplit;
	
	if (!Input_GetMouse(inputCtx, MOUSE_ANY)->hold && !cond)
		view->cameraControl = false;
	
	if (!view->cameraControl) {
		if (cond) {
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
			Room* room = Viewport_RayRooms(&editor->scene, &ray, NULL);
			
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
	Gizmo* gizmo = &this->gizmo;
	Vec3f p;
	
	if (gizmo->lock.state || !Input_GetMouse(input, MOUSE_L)->press)
		return;
	
	// Process rooms so that we do not grab actors behind walls
	if (Viewport_RayRooms(&editor->scene, &ray, NULL))
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
		this->gizmo.ppos = this->gizmo.pos = Math_Vec3f_New(UnfoldVec3(this->curActor->pos));
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

static void Viewport_InitDraw(Editor* editor, Viewport* this, Split* split) {
	split->bg.useCustomBG = true;
	n64_graph_init();
	n64_set_culling(editor->scene.state & SCENE_DRAW_CULLING);
	
	View_Update(&this->view, &editor->input, split);
	
	n64_setMatrix_model(&this->view.modelMtx);
	n64_setMatrix_view(&this->view.viewMtx);
	n64_setMatrix_projection(&this->view.projMtx);
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
	
	Profiler_I(0);
	
	gVOPADisp = gVOPAHead;
	gVXLUDisp = gVXLUHead;
	Assert(POLY_VXLU_DISP && POLY_VOPA_DISP);
	if (editor->scene.segment)
		Scene_Draw(&editor->scene);
	
	Log("Draw/Update: Gizmo & Actor");
	if (this->curActor)
		Viewport_Gizmo(editor, this, split);
	Viewport_UpdateActors(editor, this, split);
	
	for (s32 i = 0; i < scene->numRoom; i++)
		Actor_Draw_RoomHeader(Scene_GetRoomHeader(scene, i));
	
	n64_draw_buffers();
	
	Profiler_O(0);
	
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
	
	Profiler_O(4);
	Profiler_I(4);
	
	if (editor->scene.segment == NULL)
		Viewport_DrawInfo(editor, this, split);
	else
		Viewport_DrawViewport(editor, this, split);
	
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	nvgFontSize(vg, 15);
	nvgTextLetterSpacing(vg, 0.0f);
	
	ProfilerText(vg, 0, "FPS:", "%.0f", 1 / Profiler_Time(4), 0);
	ProfilerText(vg, 1, "N64 Render:", "%.2fms", Profiler_Time(0) * 1000.f, 16.0f);
	ProfilerText(vg, 2, "Col3D:", "%.2fms", Profiler_Time(1) * 1000.f, 16.0f);
	ProfilerText(vg, 3, "Delta:", "%.2f", gDeltaTime, 0);
}
