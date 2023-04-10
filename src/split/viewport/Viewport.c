#include "Viewport.h"

extern DataFile gSpot;

void Viewport_Init(Editor* editor, Viewport* this, Split* split);
void Viewport_Destroy(Editor* editor, Viewport* this, Split* split);
void Viewport_Update(Editor* editor, Viewport* this, Split* split);
void Viewport_Draw(Editor* editor, Viewport* this, Split* split);

static void SaveConfig(void* __this, Split* split, Toml* toml, const char* prefix) {
    Viewport* this = __this;
    
    for (int i = 0; i < 3; i++) {
        Toml_SetVar(toml, x_fmt("%s.cam[0][%d]", prefix, i), "%g", this->view.currentCamera->eye.axis[i]);
        Toml_SetVar(toml, x_fmt("%s.cam[1][%d]", prefix, i), "%g", this->view.currentCamera->at.axis[i]);
        Toml_SetVar(toml, x_fmt("%s.cam[2][%d]", prefix, i), "%g", this->view.currentCamera->up.axis[i]);
        Toml_SetVar(toml, x_fmt("%s.cam[3][%d]", prefix, i), "%g", this->view.currentCamera->right.axis[i]);
        Toml_SetVar(toml, x_fmt("%s.cam[4][%d]", prefix, i), "%g", this->view.currentCamera->offset.axis[i]);
    }
    Toml_SetVar(toml, x_fmt("%s.cam[5][0]", prefix), "%g", this->view.currentCamera->dist);
    Toml_SetVar(toml, x_fmt("%s.cam[5][1]", prefix), "%g", this->view.currentCamera->targetDist);
    Toml_SetVar(toml, x_fmt("%s.cam[5][2]", prefix), "%d", this->view.currentCamera->pitch);
    Toml_SetVar(toml, x_fmt("%s.cam[5][3]", prefix), "%d", this->view.currentCamera->yaw);
    Toml_SetVar(toml, x_fmt("%s.cam[5][4]", prefix), "%d", this->view.currentCamera->roll);
}

static void LoadConfig(void* __this, Split* split, Toml* toml, const char* prefix) {
    Viewport* this = __this;
    
    for (int i = 0; i < 3; i++) {
        this->view.currentCamera->eye.axis[i] = Toml_GetFloat(toml, "%s.cam[0][%d]", prefix, i);
        this->view.currentCamera->at.axis[i] = Toml_GetFloat(toml, "%s.cam[1][%d]", prefix, i);
        this->view.currentCamera->up.axis[i] = Toml_GetFloat(toml, "%s.cam[2][%d]", prefix, i);
        this->view.currentCamera->right.axis[i] = Toml_GetFloat(toml, "%s.cam[3][%d]", prefix, i);
        this->view.currentCamera->offset.axis[i] = Toml_GetFloat(toml, "%s.cam[4][%d]", prefix, i);
    }
    this->view.currentCamera->dist = Toml_GetFloat(toml, "%s.cam[5][0]", prefix);
    this->view.currentCamera->targetDist = Toml_GetFloat(toml, "%s.cam[5][1]", prefix);
    this->view.currentCamera->pitch = Toml_GetInt(toml, "%s.cam[5][2]", prefix);
    this->view.currentCamera->yaw = Toml_GetInt(toml, "%s.cam[5][3]", prefix);
    this->view.currentCamera->roll = Toml_GetInt(toml, "%s.cam[5][4]", prefix);
}

SplitTask gViewportTask = {
    .taskName   = "Viewport",
    .init       = (void*)Viewport_Init,
    .destroy    = (void*)Viewport_Destroy,
    .update     = (void*)Viewport_Update,
    .draw       = (void*)Viewport_Draw,
    .saveConfig = SaveConfig,
    .loadConfig = LoadConfig,
    .size       = sizeof(Viewport)
};

////////////////////////////////////////////////////////////////////////////////

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

void Viewport_FocusRoom(Viewport* this, Scene* scene, int id) {
    RoomHeader* room = Scene_GetRoomHeader(scene, id);
    s16 yaw = Math_Vec3f_Yaw(this->view.currentCamera->eye, room->mesh->center);
    
    View_MoveTo(&this->view, room->mesh->center);
    View_ZoomTo(&this->view, room->mesh->size);
    View_RotTo(&this->view, Math_Vec3s_New(DegToBin(45), yaw, 0));
}

