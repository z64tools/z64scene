#ifndef __Z64_ACTOR_H__
#define __Z64_ACTOR_H__
#include <ExtLib.h>
#include <ExtGui/Math.h>
#include "Types.h"

typedef struct {
	MemFile file;
	void32  segment;
} Object;

typedef struct StructBE {
	u16   id;
	Vec3s pos, rot;
	u16   param;
} ActorEntry;

typedef struct Actor {
	struct Actor* next;
	u16     id;
	u16     param;
	Vec3s   pos;
	Vec3s   rot;
	Object* object;
} Actor;

#endif