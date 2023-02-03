#include <Editor.h>

extern DataFile gCube;

#define gCubeDL    0x06000990
#define gCubeLodDL 0x06000BF0

void Actor_Draw(Actor* this, View3D* view) {
    Vec3f pos = this->pos;
    Vec3s rot = this->rot;
    
    if (this->gizmo.selected) {
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
            
            gSPSegment(POLY_OPA_DISP++, 6, (void*)gCube.data);
            gDPSetEnvColor(POLY_OPA_DISP++, 0x60, 0x60, 0x60, 0xFF);
            
            gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
            
            if (this->state & ACTOR_SELECTED) {
                if (this->gizmo.focus)
                    gXPSetHighlightColor(POLY_OPA_DISP++, 3, 252, 240, 0x80, DODGE);
                else
                    gXPSetHighlightColor(POLY_OPA_DISP++, 3, 252, 240, 0x40, DODGE);
            }
            
            if (Math_Vec3f_DistXYZ(this->pos, view->currentCamera->eye) < 500.0f)
                gSPDisplayList(POLY_OPA_DISP++, gCubeDL);
            else
                gSPDisplayList(POLY_OPA_DISP++, gCubeLodDL);
            
            if (this->state & ACTOR_SELECTED)
                gXPClearHighlightColor(POLY_OPA_DISP++);
            
#if 0
            if (this->state & ACTOR_SELECTED) {
                gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
                
                gXPModeSet(POLY_OPA_DISP++, GX_MODE_STENCILWRITE);
                if (Math_Vec3f_DistXYZ(this->pos, view->currentCamera->eye) < 500.0f)
                    gSPDisplayList(POLY_OPA_DISP++, gCubeDL);
                else
                    gSPDisplayList(POLY_OPA_DISP++, gCubeLodDL);
                gXPModeClear(POLY_OPA_DISP++, GX_MODE_STENCILWRITE);
            }
#endif
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
