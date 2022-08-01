#ifndef __Z64_BGCHECK_H__
#define __Z64_BGCHECK_H__

#include <ExtLib.h>
#include "Types.h"
#include "n64.h"

typedef struct {
	VtxS_BE* vtxBuf;
	Gfx* tri;
} CollisionMesh;

typedef struct StructBE {
	u32 data[2];
} SurfaceType;

typedef struct StructBE {
	u16 type;
	union StructBE {
		u16 vtxData[3];
		struct StructBE {
			u16 flags_vIA;
			u16 flags_vIB;
			
			u16 vIC;
		};
	};
	Vec3s_BE normal;
	
	s16 dist;
} CollisionPoly;

typedef struct StructBE {
	Vec3s_BE       minBounds;
	Vec3s_BE       maxBounds;
	u16 numVertices;
	void32         vtxList32;
	u16 numPolygons;
	void32         polyList32;
	void32         surfaceTypeList32;
	void32         bgCamList32;
	u16 numWaterBoxes;
	void32         waterBoxe32s;
	
	Vec3s_BE*      vtxList;
	CollisionPoly* polyList;
	SurfaceType*   surfaceTypeList;
	//BgCamInfo* bgCamListP;
	//WaterBox* waterBoxesP;
} CollisionHeader;

void CollisionMesh_Generate(CollisionHeader* colHeader, CollisionMesh* mesh);
void CollisionMesh_Free(CollisionMesh* mesh);
void CollisionMesh_Draw(CollisionMesh* mesh);

#endif