static void Viewport_CamUpdate(Editor* editor, Viewport* this, Split* split) {
    Input* input = &editor->input;
    Scene* scene = &editor->scene;
    Gizmo* gizmo = &editor->gizmo;
    View3D* view = &this->view;
    
    if (Input_GetCursor(input, CLICK_ANY)->hold && this->holdBlockCamUpdate)
        return;
    
    this->holdBlockCamUpdate = 0;
    
    if (Input_GetCursor(input, CLICK_ANY)->press && !split->inputAccess) {
        view->cameraControl = 0;
        this->holdBlockCamUpdate = true;
        
        return;
    }
    
    if (!view->cameraControl)
        if (split->inputAccess)
            if (View_CheckControlKeys(input))
                view->cameraControl = true;
    if (view->isControlled)
        view->cameraControl = true;
    
    if (!View_CheckControlKeys(input) && !split->inputAccess)
        view->cameraControl = 0;
    // if (Input_GetKey(input, KEY_LEFT_CONTROL)->hold)
    //     view->cameraControl = 0;
    if (gizmo->lock.state)
        view->cameraControl = 0;
    
    // Raycast
    if (split->cursorInSplit) {
        if (Input_GetKey(input, KEY_KP_DECIMAL)->press)
            Viewport_FocusRoom(this, scene, scene->curRoom);
        
        if (Input_SelectClick(input, CLICK_M) && Input_GetKey(input, KEY_LEFT_ALT)->hold) {
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
        
        if (Input_GetCursor(input, CLICK_L)->dual) {
            RayLine ray = View_GetCursorRayLine(&this->view);
            Room* room = Scene_RaycastRoom(&editor->scene, &ray, NULL);
            
            if (room)
                Scene_SetRoom(&editor->scene, room->id);
        }
    }
}

static void Viewport_GizmoSelection(Editor* editor, Viewport* this, Split* split) {
    RayLine ray = View_GetCursorRayLine(&this->view);
    Input* input = &editor->input;
    Actor* selectedActor = NULL;
    Actor* selHit = NULL;
    Vec3f p;
    Scene* scene = &editor->scene;
    RoomHeader* room = &scene->room[scene->curRoom].header[scene->curHeader];
    
    if (!split->cursorInSplit || editor->geo.state.blockElemInput)
        return;
    if (Gizmo_IsBusy(&editor->gizmo))
        return;
    if (!Input_SelectClick(input, CLICK_L))
        return;
    
    // Process rooms so that we do not grab actors behind walls
    if (Scene_RaycastRoom(&editor->scene, &ray, NULL))
        ray.nearest += 20.0f;
    
    for (var i = 0; i < room->actorList.num; i++) {
        Actor* actor = Arli_At(&room->actorList, i);
        
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
            Gizmo_Focus(&editor->gizmo, &scene->curActor->gizmo);
            Gizmo_Unselect(&editor->gizmo, &selHit->gizmo);
        } else {
            Gizmo_UnselectAll(&editor->gizmo);
            Actor_UnselectAll(scene, room);
        }
        
        return;
        
    } else if (!Input_GetKey(input, KEY_LEFT_SHIFT)->hold) {
        Gizmo_UnselectAll(&editor->gizmo);
        Actor_UnselectAll(scene, room);
    }
    
    if (selHit && !selectedActor)
        selectedActor = selHit;
    
    if (selectedActor) {
        Actor_Select(scene, selectedActor);
        Actor_Focus(scene, selectedActor);
        
        Gizmo_Select(&editor->gizmo, &selectedActor->gizmo, &selectedActor->pos, &selectedActor->rot);
        Gizmo_Focus(&editor->gizmo, &selectedActor->gizmo);
    }
}

