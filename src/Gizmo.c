#include <Editor.h>
#include <Gizmo.h>

extern DataFile gGizmo;
#include "../assets/3D/Gizmo.h"

static void Gizmo_Reset(Gizmo* this) {
    this->lock.state = GIZMO_AXIS_ALL_FALSE;
    this->pressLock = false;
    this->release = true;
    this->action = GIZMO_ACTION_NULL;
    this->typed[0] = 0;
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
        { 0, 120, 0 },
        { 0, 400, 0 },
    };
    
    if (!this->activeElem)
        return;
    
    for (s32 i = 0; i < 3; i++) {
        if (!this->lock.state) {
            f32 add = 0.2f * this->focus.axis[i];
            NVGcolor color = nvgHSL(1.0f - (i / 3.0f), 0.5f + add, 0.5f + add);
            f32 scale = Math_Vec3f_DistXYZ(this->pos, view->currentCamera->eye) * 0.0006f;
            
            gSPSegment((*disp)++, 6, (void*)gGizmo.data);
            Matrix_Push(); {
                Matrix_Translate(UnfoldVec3(this->pos), MTXMODE_NEW);
                Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
                Matrix_Mult(&this->mtx, MTXMODE_APPLY);
                
                if (i == 2)
                    Matrix_RotateX_d(90, MTXMODE_APPLY);
                
                if (i == 0)
                    Matrix_RotateZ_d(90, MTXMODE_APPLY);
                
                Matrix_MultVec3f(&sOffsetMul[0], &this->cyl[i].start);
                Matrix_MultVec3f(&sOffsetMul[1], &this->cyl[i].end);
                
                Vec3f dir = Math_Vec3f_LineSegDir(this->cyl[i].start, this->cyl[i].end);
                Vec3f frn = Math_Vec3f_LineSegDir(view->currentCamera->eye, view->currentCamera->at);
                f32 dot = invertf(fabsf(powf(Math_Vec3f_Dot(dir, frn), 3)));
                
                if (i == 1)
                    gDPSetEnvColor((*disp)++, UnfoldNVGcolor(color), 0xFF * dot);
                
                if (i == 2)
                    gDPSetEnvColor((*disp)++, UnfoldNVGcolor(color), 0xFF * dot);
                
                if (i == 0)
                    gDPSetEnvColor((*disp)++, UnfoldNVGcolor(color), 0xFF * dot);
                
                this->cyl[i].r = Math_Vec3f_DistXYZ(this->pos, view->currentCamera->eye) * 0.02f;
                
                gSPMatrix((*disp)++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
                gSPDisplayList((*disp)++, gGizmo_DlGizmo);
            } Matrix_Pop();
        }
    }
    
    if (this->lock.state && this->lock.state != GIZMO_AXIS_ALL_TRUE) {
        Editor* editor = GetEditor();
        void* vg = editor->vg;
        
        for (s32 i = 0; i < 3; i++) {
            if (!this->lock.axis[i])
                continue;
            
            Vec3f aI = Math_Vec3f_Add(this->pos, Math_Vec3f_MulVal(mxo[i], 1000000));
            Vec3f bI = Math_Vec3f_Add(this->pos, Math_Vec3f_MulVal(mxo[i], -1000000));
            Vec2f aO, bO;
            
            View_ClipPointIntoView(view, &aI, Math_Vec3f_Invert(mxo[i]));
            View_ClipPointIntoView(view, &bI, mxo[i]);
            aO = View_GetScreenPos(view, aI);
            bO = View_GetScreenPos(view, bI);
            
            nvgBeginPath(vg);
            nvgStrokeColor(vg, nvgHSLA(1.0f - (i / 3.0f), 0.5f, 0.5f, 255));
            nvgStrokeWidth(vg, 2.0f);
            nvgMoveTo(vg, UnfoldVec2(aO));
            nvgLineTo(vg, UnfoldVec2(bO));
            nvgStroke(vg);
        }
    }
}

