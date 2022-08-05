#ifndef __EN_3D_VIEWPORT__
#define __EN_3D_VIEWPORT__

#include <Editor.h>

typedef struct {
	Vec3f pos;
	Vec3f ppos;
	Vec3f vel;
	u8    moveLock;
} Gizmo;

typedef struct {
	View3D    view;
	SkelAnime skelAnime;
	s8 headerClick;
	
	RayLine prevRay;
	RayLine rayLine;
	Vec3f   cubePos;
	
	Actor* curActor;
	
	u8 gfocus[3];
	u8 glock[3];
	
	Gizmo gizmo;
} EnViewport;

extern SplitTask gEnViewportTask;

#endif