static void Viewport_ShapeSelect_Update(Editor* editor, Viewport* this, Split* split, Scene* scene, Input* input) {
    if ( Input_GetCursor(input, CLICK_R)->hold) {
        Vec2f pos = Math_Vec2f_New(UnfoldVec2(split->cursorPos));
        
        if ( Input_GetCursor(input, CLICK_R)->press) {
            this->selID = 0;
            this->selPos[this->selID++] = pos;
            this->selMode = Input_GetKey(input, KEY_LEFT_CONTROL)->hold ? -1 : 1;
        }
        
        if (Math_Vec2f_DistXZ(pos, this->selPos[this->selID - 1]) > 4 && this->selID < 512) {
            this->selPos[this->selID++] = pos;
        }
    } else if ( Input_GetCursor(input, CLICK_R)->release) {
        BoundBox bbx = BoundBox_New2F(this->selPos[0]);
        
        for (var i = 0; i < this->selID; i++)
            BoundBox_Adjust2F(&bbx, this->selPos[i]);
        
        RoomHeader* room = Scene_GetRoomHeader(scene, scene->curRoom);
        Vec3f camn = Math_Vec3f_LineSegDir(this->view.currentCamera->eye, this->view.currentCamera->at);
        
        for (var i = 0; i < room->actorList.num; i++) {
            Actor* a = Arli_At(&room->actorList, i);
            Vec2f sp;
            
            switch (this->selMode) {
                case 1:
                    if (a->state & ACTOR_SELECTED)
                        continue;
                    break;
                    
                case -1:
                    if (!(a->state & ACTOR_SELECTED))
                        continue;
                    break;
            }
            
            sp = View_GetLocalScreenPos(&this->view, a->pos);
            
            if (Math_Vec2f_PointInShape(sp, this->selPos, this->selID)) {
                RayLine ray = View_GetRayLine(&this->view, sp);
                ray.end = a->pos;
                ray.end = Math_Vec3f_Add(ray.end, Math_Vec3f_MulVal(Math_Vec3f_LineSegDir(ray.end, ray.start), 10.0f));
                
                if (!Scene_RaycastRoom(scene, &ray, NULL)) {
                    if (0.0f > Math_Vec3f_Dot(camn, Math_Vec3f_LineSegDir(this->view.currentCamera->eye, a->pos)))
                        continue;
                    
                    switch (this->selMode) {
                        case 1:
                            Actor_Select(scene, a);
                            Gizmo_Select(&editor->gizmo, &a->gizmo, &a->pos, &a->rot);
                            
                            if (!scene->curActor) {
                                Actor_Focus(scene, a);
                                Gizmo_Focus(&editor->gizmo, &a->gizmo);
                            }
                            break;
                            
                        case -1:
                            Actor_Unselect(scene, a);
                            if (scene->curActor)
                                Gizmo_Focus(&editor->gizmo, &scene->curActor->gizmo);
                            Gizmo_Unselect(&editor->gizmo, &a->gizmo);
                            break;
                    }
                    
                }
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
        else nvgFillColor(vg, nvgHSLA(0.0f, 0.5f, 0.5f, 125));
        
        Gfx_Shape(vg, Math_Vec2f_New(0, 0), 1.0f, 0, this->selPos, this->selID);
        nvgFill(vg);
    }
}

////////////////////////////////////////////////////////////////////////////////

void Viewport_Init(Editor* editor, Viewport* this, Split* split) {
    View_Init(&this->view, &editor->input);
    
    Element_Button_SetProperties(&this->buttonResetCam, "Reset Camera", 0, 0);
    Element_Button_SetProperties(&this->buttonViewSmooth, "Smooth", true, !this->view.noSmooth);
    Element_Slider_SetParams(&this->sliderCamFov, 5, 175, "f32");
    Element_Name(&this->sliderCamFov, "FOV");
    this->sliderCamFov.isInt = true;
    this->buttonViewSmooth.align = NVG_ALIGN_CENTER;
    
    // this->view.mode = CAM_MODE_ORBIT;
    
    // Memfile_LoadBin(&this->object, "../object.zobj");
    // SkelAnime_Init(&this->object, &this->skelAnime, 0x06013990, 0x06015B20);
    // this->skelAnime.playSpeed = 1.0f;
}

void Viewport_Destroy(Editor* editor, Viewport* this, Split* split) {
    split->bg.useCustomPaint = false;
}

void Viewport_Update(Editor* editor, Viewport* this, Split* split) {
    Cursor* cursor = &editor->input.cursor;
    Scene* scene = &editor->scene;
    Gizmo* gizmo = &editor->gizmo;
    
    Element_Header(split->taskCombo, 98, &this->buttonResetCam, 98, &this->sliderCamFov, 112, &this->buttonViewSmooth, 98);
    Element_Combo(split->taskCombo);
    
    if (Element_Slider(&this->sliderCamFov))
        this->view.currentCamera->fovyTarget = Element_Slider_GetValue(&this->sliderCamFov);
    else
        Element_Slider_SetValue(&this->sliderCamFov, this->view.currentCamera->fovyTarget);
    
    if (Element_Button(&this->buttonViewSmooth))
        this->view.noSmooth = !this->buttonViewSmooth.state;
    
    if (Element_Button(&this->buttonResetCam)) {
        View_MoveTo(&this->view, Math_Vec3f_New(0, 0, 0));
        View_RotTo(&this->view, Math_Vec3s_New(0, 0, 0));
        this->view.currentCamera->targetDist = 100.0f;
    }
    
    if (editor->scene.segment == NULL) {
        split->bg.useCustomPaint = false;
        return;
    }
    
    SceneHeader* header = Scene_GetSceneHeader(scene);
    EnvLightSettings* envSettings = Arli_At(&header->envList, scene->curEnv);
    Input* input = &editor->input;
    GeoGrid* geo = &editor->geo;
    RoomHeader* room = Scene_GetRoomHeader(scene, scene->curRoom);
    
    Viewport_CamUpdate(editor, this, split);
    
    if (editor->scene.state & SCENE_DRAW_FOG)
        this->view.far = envSettings->fogFar;
    else
        this->view.far = 12800.0 + 6000;
    
    Viewport_ShapeSelect_Update(editor, this, split, &editor->scene, &editor->input);
    
    if (!Gizmo_IsBusy(gizmo) && split->cursorInSplit && !geo->state.blockElemInput) {
        if (Input_GetKey(input, KEY_DELETE)->press) {
            Arli* actorList = &room->actorList;
            int num = actorList->num;
            
            for (int i = num - 1; i > -1; i--) {
                Actor* actor = Arli_At(actorList, i);
                
                if (actor->state & ACTOR_SELECTED) {
                    Actor_Unselect(scene, actor);
                    Gizmo_Unselect(gizmo, &actor->gizmo);
                    
                    Arli_Remove(actorList, i, 1);
                }
            }
        }
        
        if (Input_GetShortcut(input, KEY_LEFT_CONTROL, KEY_D)) {
            // TODO duplicate
        }
    }
    
    // CursorIcon Wrapping
    if ( Input_GetCursor(input, CLICK_ANY)->press)
        return;
    
    if ((this->view.cameraControl && editor->input.cursor.clickAny.hold) || (gizmo->lock.state && gizmo->activeView == &this->view)) {
        s16 xMin = split->rect.x;
        s16 xMax = split->rect.x + split->rect.w;
        s16 yMin = split->rect.y;
        s16 yMax = split->rect.y + split->rect.h;
        
        if (cursor->pos.x < xMin || cursor->pos.x > xMax)
            Input_SetMousePos(&editor->input, wrapi(cursor->pos.x, xMin, xMax), MOUSE_KEEP_AXIS);
        
        if (cursor->pos.y < yMin || cursor->pos.y > yMax)
            Input_SetMousePos(&editor->input, MOUSE_KEEP_AXIS, wrapi(cursor->pos.y, yMin, yMax));
    }
    
    rgb8_t color = {
        envSettings->fogColor[0],
        envSettings->fogColor[1],
        envSettings->fogColor[2]
    };
    hsl_t hsl;
    
    Color_Convert2hsl(&hsl, &color);
    
    split->bg.paint = nvgLinearGradient(editor->vg, 0, 0, 0, split->rect.h, nvgHSL(hsl.h, hsl.s, hsl.l), nvgHSL(hsl.h, hsl.s * 0.75f, hsl.l * 0.75f));
    split->bg.useCustomPaint = true;
}

////////////////////////////////////////////////////////////////////////////////

static void ProfilerText(void* vg, s32 row, const char* msg, const char* fmt, f32 val, f32 dangerValue) {
    nvgFontSize(vg, SPLIT_TEXT + 2);
    nvgFontFace(vg, "default");
    nvgFontBlur(vg, 1.0f);
    nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
    for (s32 i = 0; i < 6; i++)
        nvgText(vg, 8, 8 + SPLIT_TEXT_H * row, msg, NULL);
    
    nvgFontBlur(vg, 0.0f);
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 225));
    nvgText(vg, 8, 8 + SPLIT_TEXT_H * row, msg, NULL);
    
    nvgFontFace(vg, "default");
    nvgFontBlur(vg, 1.0f);
    nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
    for (s32 i = 0; i < 6; i++)
        nvgText(vg, 8 + 120, 8 + SPLIT_TEXT_H * row, x_fmt(fmt, val), NULL);
    
    nvgFontBlur(vg, 0.0f);
    if (dangerValue)
        nvgFillColor(vg, nvgHSLA(SQ(clamp(val / dangerValue, 0, 1)) * 0.5f + 0.5f, 0.6, 0.6, 225));
    else
        nvgFillColor(vg, nvgRGBA(255, 255, 255, 225));
    nvgText(vg, 8 + 120, 8 + SPLIT_TEXT_H * row, x_fmt(fmt, val), NULL);
}

