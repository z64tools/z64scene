#include "Viewport.h"

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

#if 0
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
#endif

static void Viewport_Camera_Update(Editor* editor, Viewport* this, Split* split) {
    Input* input = &editor->input;
    Scene* scene = &editor->scene;
    Gizmo* gizmo = &scene->gizmo;
    View3D* view = &this->view;
    
    if (Input_GetMouse(input, CLICK_ANY)->hold && this->lockCameraAccess)
        return;
    this->lockCameraAccess = false;
    
    if (!View_CheckControlKeys(input) && !split->inputAccess)
        view->cameraControl = false;
    
    if (Input_GetKey(input, KEY_LEFT_CONTROL)->hold)
        view->cameraControl = false;
    
    if (Input_GetMouse(input, CLICK_ANY)->press && !split->inputAccess) {
        view->cameraControl = false;
        this->lockCameraAccess = true;
        
        return;
    }
    
    if (!view->cameraControl) {
        if (split->inputAccess) {
            if (View_CheckControlKeys(input))
                view->cameraControl = true;
        }
    }
    
    if (gizmo->lock.state)
        view->cameraControl = false;
    
    if (view->isControlled)
        view->cameraControl = true;
    
    if (split->mouseInSplit) {
        if (Input_GetKey(input, KEY_KP_DECIMAL)->press) {
            RoomHeader* room = Scene_GetRoomHeader(scene, scene->curRoom);
            s16 yaw = Math_Vec3f_Yaw(this->view.currentCamera->eye, room->mesh->center);
            
            View_MoveTo(&this->view, room->mesh->center);
            View_ZoomTo(&this->view, room->mesh->size);
            View_RotTo(&this->view, Math_Vec3s_New(DegToBin(45), yaw, 0));
        }
        
        if (Input_GetMouse(input, CLICK_M)->press && Input_GetKey(input, KEY_LEFT_ALT)->hold) {
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
        
        if (Input_GetMouse(input, CLICK_L)->dual) {
            RayLine ray = View_GetCursorRayLine(&this->view);
            Room* room = Scene_RaycastRoom(&editor->scene, &ray, NULL);
            
            if (room) {
                RoomHeader* hdrroom = &room->header[scene->curHeader];
                Scene_SetRoom(&editor->scene, room->id);
                s16 yaw = Math_Vec3f_Yaw(this->view.currentCamera->eye, hdrroom->mesh->center);
                
                View_MoveTo(&this->view, hdrroom->mesh->center);
                View_ZoomTo(&this->view, hdrroom->mesh->size);
                View_RotTo(&this->view, Math_Vec3s_New(DegToBin(45), yaw, 0));
            }
        }
    }
}

static void Viewport_Actor_Update(Editor* editor, Viewport* this, Split* split) {
    RayLine ray = View_GetCursorRayLine(&this->view);
    Input* input = &editor->input;
    Actor* selectedActor = NULL;
    Actor* selHit = NULL;
    Vec3f p;
    Scene* scene = &editor->scene;
    RoomHeader* room = &scene->room[scene->curRoom].header[scene->curHeader];
    
    if (!split->mouseInSplit || editor->geo.state.blockElemInput)
        return;
    if (Gizmo_IsBusy(&scene->gizmo) || !Input_GetMouse(input, CLICK_L)->press)
        return;
    
    // Process rooms so that we do not grab actors behind walls
    if (Scene_RaycastRoom(&editor->scene, &ray, NULL))
        ray.nearest += 20.0f;
    
    for (var i = 0; i < room->actorList.num; i++) {
        Actor* actor = &room->actorList.entry[i];
        
        veccpy(&actor->sph.pos, &actor->pos);
        actor->sph.pos.y += 10.0f;
        actor->sph.r = 20;
        
        if (Col3D_LineVsSphere(&ray, &actor->sph, &p)) {
            if (actor->state & ACTOR_SELECTED) {
                selHit = actor;
                
                continue;
            }
            
            selectedActor = actor;
        }
    }
    
    if (selHit && !selectedActor && Input_GetKey(input, KEY_LEFT_SHIFT)->hold) {
        Actor_Unselect(scene, selHit);
        if (scene->curActor) {
            Gizmo_Focus(&scene->gizmo, &scene->curActor->gizmo);
            Gizmo_Unselect(&scene->gizmo, &selHit->gizmo);
        } else {
            Gizmo_UnselectAll(&scene->gizmo);
            Actor_UnselectAll(scene, room);
        }
        
        return;
    } else if (!Input_GetKey(input, KEY_LEFT_SHIFT)->hold) {
        Gizmo_UnselectAll(&scene->gizmo);
        Actor_UnselectAll(scene, room);
    }
    
    if (selHit && !selectedActor)
        selectedActor = selHit;
    
    if (selectedActor) {
        Actor_Select(scene, selectedActor);
        Actor_Focus(scene, selectedActor);
        
        Gizmo_Select(&scene->gizmo, &selectedActor->gizmo,
            &selectedActor->pos, &selectedActor->rot);
        Gizmo_Focus(&scene->gizmo, &selectedActor->gizmo);
    }
}

static void Viewport_ShapeSelect_Update(Viewport* this, Split* split, Scene* scene, Input* input) {
    if (Input_GetMouse(input, CLICK_R)->hold) {
        Vec2f pos = Math_Vec2f_New(UnfoldVec2(split->cursorPos));
        
        if (Input_GetMouse(input, CLICK_R)->press) {
            this->selID = 0;
            this->selPos[this->selID++] = pos;
            this->selMode = Input_GetKey(input, KEY_LEFT_CONTROL)->hold ? -1 : 1;
        }
        
        if (Math_Vec2f_DistXZ(pos, this->selPos[this->selID - 1]) > 4 && this->selID < 512) {
            this->selPos[this->selID++] = pos;
        }
    } else if (Input_GetMouse(input, CLICK_R)->release) {
        BoundBox bbx = BoundBox_New2F(this->selPos[0]);
        
        for (var i = 0; i < this->selID; i++) {
            BoundBox_Adjust2F(&bbx, this->selPos[i]);
        }
        
        RoomHeader* room = Scene_GetRoomHeader(scene, scene->curRoom);
        Vec3f camn = Math_Vec3f_LineSegDir(this->view.currentCamera->eye, this->view.currentCamera->at);
        for (var i = 0; i < room->actorList.num; i++) {
            Actor* a = &room->actorList.entry[i];
            Vec2f sp;
            
            switch (this->selMode) {
                case 1:
                    if (a->state & ACTOR_SELECTED)
                        continue;
                    
                    sp = View_GetScreenPos(&this->view, a->pos);
                    
                    if (Math_Vec2f_PointInShape(sp, this->selPos, this->selID)) {
                        Actor_Select(scene, a);
                        Gizmo_Select(&scene->gizmo, &a->gizmo, &a->pos, &a->rot);
                        
                        if (!scene->curActor) {
                            Actor_Focus(scene, a);
                            Gizmo_Focus(&scene->gizmo, &a->gizmo);
                        }
                    }
                    break;
                    
                case -1:
                    if (!(a->state & ACTOR_SELECTED))
                        continue;
                    
                    sp = View_GetScreenPos(&this->view, a->pos);
                    
                    if (Math_Vec2f_PointInShape(sp, this->selPos, this->selID)) {
                        if (0.0f > Math_Vec3f_Dot(camn, Math_Vec3f_LineSegDir(this->view.currentCamera->eye, a->pos)))
                            continue;
                        
                        Actor_Unselect(scene, a);
                        if (scene->curActor)
                            Gizmo_Focus(&scene->gizmo, &scene->curActor->gizmo);
                        Gizmo_Unselect(&scene->gizmo, &a->gizmo);
                    }
                    break;
            }
        }
        
        this->selID = 0;
    }
}

static void Viewport_ShapeSelect_Draw(Viewport* this, void* vg) {
    if (this->selID) {
        nvgBeginPath(vg);
        if (this->selMode > 0)
            nvgFillColor(vg, nvgHSLA(0.6f, 0.5f, 0.5f, 125));
        else
            nvgFillColor(vg, nvgHSLA(0.0f, 0.5f, 0.5f, 125));
        Gfx_Shape(vg, Math_Vec2f_New(0, 0), 1.0f, 0, this->selPos, this->selID);
        nvgFill(vg);
    }
}

// # # # # # # # # # # # # # # # # # # # #
// #                                     #
// # # # # # # # # # # # # # # # # # # # #

void Viewport_Init(Editor* editor, Viewport* this, Split* split) {
    View_Init(&this->view, &editor->input);
    
    if (gInstance++ == 0) {
        gVOPAHead = new(Gfx[4096]);
        gVXLUHead = new(Gfx[4096]);
    }
    
    Element_Name(&this->resetCam, "Reset Camera");
    this->resetCam.align = ALIGN_LEFT;
}

void Viewport_Destroy(Editor* editor, Viewport* this, Split* split) {
    split->bg.useCustomPaint = false;
    
    if (--gInstance == 0)
        vfree(gVOPAHead, gVXLUHead);
}

void Viewport_Update(Editor* editor, Viewport* this, Split* split) {
    SceneHeader* header;
    Cursor* cursor = &editor->input.cursor;
    Scene* scene = &editor->scene;
    Gizmo* gizmo = &scene->gizmo;
    
    Element_Header(split->taskCombo, 98, &this->resetCam, 98);
    Element_Combo(split->taskCombo);
    
    if (Element_Button(&this->resetCam)) {
        info("Reset Camera");
        View_MoveTo(&this->view, Math_Vec3f_New(0, 0, 0));
        View_RotTo(&this->view, Math_Vec3s_New(0, 0, 0));
    }
    
    if (editor->scene.segment == NULL) {
        split->bg.useCustomPaint = false;
        return;
    }
    
    Viewport_Camera_Update(editor, this, split);
    header = Scene_GetSceneHeader(&editor->scene);
    
    if (editor->scene.state & SCENE_DRAW_FOG)
        this->view.far = header->envList.entry[editor->scene.curEnv].fogFar;
    else
        this->view.far = 12800.0 + 6000;
    
    Viewport_ShapeSelect_Update(this, split, &editor->scene, &editor->input);
    
    // CursorIcon Wrapping
    if (Input_GetMouse(&editor->input, CLICK_ANY)->press)
        return;
    
    if ((this->view.cameraControl && editor->input.cursor.clickAny.hold) || gizmo->lock.state) {
        s16 xMin = split->rect.x;
        s16 xMax = split->rect.x + split->rect.w;
        s16 yMin = split->rect.y;
        s16 yMax = split->rect.y + split->rect.h;
        
        if (cursor->pos.x < xMin || cursor->pos.x > xMax)
            Input_SetMousePos(&editor->input, wrapi(cursor->pos.x, xMin, xMax), MOUSE_KEEP_AXIS);
        
        if (cursor->pos.y < yMin || cursor->pos.y > yMax)
            Input_SetMousePos(&editor->input, MOUSE_KEEP_AXIS, wrapi(cursor->pos.y, yMin, yMax));
    }
}

// # # # # # # # # # # # # # # # # # # # #
// #                                     #
// # # # # # # # # # # # # # # # # # # # #

static void ProfilerText(void* vg, s32 row, const char* msg, const char* fmt, f32 val, f32 dangerValue) {
    nvgFontSize(vg, SPLIT_TEXT);
    nvgFontFace(vg, "default");
    nvgFontBlur(vg, 1.0f);
    nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
    for (s32 i = 0; i < 2; i++)
        nvgText(vg, 8, 8 + SPLIT_TEXT_H * row, msg, NULL);
    
    nvgFontBlur(vg, 0.0f);
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 225));
    nvgText(vg, 8, 8 + SPLIT_TEXT_H * row, msg, NULL);
    
    nvgFontFace(vg, "default");
    nvgFontBlur(vg, 1.0f);
    nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
    for (s32 i = 0; i < 2; i++)
        nvgText(vg, 8 + 120, 8 + SPLIT_TEXT_H * row, x_fmt(fmt, val), NULL);
    
    nvgFontBlur(vg, 0.0f);
    if (dangerValue)
        nvgFillColor(vg, nvgHSLA(SQ(clamp(val / dangerValue, 0, 1)) * 0.5f + 0.5f, 0.6, 0.6, 225));
    else
        nvgFillColor(vg, nvgRGBA(255, 255, 255, 225));
    nvgText(vg, 8 + 120, 8 + SPLIT_TEXT_H * row, x_fmt(fmt, val), NULL);
}

