#include <ExtTypes.h>
#include <ExtGui/Math.h>
#include <ExtGui/Input.h>
#include <ExtGui/View.h>
#include <Scene.h>

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
		u8 axis[3];
	};
} GizmoAxis;

typedef enum {
	GIZMO_ACTION_NULL,
	GIZMO_ACTION_MOVE,
	GIZMO_ACTION_ROTATE,
} GizmoAction;

typedef struct {
	Vec3f pos;
	Vec3f ppos;
	Vec3f vel;
	Vec3f initpos;
	
	f64 degr;
	s16 pyaw;
	
	GizmoAxis focus;
	GizmoAxis lock;
	Cylinder  cyl[3];
	
	MtxF mtx;
	
	bool pressLock;
	bool release;
	bool resetRot;
	GizmoAction action;
} Gizmo;

void Gizmo_Draw(Gizmo* this, View3D* view, Gfx** disp);
void Gizmo_Update(Gizmo* this, Scene* scene, View3D* view, Input* input);
bool Gizmo_IsBusy(Gizmo* this);