static bool Viewport_N64CullingCallback(void* userData, const n64_cullingCallbackData * vtx, u32 numVtx) {
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
    n64_graph_init();
    n64_set_culling(editor->scene.state & SCENE_DRAW_CULLING);
    
    View_Update(&this->view, &editor->input, split);

    n64_setMatrix_model(&this->view.modelMtx);
    n64_setMatrix_view(&this->view.viewMtx);
    n64_setMatrix_projection(&this->view.projMtx);
    n64_set_cullingCallbackFunc(&this->view, Viewport_N64CullingCallback);
}

static void Viewport_Draw_NoFile(Editor* editor, Viewport* this, Split* split) {
    const char* txt = "z64scene";
    void* vg = editor->vg;
    
    nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
    nvgFontBlur(vg, 0.0);
    
    nvgFontFace(vg, "default-bold");
    nvgFontSize(vg, 35);
    nvgFillColor(vg, Theme_GetColor(THEME_TEXT, 255, 1.0f));
    nvgText(vg, split->rect.w * 0.5, split->rect.h * 0.5, txt, NULL);
    
    nvgFontFace(vg, "default");
    nvgFontSize(vg, 15);
    nvgFillColor(vg, Theme_GetColor(THEME_BASE, 255, 2.0f));
    nvgText(vg, split->rect.w * 0.5, split->rect.h * 0.5 + 35 * 0.75f, "drop files here", NULL);
}