static void Viewport_InitDraw(Editor* editor, Viewport* this, Split* split) {
    n64_graph_init();
    n64_set_culling(editor->scene.state & SCENE_DRAW_CULLING);
    
    View_Update(&this->view, &editor->input, split);
    
    nested(bool, N64_CullingCallback, (void* userData, const n64_cullingCallbackData * vtx, u32 numVtx)) {
        View3D* view = userData;
        
        if (view->ortho)
            return false;
        
        for (s32 i = 0; i < numVtx; i++, vtx++) {
            if (!View_PointInScreen(userData, Math_Vec3f_New(vtx->x, vtx->y, vtx->z)))
                continue;
            
            return false;
        }
        
        return true;
    };
    
    n64_setMatrix_model(&this->view.modelMtx);
    n64_setMatrix_view(&this->view.viewMtx);
    n64_setMatrix_projection(&this->view.projMtx);
    n64_set_cullingCallbackFunc(&this->view, (void*)N64_CullingCallback);
}

static void Viewport_Draw2D(Editor* editor, Viewport* this, Split* split) {
    const char* txt = "z64scene";
    void* vg = editor->vg;
    
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

static void Viewport_Draw3D(Editor* editor, Viewport* this, Split* split) {
    Scene* scene = &editor->scene;
    
    Viewport_InitDraw(editor, this, split);
    
    gVOPADisp = gVOPAHead;
    gVXLUDisp = gVXLUHead;
    
    Scene_Update(scene, &this->view);
    
    profi_start(2);
    if (split->mouseInSplit) {
        Gizmo_Update(&scene->gizmo, &this->view, &editor->input, scene->mesh.rayHit ? &scene->mesh.rayPos : NULL);
        Viewport_Actor_Update(editor, this, split);
    }
    profi_stop(2);
    
    profi_start(0);
    Scene_Draw(&editor->scene, &this->view);
    profi_stop(0);
    
    Gizmo_Draw(&scene->gizmo, &this->view, &POLY_VOPA_DISP);
    
    gSPEndDisplayList(POLY_VOPA_DISP++);
    gSPEndDisplayList(POLY_VXLU_DISP++);
    n64_draw(gVOPAHead);
    n64_draw(gVXLUHead);
    
    Viewport_ShapeSelect_Draw(this, editor->vg);
    
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
        Viewport_Draw2D(editor, this, split);
    else
        Viewport_Draw3D(editor, this, split);
    
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgFontSize(vg, 15);
    nvgTextLetterSpacing(vg, 0.0f);
    
    ProfilerText(vg, 0, "FPS:", "%.0f", 1 / profi_get(PROFILER_FPS), 0);
    ProfilerText(vg, 1, "Total:", "%.2fms", profi_get(0xF0) * 1000.0f, 16.0f);
    ProfilerText(vg, 2, "Scene Draw:", "%.2fms", profi_get(0) * 1000.f, 16.0f);
    ProfilerText(vg, 3, "Gizmo Update:", "%.2fms", profi_get(2) * 1000.f, 16.0f);
    ProfilerText(vg, 4, "n64:", "%.2fms", profi_get(8) * 1000.f, 16.0f);
    ProfilerText(vg, 5, "Delta:", "%.2f", gDeltaTime, 0);
}
