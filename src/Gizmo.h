#ifndef GIZMO_H
#define GIZMO_H

#include <ext_type.h>
#include <ext_math.h>
#include <ext_input.h>
#include <ext_view.h>
#include "Types.h"

struct Gfx;
struct Scene;

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

typedef struct GizmoElem {
    struct GizmoElem* next;
    Vec3f pos;
    Vec3s rot;
    bool  selected;
    
    Vec3f* dpos;
    Vec3s* drot;
} GizmoElem;

typedef struct {
    Vec3f pos;
    Vec3f initpos;
    
    f32 degr;
    s16 pyaw;
    
    GizmoAxis   focus;
    GizmoAxis   lock;
    Cylinder    cyl[3];
    MtxF        mtx;
    Vec3f       pivotPos;
    GizmoAction action;
    struct {
        bool pressLock : 1;
        bool release   : 1;
        bool resetRot  : 1;
    };
    
    GizmoElem* elemHead;
    GizmoElem* activeElem;
} Gizmo;

void Gizmo_Draw(Gizmo* this, View3D* view, struct Gfx** disp);
void Gizmo_Update(Gizmo* this, View3D* view, Input* input, Vec3f* rayPos);
bool Gizmo_IsBusy(Gizmo* this);

void Gizmo_Select(Gizmo* this, GizmoElem* elem, Vec3f* pos, Vec3s* rot);
void Gizmo_UnselectAll(Gizmo* this);
void Gizmo_Unselect(Gizmo* this, GizmoElem* elem);

void Gizmo_ApplyTransforms(Gizmo* this);
void Gizmo_ResetTransforms(Gizmo* this);

void Gizmo_Focus(Gizmo* this, GizmoElem* elem);

#endif