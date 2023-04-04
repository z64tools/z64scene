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
            
            gSPSegment(POLY_OPA_DISP++, 6, (void*)gCube.data);
            gDPSetEnvColor(POLY_OPA_DISP++, 0xF0, 0xF0, 0xF0, 0xFF);
            
            gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
            
            if (this->state & ACTOR_SELECTED) {
                gDPSetEnvColor(POLY_OPA_DISP++, 0xC0, 0xC0, 0xC0, 0xFF);
                if (this->gizmo.focus)
                    gXPSetHighlightColor(POLY_OPA_DISP++, 0xFF, 0x85, 0x00, 0xC0, DODGE);
                else
                    gXPSetHighlightColor(POLY_OPA_DISP++, 0xFF, 0x85, 0x00, 0xC0, MUL);
            }
            
            gSPDisplayList(POLY_OPA_DISP++, gCube_DlCube);
            
            if (this->state & ACTOR_SELECTED)
                gXPClearHighlightColor(POLY_OPA_DISP++);
            
        } Matrix_Pop();
    } Matrix_Pop();
}

void Actor_Draw_RoomHeader(RoomHeader* header, View3D* view) {
    for (var i = 0; i < header->actorList.num; i++)
        Actor_Draw(Arli_At(&header->actorList, i), view);
}

void Actor_Focus(Scene* scene, Actor* this) {
    RoomHeader* room = Scene_GetRoomHeader(scene, scene->curRoom);
    
    scene->curActor = this;
    Arli_Set(&room->actorList, Arli_IndexOf(&room->actorList, this));
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
    for (var i = 0; i < hdr->actorList.num; i++) {
        Actor* actor = Arli_At(&hdr->actorList, i);
        
        actor->state &= ~ACTOR_SELECTED;
    }
}

u16 Actor_rmask(Actor* this, int source, u16 mask) {
    u16 val = 0;
    
    switch (source) {
        case 'vari':
            val = this->param;
            break;
        case 'posx':
            val = this->pos.x;
            break;
        case 'posy':
            val = this->pos.y;
            break;
        case 'posz':
            val = this->pos.z;
            break;
        case 'rotx':
            val = this->rot.x;
            break;
        case 'roty':
            val = this->rot.y;
            break;
        case 'rotz':
            val = this->rot.z;
            break;
    }
    
    return rmask(val, mask);
}

void Actor_wmask(Actor* this, int source, u16 value, u16 mask) {
    u16 val = wmask(value, mask);
    s16 t;
    
    switch (source) {
        case 'vari':
            this->param &= ~mask;
            this->param |= val;
            break;
        case 'posx':
            t = this->pos.x;
            t &= ~mask;
            t |= val;
            this->pos.x = t;
            break;
        case 'posy':
            t = this->pos.y;
            t &= ~mask;
            t |= val;
            this->pos.y = t;
            break;
        case 'posz':
            t = this->pos.z;
            t &= ~mask;
            t |= val;
            this->pos.z = t;
            break;
        case 'rotx':
            this->rot.x &= ~mask;
            this->rot.x |= val;
            break;
        case 'roty':
            this->rot.y &= ~mask;
            this->rot.y |= val;
            break;
        case 'rotz':
            this->rot.z &= ~mask;
            this->rot.z |= val;
            break;
    }
}
