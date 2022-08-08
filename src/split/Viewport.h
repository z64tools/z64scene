#ifndef __EN_3D_VIEWPORT__
#define __EN_3D_VIEWPORT__

#include <Editor.h>

typedef enum {
	GIZMO_AXIS_ALL_TRUE  = 0x00010101,
	GIZMO_AXIS_ALL_FALSE = 0x00000000,
} GizmoState;

typedef struct {
	union {
		struct {
			u8 x;
			u8 y;
			u8 z;
		};
		GizmoState state;
		u8  axis[3];
	};
} GizmoAxis;

typedef struct {
	Vec3f     pos;
	Vec3f     ppos;
	Vec3f     vel;
	u8        moveLock;
	GizmoAxis focus;
	GizmoAxis lock;
} Gizmo;

typedef struct {
	View3D    view;
	SkelAnime skelAnime;
	s8 headerClick;
	
	Vec3f  cubePos;
	Actor* curActor;
	
	Gizmo gizmo;
} Viewport;

extern SplitTask gViewportTask;

#endif