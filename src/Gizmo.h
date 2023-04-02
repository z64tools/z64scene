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
    GIZMO_AXIS_X         = 0x00010000,
    GIZMO_AXIS_Y         = 0x00000100,
    GIZMO_AXIS_Z         = 0x00000001,
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
    GIZMO_ACTION_MOVE = 1,
    GIZMO_ACTION_ROTATE,
} GizmoAction;

typedef struct GizmoElem {
    struct GizmoElem* next;
    Vec3f pos;
    Vec3s rot;
    s8    interact;
    s8    selected;
    s8    focus;
    s8    refresh;
    
    Vec3f* dpos;
    Vec3s* drot;
} GizmoElem;

typedef enum {
    GIZMO_ORIENTATION_GLOBAL,
    GIZMO_ORIENTATION_LOCAL,
    GIZMO_ORIENTATION_VIEW,
} GizmoOrientation;

typedef struct {
    Vec3f pos;
    Vec3f initpos;
    
    Vec3s rot;
    s16   prev_yaw;
    
    bool initAction;
    
    GizmoAxis        focus;
    GizmoAxis        lock;
    Cylinder         cyl[3];
    MtxF             mtx;
    Vec3f            pivotPos;
    GizmoAction      action;
    GizmoOrientation orientation;
    struct {
        int  release   : 3;
        bool typing    : 1;
        bool trackball : 1;
        bool pressHold : 1;
    };
    char typed[20];
    f32  value;
    
    GizmoElem* elemHead;
    GizmoElem* activeElem;
    
    int refreshNodes;
    int resetTransforms;
    int refreshTransforms;
} Gizmo;

void Gizmo_Draw(Gizmo* this, View3D* view, struct Gfx** disp);
void Gizmo_Update(Gizmo* this, Input* input);
void Gizmo_ViewportUpdate(Gizmo* this, View3D* view, Input* input, Vec3f* rayPos);
bool Gizmo_IsBusy(Gizmo* this);

void Gizmo_Select(Gizmo* this, GizmoElem* elem, Vec3f* pos, Vec3s* rot);
void Gizmo_UnselectAll(Gizmo* this);
void Gizmo_Unselect(Gizmo* this, GizmoElem* elem);

void Gizmo_ApplyTransforms(Gizmo* this);
void Gizmo_ResetTransforms(Gizmo* this);

void Gizmo_Focus(Gizmo* this, GizmoElem* elem);

#endif