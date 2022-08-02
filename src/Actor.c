#include <Editor.h>

#define INCBIN_PREFIX
#include <incbin.h>
INCBIN(gCube_, "assets/3D/Cube.zobj");

Actor* Actor_New(Room* room, u16 id, u16 param, Vec3s pos, Vec3s rot) {
	Actor* this;
	
	CallocX(this);
	this->id = id;
	this->param = param;
	this->pos = pos;
	this->rot = rot;
	Node_Add(room->actorHead, this);
	
	return this;
}

void Actor_RemoveNodeList(Room* room) {
	while (room->actorHead)
		Node_Kill(room->actorHead, room->actorHead);
}

static void Actor_Draw(Actor* this) {
	Matrix_Push();
	
	Matrix_Translate(UnfoldVec3(this->pos), MTXMODE_APPLY);
	Matrix_Push();
	
	Matrix_Scale(this->scale, this->scale, this->scale, MTXMODE_APPLY);
	Matrix_RotateY_s(this->rot.y, MTXMODE_APPLY);
	Matrix_RotateX_s(this->rot.x, MTXMODE_APPLY);
	Matrix_RotateZ_s(this->rot.z, MTXMODE_APPLY);
	
	gSPSegment(POLY_OPA_DISP++, 6, (void*)gCube_Data);
	gDPSetEnvColor(POLY_OPA_DISP++, 0xFF, 0xFF, 0xFF, 0xFF);
	gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
	// gDPExtras(POLY_OPA_DISP++, 0, GX_OUTLINE);
	gSPDisplayList(POLY_OPA_DISP++, 0x06000238);
	// gDPExtras(POLY_OPA_DISP++, GX_OUTLINE, 0);
	
	Matrix_Pop();
	Matrix_Pop();
}

void Actor_UpdateAll(Room* room) {
	Actor* this = room->actorHead;
	
	while (this) {
		this->scale = 0.01f;
		
		Actor_Draw(this);
		
		this = this->next;
	}
}