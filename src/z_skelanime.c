#include "z_skelanime.h"
#include <n64.h>

static u32 gS;

void SkelAnime_Init(MemFile* memFile, SkelAnime* this, u32 skeleton, u32 animation) {
	this->memFile = memFile;
	
	Log("%08X", skeleton);
	SkeletonHeader* skel = SEGMENTED_TO_VIRTUAL(skeleton);
	
	Log("%08X", skel);
	
	this->skeleton = skeleton;
	this->animation = animation;
	this->limbCount = skel->limbCount + 1;
	Calloc(this->jointTable, this->limbCount * sizeof(Vec3s));
	Calloc(this->morphTable, this->limbCount * sizeof(Vec3s));
	
	printf_info_align("skeleton", "%08X", skeleton);
	printf_info_align("jointTable", "%08X", this->jointTable);
	printf_info_align("morphTable", "%08X", this->morphTable);
	printf_info_align("animation", "%08X", animation);
	printf_info_align("limbCount", "%08X", skel->limbCount + 1);
}

void SkelAnime_Free(SkelAnime* this) {
	Free(this->jointTable);
	Free(this->morphTable);
}

void SkelAnime_GetFrameData(u32 animation, s32 frame, s32 limbCount, Vec3s* frameTable) {
	AnimationHeader animHeader = *((AnimationHeader*)SEGMENTED_TO_VIRTUAL(animation));
	
	SwapBE(animHeader.jointIndices);
	SwapBE(animHeader.frameData);
	SwapBE(animHeader.common.frameCount);
	SwapBE(animHeader.staticIndexMax);
	
	JointIndex* jointIndices = SEGMENTED_TO_VIRTUAL(animHeader.jointIndices);
	s16* frameData = SEGMENTED_TO_VIRTUAL(animHeader.frameData);
	s16* staticData = &frameData[0];
	s16* dynamicData = &frameData[frame];
	u16 staticIndexMax = animHeader.staticIndexMax;
	s32 i;
	
	for (i = 0; i < limbCount; i++, frameTable++, jointIndices++) {
		Vec3s swapInd = {
			jointIndices->x,
			jointIndices->y,
			jointIndices->z
		};
		SwapBE(swapInd.x);
		SwapBE(swapInd.y);
		SwapBE(swapInd.z);
		frameTable->x =
			(swapInd.x >= staticIndexMax) ? dynamicData[swapInd.x] : staticData[swapInd.x];
		frameTable->y =
			(swapInd.y >= staticIndexMax) ? dynamicData[swapInd.y] : staticData[swapInd.y];
		frameTable->z =
			(swapInd.z >= staticIndexMax) ? dynamicData[swapInd.z] : staticData[swapInd.z];
		
		SwapBE(frameTable->x);
		SwapBE(frameTable->y);
		SwapBE(frameTable->z);
	}
}

void SkelAnime_InterpFrameTable(s32 limbCount, Vec3s* dst, Vec3s* start, Vec3s* target, f32 weight) {
	s32 i;
	s16 diff;
	s16 base;
	
	if (weight < 1.0f) {
		for (i = 0; i < limbCount; i++, dst++, start++, target++) {
			base = start->x;
			diff = target->x - base;
			dst->x = (s16)(diff * weight) + base;
			base = start->y;
			diff = target->y - base;
			dst->y = (s16)(diff * weight) + base;
			base = start->z;
			diff = target->z - base;
			dst->z = (s16)(diff * weight) + base;
		}
	} else {
		for (i = 0; i < limbCount; i++, dst++, target++) {
			dst->x = target->x;
			dst->y = target->y;
			dst->z = target->z;
		}
	}
}

void SkelAnime_Update(SkelAnime* this) {
	AnimationHeader animHeader = *((AnimationHeader*)SEGMENTED_TO_VIRTUAL(this->animation));
	
	SwapBE(animHeader.jointIndices);
	SwapBE(animHeader.frameData);
	SwapBE(animHeader.common.frameCount);
	SwapBE(animHeader.staticIndexMax);
	
	this->endFrame = animHeader.common.frameCount - 1;
	SkelAnime_GetFrameData(this->animation, floor(this->curFrame), this->limbCount, this->jointTable);
	SkelAnime_GetFrameData(this->animation, WrapF(floor(this->curFrame) + 1, 0, this->endFrame), this->limbCount, this->morphTable);
	
	SkelAnime_InterpFrameTable(
		this->limbCount,
		this->jointTable,
		this->jointTable,
		this->morphTable,
		fmod(this->curFrame, 1.0f)
	);
	
	if (this->curFrame < this->endFrame) {
		this->curFrame += this->playSpeed;
	} else {
		this->curFrame = 0;
	}
	this->prevFrame = this->curFrame;
}

void SkelAnime_Limb(u32 skelSeg, u8 limbId, Mtx** mtx, Vec3s* jointTable) {
	StandardLimb* limb;
	u32* limbList;
	Vec3s rot = { 0 };
	Vec3s pos;
	Vec3f rpos;
	
	limbList = SEGMENTED_TO_VIRTUAL(skelSeg);
	limb = SEGMENTED_TO_VIRTUAL(ReadBE(limbList[limbId]));
	
	Matrix_Push();
	
	if (limbId == 0) {
		Vec3_Copy(pos, jointTable[0]);
		Vec3_Copy(rot, jointTable[1]);
	} else {
		Vec3_Copy(pos, limb->jointPos);
		
		for (s32 i = 0; i < 3; i++)
			SwapBE(pos.axis[i]);
		
		limbId++;
		Vec3_Copy(rot, jointTable[limbId]);
	}
	
	Vec3_Copy(rpos, pos);
	
	Matrix_TranslateRotateZYX(&rpos, &rot);
	
	if (limb->dList) {
		if (mtx && *mtx) {
			Matrix_ToMtx((*mtx));
			gSPMatrix(POLY_OPA_DISP++, (*mtx), G_MTX_LOAD);
			(*mtx)++;
		}
		gSPDisplayList(POLY_OPA_DISP++, ReadBE(limb->dList));
	}
	
	limbList++;
	
	if (limb->child != 0xFF)
		SkelAnime_Limb(skelSeg, limb->child, mtx, jointTable);
	
	Matrix_Pop();
	
	if (limb->sibling != 0xFF)
		SkelAnime_Limb(skelSeg, limb->sibling, mtx, jointTable);
}

void SkelAnime_Draw(SkelAnime* this, SkelanimeType type) {
	SkeletonHeader* skel;
	Mtx* mtx = NULL;
	
	n64_set_onlyGeoLayer(GEOLAYER_ALL);
	
	Matrix_Push();
	skel = SEGMENTED_TO_VIRTUAL(this->skeleton);
	if (type == SKELANIME_FLEX) {
		mtx = n64_graph_alloc(sizeof(Mtx) * skel->limbCount);
		gxSPSegment(POLY_OPA_DISP++, 0x0D, mtx);
	}
	
	SkelAnime_Limb(ReadBE(skel->segment), 0, &mtx, this->jointTable);
	Matrix_Pop();
	
	gS = 1;
}
