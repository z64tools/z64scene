#include <Editor.h>

extern DataFile gCube;
#include "../assets/3D/Cube.h"

void Actor_Draw(Actor* this, View3D* view) {
    Vec3f pos = this->pos;
    Vec3s rot = this->rot;
    
    if (this->gizmo.interact && this->gizmo.selected) {
        pos = this->gizmo.pos;
        rot = this->gizmo.rot;
    }
    
    if (!View_PointInScreen(view, pos))
        return;
    
    Matrix_Push(); {
        Matrix_Translate(UnfoldVec3(pos), MTXMODE_APPLY);
        Matrix_Push(); {
            Matrix_Scale(0.01, 0.01, 0.01, MTXMODE_APPLY);
            Matrix_RotateY_s(rot.y, MTXMODE_APPLY);
            Matrix_RotateX_s(rot.x, MTXMODE_APPLY);
            Matrix_RotateZ_s(rot.z, MTXMODE_APPLY);
            
#if 0
            if (this->state & ACTOR_SELECTED) {
                Matrix_Scale(1.25f, 1.25f, 1.25f, MTXMODE_APPLY);
                gXPModeSet(POLY_STENCIL++, GX_MODE_STENCIL);
                gSPSegment(POLY_STENCIL++, 6, (void*)gCube.data);
                gSPMatrix(POLY_STENCIL++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
                gDPSetEnvColor(POLY_STENCIL++, 0xFF, 0x0, 0x0, 0x30);
                
                if (Math_Vec3f_DistXYZ(this->pos, view->currentCamera->eye) < 500.0f)
                    gSPDisplayList(POLY_STENCIL++, gCube_DlCube);
                else
                    gSPDisplayList(POLY_STENCIL++, gCube_DlCubeLOD);
                gXPModeClear(POLY_STENCIL++, GX_MODE_STENCIL);
                
                Matrix_Scale(1 / 1.25f, 1 / 1.25f, 1 / 1.25f, MTXMODE_APPLY);
            }
#endif
            {
                gSPSegment(POLY_OPA_DISP++, 6, (void*)gCube.data);
                gDPSetEnvColor(POLY_OPA_DISP++, 0x60, 0x60, 0x60, 0xFF);
                
                gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
                
                if (this->state & ACTOR_SELECTED) {
                    if (this->gizmo.focus)
                        gXPSetHighlightColor(POLY_OPA_DISP++, 252, 186, 0, 0xA0, DODGE);
                    else
                        gXPSetHighlightColor(POLY_OPA_DISP++, 252, 186, 0, 0x70, DODGE);
                }
                
                if (Math_Vec3f_DistXYZ(this->pos, view->currentCamera->eye) < 500.0f)
                    gSPDisplayList(POLY_OPA_DISP++, gCube_DlCube);
                else
                    gSPDisplayList(POLY_OPA_DISP++, gCube_DlCubeLOD);
                
                if (this->state & ACTOR_SELECTED)
                    gXPClearHighlightColor(POLY_OPA_DISP++);
            }
            
        } Matrix_Pop();
    } Matrix_Pop();
}

void Actor_Draw_RoomHeader(RoomHeader* header, View3D* view) {
    for (var i = 0; i < header->actorList.num; i++)
        Actor_Draw(&header->actorList.entry[i], view);
}

void Actor_Focus(Scene* scene, Actor* this) {
    scene->curActor = this;
}

void Actor_Select(Scene* scene, Actor* this) {
    arrmove_r(scene->prevActor, 0, 64);
    scene->prevActor[0] = this;
    this->state |= ACTOR_SELECTED;
}

void Actor_Unselect(Scene* scene, Actor* this) {
    this->state &= ~ACTOR_SELECTED;
    
    foreach(i, scene->prevActor) {
        if (scene->prevActor[i] == this) {
            scene->prevActor[i] = NULL;
            arrmove_l(scene->prevActor, i, 64 - i);
        }
    }
    
    if (scene->curActor == this) {
        scene->curActor = scene->prevActor[0];
        scene->prevActor[0] = NULL;
        arrmove_l(scene->prevActor, 0, 64);
    }
}

void Actor_UnselectAll(Scene* scene, RoomHeader* hdr) {
    scene->curActor = NULL;
    memset(scene->prevActor, 0, sizeof(scene->prevActor));
    for (var i = 0; i < hdr->actorList.num; i++)
        hdr->actorList.entry[i].state &= ~ACTOR_SELECTED;
}
