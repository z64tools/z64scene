#include <Editor.h>

#define INCBIN_PREFIX
#include <incbin.h>
INCBIN(gCube_, "assets/3D/Cube.zobj");

void Actor_Draw(Actor* this) {
	Matrix_Push();
	
	Matrix_Translate(UnfoldVec3(this->pos), MTXMODE_APPLY);
	Matrix_Push();
	
	Matrix_Scale(0.01, 0.01, 0.01, MTXMODE_APPLY);
	Matrix_RotateY_s(this->rot.y, MTXMODE_APPLY);
	Matrix_RotateX_s(this->rot.x, MTXMODE_APPLY);
	Matrix_RotateZ_s(this->rot.z, MTXMODE_APPLY);
	
	gSPSegment(POLY_OPA_DISP++, 6, (void*)gCube_Data);
	gDPSetEnvColor(POLY_OPA_DISP++, 0xFF, 0xFF, 0xFF, 0xFF);
	gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
	gXPOutline(POLY_OPA_DISP++, 0xFF, 0x40, 0x40, 0x80, 4);
	gSPDisplayList(POLY_OPA_DISP++, 0x06000820);
	gXPClearOutline(POLY_OPA_DISP++);
	
	Matrix_Pop();
	Matrix_Pop();
}