#include <Editor.h>

#define INCBIN_PREFIX
#include <incbin.h>
INCBIN(gCube_, "assets/3D/Cube.zobj");

void Actor_Draw(Actor* this) {
	Vec3f pos = Math_Vec3f_Add(Math_Vec3f_New(UnfoldVec3(this->pos)), this->offset);
	
	Matrix_Push(); {
		Matrix_Translate(UnfoldVec3(pos), MTXMODE_APPLY);
		Matrix_Push(); {
			Matrix_Scale(0.01, 0.01, 0.01, MTXMODE_APPLY);
			Matrix_RotateY_s(this->rot.y, MTXMODE_APPLY);
			Matrix_RotateX_s(this->rot.x, MTXMODE_APPLY);
			Matrix_RotateZ_s(this->rot.z, MTXMODE_APPLY);
			
			gSPSegment(POLY_OPA_DISP++, 6, (void*)gCube_Data);
			
			if (this->state & ACTOR_SELECTED)
				gDPSetEnvColor(POLY_OPA_DISP++, 0x40, 0x40, 0xFF, 0xFF);
			else
				gDPSetEnvColor(POLY_OPA_DISP++, 0xC0, 0xC0, 0xC0, 0xFF);
			
			gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
			gSPDisplayList(POLY_OPA_DISP++, 0x06000820);
		} Matrix_Pop();
	} Matrix_Pop();
}

void ActorList_Draw(ActorList* list) {
	if (list == NULL)
		return;
	Actor* this = list->head;
	
	for (s32 i = 0; i < list->num; i++, this++)
		Actor_Draw(this);
}