static void Gizmo_Move(Gizmo* this, View3D* view, Input* input, Vec3f* rayPos) {
    bool ctrlHold = Input_GetKey(input, KEY_LEFT_CONTROL)->hold;
    Vec3f mxo[3] = {
        /* Right */ { this->mtx.xx, this->mtx.yx, this->mtx.zx },
        /* Up    */ { this->mtx.xy, this->mtx.yy, this->mtx.zy },
        /* Front */ { this->mtx.xz, this->mtx.yz, this->mtx.zz },
    };
    
    if (ctrlHold && this->lock.state == GIZMO_AXIS_ALL_TRUE) {
        _log("SnapTo");
        if (rayPos) {
            this->pos.x = rintf(rayPos->x);
            this->pos.y = rintf(rayPos->y);
            this->pos.z = rintf(rayPos->z);
        }
    } else {
        _log("Other");
        Vec2f gizmoScreenSpace = View_GetScreenPos(view, this->pos);
        Vec2f mv = Math_Vec2f_New(UnfoldVec2(input->cursor.vel));
        RayLine curRay = View_GetPointRayLine(view,  gizmoScreenSpace);
        RayLine nextRay = View_GetPointRayLine(view,  Math_Vec2f_Add(gizmoScreenSpace, mv));
        
        if (this->lock.state == GIZMO_AXIS_ALL_TRUE) {
            _log("View Move");
            Vec3f nextRayN = Math_Vec3f_LineSegDir(nextRay.start, nextRay.end);
            f32 nextDist = Math_Vec3f_DistXYZ(nextRay.start, nextRay.end);
            f32 curDist = Math_Vec3f_DistXYZ(curRay.start, curRay.end);
            f32 pointDist = Math_Vec3f_DistXYZ(curRay.start, this->pos);
            f32 distRatio = nextDist / curDist;
            
            this->pos = Math_Vec3f_Add(nextRay.start, Math_Vec3f_MulVal(nextRayN, pointDist * distRatio));
        } else {
            _log("Axis Move");
            
            if (ctrlHold && rayPos) {
                for (s32 i = 0; i < 3; i++) {
                    if (!this->lock.axis[i])
                        continue;
                    Vec3f off = Math_Vec3f_Project(Math_Vec3f_Sub(*rayPos, this->pos), mxo[i]);
                    
                    this->pos = Math_Vec3f_Add(this->pos, off);
                }
            } else {
                for (s32 i = 0; i < 3; i++) {
                    if (this->lock.axis[i]) {
                        this->pos = Math_Vec3f_ClosestPointOnRay(nextRay.start, nextRay.end, this->pos, Math_Vec3f_Add(this->pos, mxo[i]));
                        break;
                    }
                }
            }
        }
    }
    
    _log("Apply Pos");
    fornode(elem, this->elemHead) {
        Vec3f relPos = Math_Vec3f_Sub(this->pos, this->pivotPos);
        
        elem->action = true;
        elem->pos = Math_Vec3f_Add(*elem->dpos, relPos);
        
        for (var i  = 0; i < 3; i++)
            elem->pos.axis[i] = rint(elem->pos.axis[i]);
    }
    _log("!");
}

static void Gizmo_Rotate(Gizmo* this, View3D* view, Input* input, Vec3f* rayPos) {
    bool step = Input_GetKey(input, KEY_LEFT_CONTROL)->hold;
    Vec2f sp = View_GetScreenPos(view, this->pos);
    Vec2f mp = Math_Vec2f_New(UnfoldVec2(input->cursor.pos));
    s16 yaw = Math_Vec2f_Yaw(sp, mp);
    
    if (this->initAction) {
        this->prev_yaw = yaw;
        this->degr = 0;
    }
    
    if (this->typed[0]) {
        this->degr = DegToBin(wrapf(this->value, -180, 180));
    }
    
    this->degr += this->prev_yaw - yaw;
    
    MtxF orientation;
    int lock_axis = 0;
    
    f32 dgr = BinToDeg(this->degr);
    if (step) dgr = roundstepf(dgr, 15.0f);
    
    Matrix_Push();
    
    if (this->lock.state != GIZMO_AXIS_ALL_TRUE && this->lock.state & GIZMO_AXIS_ALL_TRUE) {
        while (!this->lock.axis[lock_axis]) lock_axis++;
        orientation = View_GetLockOrientedMtxF(view, dgr, lock_axis);
    } else
        orientation = View_GetOrientedMtxF(view, dgr);
    
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_NEW);
    Matrix_Translate(UnfoldVec3(this->pivotPos), MTXMODE_APPLY);
    Matrix_Mult(&orientation, MTXMODE_APPLY);
    
    fornode(elem, this->elemHead) {
        Vec3f relPos = Math_Vec3f_Sub(*elem->dpos, this->pivotPos);
        Vec3f zero = {};
        
        elem->action = true;
        
        Matrix_Push();
        
        Matrix_Translate(UnfoldVec3(relPos), MTXMODE_APPLY);
        Matrix_RotateY_s(elem->drot->y, MTXMODE_APPLY);
        Matrix_RotateX_s(elem->drot->x, MTXMODE_APPLY);
        Matrix_RotateZ_s(elem->drot->z, MTXMODE_APPLY);
        
        Matrix_MultVec3f(&zero, &elem->pos);
        Matrix_MtxFToYXZRotS(&elem->rot, 0);
        
        Matrix_Pop();
        
        for (var j  = 0; j < 3; j++)
            elem->pos.axis[j] = rint(elem->pos.axis[j]);
    }
    
    Matrix_Pop();
    
    this->prev_yaw = yaw;
}

static void Gizmo_UpdateTyped(Gizmo* this, Input* input) {
    if (input->buffer[0]) {
        char* buf = x_strdup(input->buffer);
        int len = strlen(buf);
        
        for (int i = 0; i < len; i++)
            if (!isdigit(buf[i]) && buf[i] != '.' && buf[i] != '-')
                strrem(buf, 1);
        strncat(this->typed, buf, 32);
        this->value = sfloat(this->typed);
    }
    
    if (Input_GetKey(input, KEY_ESCAPE)->press) {
        Gizmo_ResetTransforms(this);
        Gizmo_Reset(this);
    }
    
    if (Input_GetKey(input, KEY_ENTER)->press) {
        Gizmo_ApplyTransforms(this);
        Gizmo_Reset(this);
    }
}

