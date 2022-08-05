#ifndef __Z64_ACTOR_H__
#define __Z64_ACTOR_H__
#include <ExtLib.h>
#include <ExtGui/Math.h>
#include <ExtGui/Collision.h>
#include "Types.h"

struct Room;

typedef struct {
	MemFile file;
	void32  segment;
} Object;

typedef struct StructBE {
	u16      id;
	Vec3s_BE pos, rot;
	u16      param;
} ActorEntry;

typedef enum {
	ACTOR_SELECTED = 1 << 0,
} ActorState;

typedef struct {
	u16   id;
	Vec3f pos;
	Vec3s rot;
	u16   param;
	
	Vec3f      offset;
	ActorState state;
	Sphere     sph;
} Actor;

typedef struct {
	DataNode data;
	Actor*   head;
	u32      num;
} ActorList;

void Actor_Draw(Actor* this);
void ActorList_Draw(ActorList* list);

#endif