#ifndef __Z64_ACTOR_H__
#define __Z64_ACTOR_H__
#include <ext_lib.h>
#include <ext_math.h>
#include <ext_collision.h>
#include <ext_view.h>
#include "Types.h"
#include "Gizmo.h"
#include "Database.h"
#include "SkelAnime.h"

struct Room;
struct RoomHeader;

typedef struct {
	Memfile file;
	u32     segment;
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

typedef struct Actor {
	u16   id;
	Vec3f pos;
	Vec3s rot;
	u16   param;
	
	ActorState state;
	Sphere     sph;
	
	GizmoElem gizmo;
	
	SkelAnime skelAnime;
	DbRender* render;
	
	u32 renderUUID;
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

void Actor_Draw_RoomHeader(struct RoomHeader*, View3D* view);

void Actor_Focus(struct Scene* scene, Actor* this);
void Actor_Select(struct Scene* scene, Actor* this);
void Actor_Unselect(struct Scene* scene, Actor* this);
void Actor_UnselectAll(struct Scene* scene, struct RoomHeader* hdr);

u16 Actor_rmask(Actor* this, DbSource source, u16 mask);
void Actor_wmask(Actor* this, DbSource source, u16 value, u16 mask);

#endif
