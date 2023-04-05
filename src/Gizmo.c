#include <Editor.h>
#include <Gizmo.h>

extern DataFile gGizmo;
#include "../assets/3D/Gizmo.h"

static void Gizmo_Reset(Gizmo* this) {
    info("Gizmo Reset");
    this->lock.state = GIZMO_AXIS_ALL_FALSE;
    this->release = true;
    this->action = 0;
    this->activeView = NULL;
    this->typed[0] = 0;
    this->typing = false;
}

static void Gizmo_UpdateMtx(Gizmo* this, View3D* view) {
    switch (this->orientation) {
        case GIZMO_ORIENTATION_GLOBAL:
            Matrix_Clear(&this->mtx);
            break;
            
        case GIZMO_ORIENTATION_LOCAL:
            Matrix_Push();
            Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_NEW);
            Matrix_RotateY_s(this->activeElem->drot->y, MTXMODE_APPLY);
            Matrix_RotateX_s(this->activeElem->drot->x, MTXMODE_APPLY);
            Matrix_RotateZ_s(this->activeElem->drot->z, MTXMODE_APPLY);
            Matrix_Get(&this->mtx);
            Matrix_Pop();
            break;
            
        case GIZMO_ORIENTATION_VIEW:
            break;
    }
    
}

static void Gizmo_Move(Gizmo* this, Vec3f* rayPos) {
    View3D* view = this->curView;
    Input* input = this->input;
    bool ctrlHold = Input_GetKey(input, KEY_LEFT_CONTROL)->hold;
    Vec3f mxo[3] = {
        /* Right */ { this->mtx.xx, this->mtx.yx, this->mtx.zx },
        /* Up    */ { this->mtx.xy, this->mtx.yy, this->mtx.zy },
        /* Front */ { this->mtx.xz, this->mtx.yz, this->mtx.zz },
    };
    
    if (this->typing && this->lock.state != GIZMO_AXIS_ALL_TRUE) {
        this->pos.x = this->pivotPos.x + this->value * this->lock.x;
        this->pos.y = this->pivotPos.y + this->value * this->lock.y;
        this->pos.z = this->pivotPos.z + this->value * this->lock.z;
        
    } else if (ctrlHold && this->lock.state == GIZMO_AXIS_ALL_TRUE) {
        if (rayPos) {
            this->pos.x = rintf(rayPos->x);
            this->pos.y = rintf(rayPos->y);
            this->pos.z = rintf(rayPos->z);
        }
        
    } else {
        Vec2f gizmoScreenSpace = View_GetScreenPos(view, this->pos);
        Vec2f mv = Math_Vec2f_New(UnfoldVec2(input->cursor.vel));
        RayLine curRay = View_GetRayLine(view,  gizmoScreenSpace);
        RayLine nextRay = View_GetRayLine(view,  Math_Vec2f_Add(gizmoScreenSpace, mv));
        
        if (this->lock.state == GIZMO_AXIS_ALL_TRUE) {
            Vec3f nextRayN = Math_Vec3f_LineSegDir(nextRay.start, nextRay.end);
            f32 nextDist = Math_Vec3f_DistXYZ(nextRay.start, nextRay.end);
            f32 curDist = Math_Vec3f_DistXYZ(curRay.start, curRay.end);
            f32 pointDist = Math_Vec3f_DistXYZ(curRay.start, this->pos);
            f32 distRatio = nextDist / curDist;
            
            this->pos = Math_Vec3f_Add(nextRay.start, Math_Vec3f_MulVal(nextRayN, pointDist * distRatio));
        } else {
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
    
    fornode(elem, this->elemHead) {
        Vec3f relPos = Math_Vec3f_Sub(this->pos, this->pivotPos);
        
        elem->interact = true;
        elem->pos = Math_Vec3f_Add(*elem->dpos, relPos);
        
        for (var i  = 0; i < 3; i++)
            elem->pos.axis[i] = rint(elem->pos.axis[i]);
    }
}

static void Gizmo_Rotate(Gizmo* this, Vec3f* rayPos) {
    View3D* view = this->curView;
    Input* input = this->input;
    bool step = Input_GetKey(input, KEY_LEFT_CONTROL)->hold;
    Vec2f sp = View_GetScreenPos(view, this->pos);
    Vec2f mp = Math_Vec2f_New(UnfoldVec2(input->cursor.pos));
    s16 yaw = Math_Vec2f_Yaw(sp, mp);
    
    if (this->initAction) {
        this->prev_yaw = yaw;
        this->rot.x = 0;
        this->rot.y = 0;
        this->rot.z = 0;
    }
    
    this->rot.x += (int)(DegToBin(input->cursor.vel.x) * 0.75f);
    this->rot.y -= (int)(DegToBin(input->cursor.vel.y) * 0.75f);
    this->rot.z += this->prev_yaw - yaw;
    
    f32 x = BinToDeg(this->rot.x);
    f32 y = BinToDeg(this->rot.y);
    f32 z = BinToDeg(this->rot.z);
    
    if (this->typing) z = this->value;
    
    if (step) {
        x = roundstepf(x, 15.0f);
        y = roundstepf(y, 15.0f);
        z = roundstepf(z, 15.0f);
    }
    
    if (!this->typing) this->value = z;
    
    if (this->trackball) {
        z = 0;
        this->lock.state = GIZMO_AXIS_ALL_TRUE;
    } else
        x = y = 0;
    
    Matrix_Push();
    
    MtxF orientation;
    int lock_axis = 0;
    
    if (this->lock.state != GIZMO_AXIS_ALL_TRUE && this->lock.state & GIZMO_AXIS_ALL_TRUE) {
        while (!this->lock.axis[lock_axis]) lock_axis++;
        
        if (this->orientation == GIZMO_ORIENTATION_LOCAL) {
            Matrix_Push();
            Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_NEW);
            
            Matrix_RotateY_s(this->activeElem->drot->y, MTXMODE_APPLY);
            Matrix_RotateX_s(this->activeElem->drot->x, MTXMODE_APPLY);
            Matrix_RotateZ_s(this->activeElem->drot->z, MTXMODE_APPLY);
            
            Vec3f axisDir[3] = {
                { 1.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f },
            };
            Vec3f zero = {};
            Vec3f look;
            void (*mtxRot[])(f32 val, MtxMode mode) = {
                Matrix_RotateX_d, Matrix_RotateY_d, Matrix_RotateZ_d
            };
            
            Matrix_MultVec3f(&axisDir[lock_axis], &look);
            
            Vec3f camDir = Math_Vec3f_LineSegDir(view->currentCamera->eye, view->currentCamera->at);
            Vec3f dir = Math_Vec3f_LineSegDir(zero, look);
            f32 dgr = z;
            
            if (!this->typing)
                if (Math_Vec3f_Dot(dir, camDir) <= 0.0f)
                    dgr = -dgr;
            
            mtxRot[lock_axis](dgr, MTXMODE_APPLY);
            
            Matrix_RotateZ_s(-this->activeElem->drot->z, MTXMODE_APPLY);
            Matrix_RotateX_s(-this->activeElem->drot->x, MTXMODE_APPLY);
            Matrix_RotateY_s(-this->activeElem->drot->y, MTXMODE_APPLY);
            
            Matrix_Get(&orientation);
            Matrix_Pop();
        } else
            orientation = View_GetLockOrientedMtxF(view, z, lock_axis, !this->typing);
        
    } else
        orientation = View_GetOrientedMtxF(view, x, y, z);
    
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_NEW);
    Matrix_Translate(UnfoldVec3(this->pivotPos), MTXMODE_APPLY);
    
    Matrix_Mult(&orientation, MTXMODE_APPLY);
    
    fornode(elem, this->elemHead) {
        Vec3f relPos = Math_Vec3f_Sub(*elem->dpos, this->pivotPos);
        Vec3f zero = {};
        
        elem->interact = true;
        
        Matrix_Push();
        
        Matrix_Translate(UnfoldVec3(relPos), MTXMODE_APPLY);
        Matrix_RotateY_s(elem->drot->y, MTXMODE_APPLY);
        Matrix_RotateX_s(elem->drot->x, MTXMODE_APPLY);
        Matrix_RotateZ_s(elem->drot->z, MTXMODE_APPLY);
        
        Matrix_MultVec3f(&zero, &elem->pos);
        Matrix_MtxFToYXZRotS(&elem->rot, true);
        
        Matrix_Pop();
        
        for (var j  = 0; j < 3; j++)
            elem->pos.axis[j] = rint(elem->pos.axis[j]);
    }
    
    Matrix_Pop();
    
    this->prev_yaw = yaw;
}