void Gizmo_Update(Gizmo* this, View3D* view, Input* input, Vec3f* rayPos) {
    void (* gizmoActionFunc[])(Gizmo*, View3D*, Input*, Vec3f*) = {
        NULL,
        Gizmo_Move,
        Gizmo_Rotate,
    };
    bool alt = Input_GetKey(input, KEY_LEFT_ALT)->hold;
    u8 oneHit = 0;
    
    fornode(elem, this->elemHead) {
        elem->action = false;
    }
    
    if (!this->activeElem)
        return;
    
    if (this->undorepos) {
        fornode(elem, this->elemHead) {
            elem->pos = *elem->dpos;
            elem->rot = *elem->drot;
        }
        
        Gizmo_Focus(this, this->activeElem);
        this->undorepos = false;
    }
    
    // Reset for now, utilize for local space later
    Matrix_Clear(&this->mtx);
    Gizmo_UpdateTyped(this, input);
    
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
            this->initpos = this->pos;
            this->focus.state = GIZMO_AXIS_ALL_TRUE;
            this->initAction = true;
            oneHit = true;
            
            if (alt) {
                this->pos = Math_Vec3f_New(0, 0, 0);
            }
        }
        
        if (Input_GetKey(input, KEY_R)->press) {
            this->action = GIZMO_ACTION_ROTATE;
            this->pressLock = true;
            this->initpos = this->pos;
            this->focus.state = GIZMO_AXIS_ALL_TRUE;
            this->initAction = true;
            oneHit = true;
            
            if (alt) {
                fornode(elem, this->elemHead) {
                    elem->rot = Math_Vec3s_New(0, 0, 0);
                    *elem->drot = Math_Vec3s_New(0, 0, 0);
                }
            }
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
        if (Input_GetMouse(input, CLICK_ANY)->press || Input_GetKey(input, KEY_ESCAPE)->press) {
            
            if (Input_GetMouse(input, CLICK_ANY)->press)
                Gizmo_ApplyTransforms(this);
            else
                Gizmo_ResetTransforms(this);
            
            Gizmo_Reset(this);
            return;
        }
    } else {
        if (Input_GetMouse(input, CLICK_L)->hold == false || Input_GetKey(input, KEY_ESCAPE)->press) {
            if (Input_GetMouse(input, CLICK_ANY)->hold == false)
                Gizmo_ApplyTransforms(this);
            else
                Gizmo_ResetTransforms(this);
            
            Gizmo_Reset(this);
            return;
        }
    }
    
    _log("Gizmo Update: %d", this->action);
    if (gizmoActionFunc[this->action])
        gizmoActionFunc[this->action](this, view, input, rayPos);
    this->initAction = false;
    
    this->lock = this->focus;
    
    if (alt && oneHit) {
        Gizmo_ApplyTransforms(this);
        Gizmo_Reset(this);
    }
}

void Gizmo_Focus(Gizmo* this, GizmoElem* elem) {
    if (this->activeElem) this->activeElem->focus = false;
    this->activeElem = elem;
    this->activeElem->focus = true;
    this->pos = this->pivotPos = *elem->dpos;
}

void Gizmo_Select(Gizmo* this, GizmoElem* elem, Vec3f* pos, Vec3s* rot) {
    memset(elem, 0, sizeof(GizmoElem));
    Node_Add(this->elemHead, elem);
    
    elem->pos = *pos;
    elem->rot = *rot;
    elem->dpos = pos;
    elem->drot = rot;
    elem->selected = true;
}

void Gizmo_UnselectAll(Gizmo* this) {
    Gizmo_Reset(this);
    this->release = false;
    
    while (this->elemHead) {
        this->elemHead->selected = false;
        this->elemHead->focus = false;
        Node_Remove(this->elemHead, this->elemHead);
    }
    
    this->activeElem = NULL;
}

void Gizmo_Unselect(Gizmo* this, GizmoElem* elem) {
    elem->selected = false;
    elem->focus = false;
    Node_Remove(this->elemHead, elem);
    
    if (this->activeElem == elem)
        this->activeElem = this->elemHead;
    if (this->activeElem == NULL)
        Gizmo_UnselectAll(this);
}

void Gizmo_ApplyTransforms(Gizmo* this) {
    UndoEvent* undo = NULL;
    
    fornode(elem, this->elemHead) {
        if (!elem->selected)
            continue;
        
        if (!undo) {
            undo = Undo_New();
            Undo_Response(undo, &this->undorepos);
        }
        
        Undo_RegVar(undo, elem->dpos);
        Undo_RegVar(undo, elem->drot);
        
        *elem->dpos = elem->pos;
        *elem->drot = elem->rot;
    }
    
    this->pivotPos = this->pos;
}

void Gizmo_ResetTransforms(Gizmo* this) {
    fornode(elem, this->elemHead) {
        if (!elem->selected)
            continue;
        elem->pos = *elem->dpos;
        elem->rot = *elem->drot;
        elem->action = false;
    }
    
    this->pos = *this->activeElem->dpos;
}
