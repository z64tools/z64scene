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
	u16 id;
	Vec3s_BE pos, rot;
	u16 param;
} ActorEntry;

typedef struct Actor {
	struct Actor* next;
	const char*   name;
	u16     id;
	u16     param;
	Vec3s   pos;
	Vec3s   rot;
	Object* object;
	f32     scale;
} Actor;

Actor* Actor_New(struct Room* room, u16 id, u16 param, Vec3s pos, Vec3s rot);
void Actor_RemoveNodeList(struct Room* room);
void Actor_UpdateAll(struct Room* room);

#endif