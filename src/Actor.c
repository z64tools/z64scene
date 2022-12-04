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
            
            if (this->state & ACTOR_SELECTED)
                gDPSetEnvColor(POLY_OPA_DISP++, 0x40, 0xA0, 0xF0, 0xFF);
            else
                gDPSetEnvColor(POLY_OPA_DISP++, 0xC0, 0xC0, 0xC0, 0xFF);
            
            gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
            
            if (Math_Vec3f_DistXYZ(this->pos, view->currentCamera->eye) < 500.0f)
                gSPDisplayList(POLY_OPA_DISP++, gCubeDL);
            else
                gSPDisplayList(POLY_OPA_DISP++, gCubeLodDL);
        } Matrix_Pop();
    } Matrix_Pop();
}

void Actor_Draw_RoomHeader(RoomHeader* header, View3D* view) {
    for (var i = 0; i < header->actorList.num; i++)
        Actor_Draw(&header->actorList.entry[i], view);
}

void Actor_Select(Scene* scene, Actor* this) {
    ArrMoveR(scene->prevActor, 0, 64);
    scene->prevActor[0] = scene->curActor;
    scene->curActor = this;
    this->state |= ACTOR_SELECTED;
}

void Actor_Unselect(Scene* scene, Actor* this) {
    this->state &= ~ACTOR_SELECTED;
    
    foreach(i, scene->prevActor) {
        if (scene->prevActor[i] == this) {
            scene->prevActor[i] = NULL;
            ArrMoveL(scene->prevActor, i, 64 - i);
        }
    }
    
    if (scene->curActor == this) {
        scene->curActor = scene->prevActor[0];
        scene->prevActor[0] = NULL;
        ArrMoveL(scene->prevActor, 0, 64);
    }
}

void Actor_UnselectAll(Scene* scene, RoomHeader* hdr) {
    scene->curActor = NULL;
    memset(scene->prevActor, 0, sizeof(scene->prevActor));
    for (var i = 0; i < hdr->actorList.num; i++)
        hdr->actorList.entry[i].state &= ~ACTOR_SELECTED;
}