static void Gizmo_UpdateTyped(Gizmo* this, Input* input) {
    if (Input_GetKey(input, KEY_BACKSPACE)->press) {
        if (this->typed[0]) {
            this->typed[strlen(this->typed) - 1] = 0;
            this->value = sfloat(this->typed);
        } else
            this->typing = false;
    } else if (input->buffer[0]) {
        char* buf = x_strdup(input->buffer);
        int len = strlen(buf);
        
        for (int i = 0; i < len; i++)
            if (!isdigit(buf[i]) && buf[i] != '.' && buf[i] != '-')
                strrem(buf, 1);
        strncat(this->typed, buf, 12);
        this->typed[12] = 0;
        this->value = sfloat(this->typed);
        
        if (this->typed[0])
            this->typing = true;
    }
}

static void Gizmo_SetAction(Gizmo* this, GizmoAction action) {
    info("Gizmo Set: %d", action);
    this->action = action;
    this->activeView = this->curView;
}

////////////////////////////////////////////////////////////////////////////////

void Gizmo_Init(Gizmo* this, Input* input, void* vg) {
    this->input = input;
    this->vg = vg;
    this->gfxHead = new(Gfx[1024]);
}

void Gizmo_Free(Gizmo* this) {
    vfree(this->gfxHead);
}