static void Viewport_Draw_Scene(Editor* editor, Viewport* this, Split* split) {
    Scene* scene = &editor->scene;
    
    Viewport_InitDraw(editor, this, split);
    Scene_ViewportUpdate(scene, &this->view);
    
    if (Gizmo_SetActiveContext(&editor->gizmo, &this->view, split)) {
        Gizmo_UpdateView3D(&editor->gizmo, scene->mesh.rayHit ? &scene->mesh.rayPos : NULL);
        Viewport_GizmoSelection(editor, this, split);
    }
    
#if 0
    Matrix_Push(); {
        gxSPSegment(POLY_OPA_DISP++, 0x6, this->object.data);
        gDPSetEnvColor(POLY_OPA_DISP++, 0xFF, 0xFF, 0xFF, 0xFF);
        
        Matrix_Translate(0, 0, 0, MTXMODE_NEW);
        Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
        
        gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
        
        SkelAnime_Update(&this->skelAnime);
        SkelAnime_Draw(&this->skelAnime, SKELANIME_FLEX);
    } Matrix_Pop();
#endif
    
    Scene_Draw(&editor->scene, &this->view);
    
    Gizmo_Draw(&editor->gizmo);
    Viewport_ShapeSelect_Draw(this, editor->vg);
}

char sMsg[32][128];
int sMsgState[32];

void SlotMsg(int i, const char* fmt, ...) {
    va_list va;
    
    va_start(va, fmt);
    xl_vsnprintf(sMsg[i], 128, fmt, va);
    va_end(va);
    sMsgState[i] = true;
}

void Viewport_Draw(Editor* editor, Viewport* this, Split* split) {
    void* vg = editor->vg;
    
    if (editor->scene.segment == NULL)
        Viewport_Draw_NoFile(editor, this, split);
    else
        Viewport_Draw_Scene(editor, this, split);
    
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgFontSize(vg, 15);
    nvgTextLetterSpacing(vg, 0.0f);
    
    ProfilerText(vg, 0, "FPS:", "%.0f", 1 / gFpsTime, 0);
    ProfilerText(vg, 1, "Total:", "%.2fms", gTotalTime * 1000.0f, 16.0f);
    ProfilerText(vg, 2, "Delta:", "%.2f", gDeltaTime, 0);
    
    for (int i = 0; i < ArrCount(sMsg); i++) {
        if (sMsg[i][0]) {
            nvgFontSize(vg, SPLIT_TEXT + 2);
            nvgFontFace(vg, "default");
            nvgFontBlur(vg, 1.0f);
            nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
            for (s32 i = 0; i < 2; i++)
                nvgText(vg, 8, 8 + SPLIT_TEXT_H * (3 + i), sMsg[i], NULL);
            
            nvgFontBlur(vg, 0.0f);
            if (sMsgState[i])
                nvgFillColor(vg, nvgRGBA(255, 255, 255, 225));
            else
                nvgFillColor(vg, nvgRGBA(80, 80, 80, 225));
            nvgText(vg, 8, 8 + SPLIT_TEXT_H * (3 + i), sMsg[i], NULL);
            
            sMsgState[i] = false;
        }
    }
}
