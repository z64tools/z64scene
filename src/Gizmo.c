#include <Editor.h>
#include <Gizmo.h>

extern DataFile gGizmo;

#define gGizmo_GizmoDL 0x060006D0
#define gGizmo_LineDL  0x06000C60

static void Gizmo_Reset(Gizmo* this) {
	this->lock.state = GIZMO_AXIS_ALL_FALSE;
	this->pressLock = false;
	this->ppos = this->pos;
	this->release = true;
	this->action = GIZMO_ACTION_NULL;
}

bool Gizmo_IsBusy(Gizmo* this) {
	if (this->release || this->lock.state)
		return true;
	
	return false;
}

void Gizmo_Draw(Gizmo* this, View3D* view, Gfx** disp) {
	Vec3f mxo[3] = {
		/* MXO_R */ { this->mtx.xx, this->mtx.yx, this->mtx.zx },
		/* MXO_U */ { this->mtx.xy, this->mtx.yy, this->mtx.zy },
		/* MXO_F */ { this->mtx.xz, this->mtx.yz, this->mtx.zz },
	};
	static Vec3f sOffsetMul[] = {
		{ 0, 100 * 100, 0 },
		{ 0, 230 * 100, 0 },
	};
	
	for (s32 i = 0; i < 3; i++) {
		if (!this->lock.state) {
			NVGcolor color = nvgHSL(i / 3.0, 0.5 + 0.2 * this->focus.axis[i], 0.5 + 0.2 * this->focus.axis[i]);
			f32 scale = Math_Vec3f_DistXYZ(this->pos, view->currentCamera->eye) * 0.00001f;
			
			gSPSegment((*disp)++, 6, (void*)gGizmo.data);
			Matrix_Push(); {
				Matrix_Translate(UnfoldVec3(this->pos), MTXMODE_NEW);
				Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
				Matrix_Mult(&this->mtx, MTXMODE_APPLY);
				
				if (i == 1) {
					gDPSetEnvColor((*disp)++, UnfoldNVGcolor(color), 0xFF);
				}
				
				if (i == 2) {
					gDPSetEnvColor((*disp)++, UnfoldNVGcolor(color), 0xFF);
					Matrix_RotateX_d(90, MTXMODE_APPLY);
				}
				if (i == 0) {
					gDPSetEnvColor((*disp)++, UnfoldNVGcolor(color), 0xFF);
					Matrix_RotateZ_d(90, MTXMODE_APPLY);
				}
				
				Matrix_MultVec3f(&sOffsetMul[0], &this->cyl[i].start);
				Matrix_MultVec3f(&sOffsetMul[1], &this->cyl[i].end);
				this->cyl[i].r = Math_Vec3f_DistXYZ(this->pos, view->currentCamera->eye) * 0.02f;
				
				gSPMatrix((*disp)++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
				gSPDisplayList((*disp)++, gGizmo_GizmoDL);
			} Matrix_Pop();
		}
	}
	
	if (this->lock.state && this->lock.state != GIZMO_AXIS_ALL_TRUE) {
		Editor* editor = GetEditor();
		void* vg = editor->vg;
		
		for (s32 i = 0; i < 3; i++) {
			if (!this->lock.axis[i])
				continue;
			
			Vec3f aI = Math_Vec3f_Add(this->pos, Math_Vec3f_MulVal(mxo[i], 10000));
			Vec3f bI = Math_Vec3f_Add(this->pos, Math_Vec3f_MulVal(mxo[i], -10000));
			Vec2f aO, bO;
			
			View_ClipPointIntoView(view, &aI, Math_Vec3f_Invert(mxo[i]));
			View_ClipPointIntoView(view, &bI, mxo[i]);
			aO = View_GetScreenPos(view, aI);
			bO = View_GetScreenPos(view, bI);
			
			nvgBeginPath(vg);
			nvgStrokeColor(vg, nvgHSLA(i / 3.0, 0.5, 0.5, 255));
			nvgStrokeWidth(vg, 2.0f);
			nvgMoveTo(vg, UnfoldVec2(aO));
			nvgLineTo(vg, UnfoldVec2(bO));
			nvgStroke(vg);
		}
	}
}

static void Gizmo_Move(Gizmo* this, Scene* scene, View3D* view, Input* input) {
	bool ctrlHold = Input_GetKey(input, KEY_LEFT_CONTROL)->hold;
	Vec3f mxo[3] = {
		/* Right */ { this->mtx.xx, this->mtx.yx, this->mtx.zx },
		/* Up    */ { this->mtx.xy, this->mtx.yy, this->mtx.zy },
		/* Front */ { this->mtx.xz, this->mtx.yz, this->mtx.zz },
	};
	
	if (ctrlHold && this->lock.state == GIZMO_AXIS_ALL_TRUE) {
		Log("SnapTo");
		RayLine r = View_GetCursorRayLine(view);
		Vec3f p;
		
		if (Room_Raycast(scene, &r, &p)) {
			this->pos.x = rintf(p.x);
			this->pos.y = rintf(p.y);
			this->pos.z = rintf(p.z);
		}
	} else {
		Log("Other");
		Vec2f gizmoScreenSpace = View_GetScreenPos(view, this->pos);
		Vec2f mv = Math_Vec2f_New(UnfoldVec2(input->cursor.vel));
		RayLine curRay = View_GetPointRayLine(view,  gizmoScreenSpace);
		RayLine nextRay = View_GetPointRayLine(view,  Math_Vec2f_Add(gizmoScreenSpace, mv));
		RayLine mouseRay = View_GetCursorRayLine(view);
		
		if (this->lock.state == GIZMO_AXIS_ALL_TRUE) {
			Log("View Move");
			Vec3f nextRayN = Math_Vec3f_LineSegDir(nextRay.start, nextRay.end);
			f32 nextDist = Math_Vec3f_DistXYZ(nextRay.start, nextRay.end);
			f32 curDist = Math_Vec3f_DistXYZ(curRay.start, curRay.end);
			f32 pointDist = Math_Vec3f_DistXYZ(curRay.start, this->pos);
			f32 distRatio = nextDist / curDist;
			
			this->pos = Math_Vec3f_Add(nextRay.start, Math_Vec3f_MulVal(nextRayN, pointDist * distRatio));
		} else {
			Log("Axis Move");
			Vec3f p;
			
			if (ctrlHold && Room_Raycast(scene, &mouseRay, &p)) {
				for (s32 i = 0; i < 3; i++) {
					if (!this->lock.axis[i])
						continue;
					Vec3f off = Math_Vec3f_Project(Math_Vec3f_Sub(p, this->pos), mxo[i]);
					
					this->pos = Math_Vec3f_Add(this->pos, off);
				}
			} else {
				for (s32 i = 0; i < 3; i++) {
					if (this->lock.axis[i]) {
						this->pos = Math_Vec3f_ClosestPointOnRay(
							nextRay.start,
							nextRay.end,
							this->pos,
							Math_Vec3f_Add(this->pos, mxo[i])
						);
						break;
					}
				}
			}
		}
	}
	
	this->vel = Math_Vec3f_Sub(this->pos, this->ppos);
	
	if (this->vel.y || this->vel.x || this->vel.z) {
		ActorList* head = (void*)scene->dataCtx.head[SCENE_CMD_ID_ACTOR_LIST];
		
		while (head) {
			Actor* actor = head->head;
			
			for (s32 i = 0; i < head->num; i++, actor++) {
				if (!(actor->state & ACTOR_SELECTED))
					continue;
				
				actor->pos.x += this->vel.x;
				actor->pos.y += this->vel.y;
				actor->pos.z += this->vel.z;
			}
			
			head = (void*)head->data.next;
		}
	}
	
	this->ppos = this->pos;
}

static void Gizmo_Rotate(Gizmo* this, Scene* scene, View3D* view, Input* input) {
	bool step = Input_GetKey(input, KEY_LEFT_CONTROL)->hold;
	Vec2f sp = View_GetScreenPos(view, this->pos);
	Vec2f mp = Math_Vec2f_New(UnfoldVec2(input->cursor.pos));
	s16 yaw = Math_Vec2f_Yaw(sp, mp);
	s32 new = yaw - this->pyaw;
	
	this->degr += BinToDeg(new);
	this->degr = WrapF(this->degr, -180.f, 180.f);
	
	if (this->lock.state == GIZMO_AXIS_ALL_TRUE) {
	} else {
		for (s32 i = 0; i < 3; i++) {
			if (this->lock.axis[i]) {
				ActorList* head = (void*)scene->dataCtx.head[SCENE_CMD_ID_ACTOR_LIST];
				
				while (head) {
					Actor* actor = head->head;
					
					for (s32 j = 0; j < head->num; j++, actor++) {
						if (!(actor->state & ACTOR_SELECTED))
							continue;
						
						f64 dg = this->degr;
						
						if (step)
							dg = rint(dg * 0.1) * 10.0;
						
						actor->orot.axis[i] = DegToBin(dg);
					}
					
					head = (void*)head->data.next;
				}
			}
		}
	}
	
	this->pyaw = yaw;
}

void Gizmo_Update(Gizmo* this, Scene* scene, View3D* view, Input* input) {
	void (* gizmoActionFunc[])(Gizmo*, Scene*, View3D*, Input*) = {
		NULL,
		Gizmo_Move,
		Gizmo_Rotate,
	};
	bool alt = Input_GetKey(input, KEY_LEFT_ALT)->hold;
	u8 oneHit = 0;
	
	// Reset for now, utilize for local space later
	Matrix_Clear(&this->mtx);
	
	this->release = false;
	this->resetRot = false;
	
	if (this->lock.state == 0) {
		RayLine ray = View_GetCursorRayLine(view);
		
		for (s32 i = 0; i < 3; i++) {
			Vec3f p;
			this->focus.axis[i] = false;
			
			if (Col3D_LineVsCylinder(&ray, &this->cyl[i], &p)) {
				this->focus.axis[i] = true;
				oneHit = true;
				this->action = GIZMO_ACTION_MOVE;
			}
		}
		
		if (Input_GetKey(input, KEY_G)->press) {
			this->action = GIZMO_ACTION_MOVE;
			this->pressLock = true;
			this->ppos = this->initpos = this->pos;
			this->focus.state = GIZMO_AXIS_ALL_TRUE;
			oneHit = true;
			
			if (alt)
				this->pos = Math_Vec3f_New(0, 0, 0);
		}
		
		if (Input_GetKey(input, KEY_R)->press) {
			Vec2f sp = View_GetScreenPos(view, this->pos);
			Vec2f mp = Math_Vec2f_New(UnfoldVec2(input->cursor.pos));
			s16 yaw = Math_Vec2f_Yaw(sp, mp);
			
			this->action = GIZMO_ACTION_ROTATE;
			this->pressLock = true;
			this->ppos = this->initpos = this->pos;
			this->pyaw = yaw;
			this->degr = 0;
			this->focus.state = GIZMO_AXIS_ALL_TRUE;
			oneHit = true;
			
			if (alt)
				this->resetRot = true;
		}
		
		if (Input_GetKey(input, KEY_KP_0)->press)
			View_MoveTo(view, this->pos);
		if (!this->pressLock && Input_GetMouse(input, CLICK_L)->press == false)
			return;
		if (!oneHit)
			return;
		
	} else {
		if (Input_GetKey(input, KEY_X)->press) {
			if (this->focus.y || this->focus.z) {
				this->focus.state = GIZMO_AXIS_ALL_FALSE;
				this->focus.x = true;
				this->pos.y = this->initpos.y;
				this->pos.z = this->initpos.z;
			} else
				this->focus.state = GIZMO_AXIS_ALL_TRUE;
		}
		if (Input_GetKey(input, KEY_Z)->press) {
			if (this->focus.x || this->focus.z) {
				this->focus.state = GIZMO_AXIS_ALL_FALSE;
				this->focus.y = true;
				this->pos.x = this->initpos.x;
				this->pos.z = this->initpos.z;
			} else
				this->focus.state = GIZMO_AXIS_ALL_TRUE;
		}
		if (Input_GetKey(input, KEY_Y)->press) {
			if (this->focus.x || this->focus.y) {
				this->focus.state = GIZMO_AXIS_ALL_FALSE;
				this->focus.z = true;
				this->pos.x = this->initpos.x;
				this->pos.y = this->initpos.y;
			} else
				this->focus.state = GIZMO_AXIS_ALL_TRUE;
		}
	}
	
	if (this->pressLock) {
		if (Input_GetMouse(input, CLICK_ANY)->press)
			goto reset_gizmo;
	} else {
		if (Input_GetMouse(input, CLICK_L)->hold == false)
			goto reset_gizmo;
	}
	
	Log("Gizmo Update: %d", this->action);
	if (gizmoActionFunc[this->action])
		gizmoActionFunc[this->action](this, scene, view, input);
	
	this->lock = this->focus;
	
	if (alt && oneHit) {
reset_gizmo:
		Gizmo_Reset(this);
	}
}
