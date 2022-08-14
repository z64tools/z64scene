#include <Editor.h>

extern DataFile gCube;

#define gCubeDL    0x06000990
#define gCubeLodDL 0x06000BF0

void Actor_Draw(Actor* this, View3D* view) {
	Vec3f pos = Math_Vec3f_Add(Math_Vec3f_New(UnfoldVec3(this->pos)), this->offset);
	
	if (!View_PointInScreen(view, pos))
		return;
	
	Matrix_Push(); {
		Matrix_Translate(UnfoldVec3(pos), MTXMODE_APPLY);
		Matrix_Push(); {
			Matrix_Scale(0.01, 0.01, 0.01, MTXMODE_APPLY);
			Matrix_RotateY_s(this->rot.y, MTXMODE_APPLY);
			Matrix_RotateX_s(this->rot.x, MTXMODE_APPLY);
			Matrix_RotateZ_s(this->rot.z, MTXMODE_APPLY);
			
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
	ActorList* list = header->actorList;
	Actor* this = list->head;
	
	Assert(header != NULL);
	
	list = header->actorList;
	if (list == NULL)
		return;
	this = list->head;
	
	for (s32 i = 0; i < list->num; i++, this++)
		Actor_Draw(this, view);
}