#ifndef __Z64_ACTOR_H__
#define __Z64_ACTOR_H__
#include <ext_lib.h>
#include <ext_math.h>
#include <ext_collision.h>
#include <ext_view.h>
#include "Types.h"

struct Room;
struct RoomHeader;

typedef struct {
    MemFile file;
    void32  segment;
} Object;

typedef struct StructBE {
    u16      id;
    Vec3s_BE pos;
    Vec3s_BE rot;
    u16      param;
} ActorEntry;

typedef enum {
    ACTOR_SELECTED = 1 << 0,
} ActorState;

typedef struct {
    u16   id;
    Vec3f pos;
    Vec3s rot;
    Vec3s orot;
    u16   param;
    
    Vec3f      offset;
    ActorState state;
    Sphere     sph;
} Actor;

typedef struct {
    Actor actor;
    u8    spawn;
    u8    room;
} SpawnActor;

typedef struct {
    Actor actor;
    struct {
        u8 room;
        u8 cam;
    } side[2];
} TransitionActor;

void Actor_Draw(Actor* this, View3D* view);
void Actor_Draw_RoomHeader(struct RoomHeader*, View3D* view);

#endif