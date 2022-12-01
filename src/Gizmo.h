#include <ext_type.h>
#include <ext_math.h>
#include <ext_input.h>
#include <ext_view.h>
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
    
    f32 degr;
    s16 pyaw;
    
    GizmoAxis   focus;
    GizmoAxis   lock;
    Cylinder    cyl[3];
    MtxF        mtx;
    GizmoAction action;
    struct {
        bool pressLock : 1;
        bool release   : 1;
        bool resetRot  : 1;
    };
} Gizmo;

void Gizmo_Draw(Gizmo* this, View3D* view, Gfx** disp);
void Gizmo_Update(Gizmo* this, Scene* scene, View3D* view, Input* input);
bool Gizmo_IsBusy(Gizmo* this);