////////////////////////////////////////////////////////////////////////////////

bool Gizmo_IsBusy(Gizmo* this) {
    if (this->release || this->lock.state)
        return true;
    
    return false;
}

void Gizmo_Draw(Gizmo* this) {
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
    
    View3D* view = this->curView;
    Split* split = this->curSplit;
    void* vg = this->vg;
    
    this->gfxDisp = this->gfxHead;
    
    Vec3f p = Math_Vec3f_ProjectAlong(this->pos, view->currentCamera->eye, view->currentCamera->at);
    f32 dist = Math_Vec3f_DistXYZ(p, view->currentCamera->eye);
    f32 s;
    
    if (!view->ortho)
        s = dist / 2000.0f * tanf(DegToRad(view->fovy) / 2.0f);
    else
        s = view->currentCamera->dist / 2850.0f;
    
    for (s32 i = 0; i < 3; i++) {
        if (!this->lock.state) {
            NVGcolor color = nvgHSL(1.0f - (i / 3.0f), 0.75f, 0.62f);
            
            gSPSegment(this->gfxDisp++, 6, (void*)gGizmo.data);
            Matrix_Push(); {
                Matrix_Translate(UnfoldVec3(this->pos), MTXMODE_NEW);
                Matrix_Scale(s, s, s, MTXMODE_APPLY);
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
                u8 alpha = this->focus.axis[i] ? 0xFF : 0xFF * dot;
                
                if (i == 1)
                    gDPSetEnvColor(this->gfxDisp++, UnfoldNVGcolor(color), alpha);
                
                if (i == 2)
                    gDPSetEnvColor(this->gfxDisp++, UnfoldNVGcolor(color), alpha);
                
                if (i == 0)
                    gDPSetEnvColor(this->gfxDisp++, UnfoldNVGcolor(color), alpha);
                
                if (this->focus.axis[i])
                    gXPSetHighlightColor(this->gfxDisp++, 0xFF, 0xFF, 0xFF, 0x40, DODGE);
                
                this->cyl[i].r = Math_Vec3f_DistXYZ(this->pos, view->currentCamera->eye) * 0.02f;
                
                gSPMatrix(this->gfxDisp++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
                gSPDisplayList(this->gfxDisp++, gGizmo_DlGizmo);
                
                if (this->focus.axis[i])
                    gXPClearHighlightColor(this->gfxDisp++);
            } Matrix_Pop();
        }
    }
    
    gSPEndDisplayList(this->gfxDisp++);
    n64_draw(this->gfxHead);
    
    if (this->action && this->activeView == this->curView) {
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
        
        char* txt = x_fmt("%s %.8g along %s",
                this->action == 1 ? "Move" : "Rotate",
                this->value,
                this->lock.x ? "X" : this->lock.y ? "Y" : "Z"
        );
        
        nvgFontSize(vg, SPLIT_TEXT);
        nvgFontFace(vg, "default");
        nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
        
        nvgFontBlur(vg, 1.0f);
        nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
        nvgText(vg, 8, split->rect.h - SPLIT_TEXT_H - SPLIT_ELEM_X_PADDING, txt, NULL);
        
        nvgFontBlur(vg, 0.0f);
        nvgFillColor(vg, Theme_GetColor(THEME_TEXT, 255, 1.0f));
        nvgText(vg, 8, split->rect.h - SPLIT_TEXT_H - SPLIT_ELEM_X_PADDING, txt, NULL);
    }
}

////////////////////////////////////////////////////////////////////////////////

int Gizmo_SetActiveContext(Gizmo* this, View3D* view, Split* split) {
    this->curView = view;
    this->curSplit = split;
    
    return (split->cursorInSplit || this->activeView == view);
}

void Gizmo_Update(Gizmo* this) {
    fornode(elem, this->elemHead) {
        if (elem->refresh) {
            elem->pos = *elem->dpos;
            elem->rot = *elem->drot;
            
            if (elem->focus)
                Gizmo_Focus(this, elem);
            
            elem->refresh = false;
        }
    }
    
    if (this->release == 2) {
        this->release = 0;
        
    } else if (this->release == 1)
        if ( Input_GetCursor(this->input, CLICK_L)->release)
            this->release = 2;
}

void Gizmo_UpdateView3D(Gizmo* this, Vec3f* rayPos) {
    void (* gizmoActionFunc[])(Gizmo*, Vec3f*) = {
        NULL,
        Gizmo_Move,
        Gizmo_Rotate,
    };
    View3D* view = this->curView;
    Input* input = this->input;
    bool alt = Input_GetKey(input, KEY_LEFT_ALT)->hold;
    
    if (!this->activeElem)
        return;
    
    Gizmo_UpdateMtx(this, view);
    
    if (Input_GetKey(input, KEY_G)->press) {
        if (!this->action) {
            this->initAction = true;
            this->initpos = this->pos;
            this->lock.state = GIZMO_AXIS_ALL_TRUE;
            this->refreshNodes = true;
            Gizmo_ResetTransforms(this);
        } else if (this->action != GIZMO_ACTION_MOVE)
            this->refreshTransforms = true;
        
        Gizmo_SetAction(this, GIZMO_ACTION_MOVE);
        
        if (alt)
            this->resetTransforms = true;
    }
    
    if (Input_GetKey(input, KEY_R)->press) {
        if (!this->action) {
            this->initAction = true;
            this->initpos = this->pos;
            this->lock.state = GIZMO_AXIS_ALL_TRUE;
            this->trackball = 0;
            Gizmo_ResetTransforms(this);
        } else if (this->action != GIZMO_ACTION_ROTATE)
            this->refreshTransforms = true;
        
        if (this->action == GIZMO_ACTION_ROTATE)
            this->trackball ^= 1;
        Gizmo_SetAction(this, GIZMO_ACTION_ROTATE);
        
        if (alt)
            this->resetTransforms = true;
    }
    
    if (this->resetTransforms || this->refreshTransforms) {
        this->refreshNodes = true;
        this->rot = Math_Vec3s_New(0, 0, 0);
        this->refreshTransforms = false;
        
        if (this->action == GIZMO_ACTION_MOVE)
            this->pos = Math_Vec3f_New(0, 0, 0);
        if (this->action == GIZMO_ACTION_ROTATE)
            this->rot = Math_Vec3s_New(0, 0, 0);
        
        if (this->resetTransforms) {
            if (this->action == GIZMO_ACTION_MOVE) {
                fornode(elem, this->elemHead) {
                    elem->pos = Math_Vec3f_New(0, 0, 0);
                    // *elem->dpos = Math_Vec3f_New(0, 0, 0);
                }
            }
            if (this->action == GIZMO_ACTION_ROTATE) {
                fornode(elem, this->elemHead) {
                    elem->rot = Math_Vec3s_New(0, 0, 0);
                    // *elem->drot = Math_Vec3s_New(0, 0, 0);
                }
            }
            
            Gizmo_ApplyTransforms(this);
            
            Gizmo_Reset(this);
            this->resetTransforms = false;
            return;
        }
    }
    
    if (this->refreshNodes) {
        fornode(elem, this->elemHead) {
            elem->pos = *elem->dpos;
            elem->rot = *elem->drot;
        }
        
        Gizmo_Focus(this, this->activeElem);
        this->refreshNodes = false;
    }
    
    if (!this->lock.state) {
        RayLine ray = View_GetCursorRayLine(view);
        
        for (s32 i = 0; i < 3; i++) {
            Vec3f p;
            this->focus.axis[i] = false;
            
            if (Col3D_LineVsCylinder(&ray, &this->cyl[i], &p)) {
                this->focus.axis[i] = true;
                
                if ( Input_GetCursor(input, CLICK_L)->press) {
                    Gizmo_SetAction(this, GIZMO_ACTION_MOVE);
                    this->initpos = this->pos;
                    this->lock.axis[i] = true;
                    this->initAction = true;
                    this->pressHold = true;
                    break;
                }
            }
        }
    }
    
    bool moved = 0;
    
    if (Input_GetKey(input, KEY_KP_0)->press) {
        View_MoveTo(view, this->pos);
        moved = true;
        this->typed[0] = 0;
        this->typing = 0;
    }
    
    if (Input_GetKey(input, KEY_KP_ADD)->press) {
        View_MoveTo(view, this->pos);
        View_RotTo(view, Math_Vec3s_New(
                this->activeElem->rot.x,
                this->activeElem->rot.y,
                this->activeElem->rot.z));
    }
    
    if (!this->action || this->activeView != view)
        return;
    
    if (Input_GetKey(input, KEY_X)->press) {
        if (this->lock.y || this->lock.z) {
            this->lock.state = GIZMO_AXIS_ALL_FALSE;
            this->lock.x = true;
            this->pos.y = this->initpos.y;
            this->pos.z = this->initpos.z;
            this->trackball = false;
        } else
            this->lock.state = GIZMO_AXIS_ALL_TRUE;
    }
    
    if (Input_GetKey(input, KEY_Z)->press) {
        if (this->lock.x || this->lock.z) {
            this->lock.state = GIZMO_AXIS_ALL_FALSE;
            this->lock.y = true;
            this->pos.x = this->initpos.x;
            this->pos.z = this->initpos.z;
            this->trackball = false;
        } else
            this->lock.state = GIZMO_AXIS_ALL_TRUE;
    }
    
    if (Input_GetKey(input, KEY_Y)->press) {
        if (this->lock.x || this->lock.y) {
            this->lock.state = GIZMO_AXIS_ALL_FALSE;
            this->lock.z = true;
            this->pos.x = this->initpos.x;
            this->pos.y = this->initpos.y;
            this->trackball = false;
        } else
            this->lock.state = GIZMO_AXIS_ALL_TRUE;
    }
    
    if (!moved)
        Gizmo_UpdateTyped(this, input);
    
    bool cancel = Input_GetKey(input, KEY_ESCAPE)->press;
    bool apply =  Input_GetKey(input, KEY_ENTER)->press;
    
    if (this->pressHold)
        apply |=  Input_GetCursor(input, CLICK_L)->release;
    else
        apply |=  Input_GetCursor(input, CLICK_L)->press ||  Input_GetCursor(input, CLICK_R)->press;
    
    if (apply || cancel) {
        if (apply)
            Gizmo_ApplyTransforms(this);
        else
            Gizmo_ResetTransforms(this);
        
        this->pressHold = false;
        Gizmo_Reset(this);
        return;
    }
    
    _log("Gizmo Update: %d", this->action);
    if (gizmoActionFunc[this->action])
        gizmoActionFunc[this->action](this, rayPos);
    
    this->initAction = false;
}

////////////////////////////////////////////////////////////////////////////////

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
    this->activeElem = NULL;
    
    while (this->elemHead) {
        this->elemHead->selected = false;
        this->elemHead->focus = false;
        Node_Remove(this->elemHead, this->elemHead);
    }
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
            Undo_Response(undo, &this->refreshNodes);
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
        elem->interact = false;
    }
    
    this->pos = *this->activeElem->dpos;
}
