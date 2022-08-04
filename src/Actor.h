#ifndef __Z64_ACTOR_H__
#define __Z64_ACTOR_H__
#include <ExtLib.h>
#include <ExtGui/Math.h>
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
} Actor;

typedef struct {
	DataNode data;
	Actor*   head;
	u32      num;
} ActorList;

void Actor_Draw(Actor* this);

#endif