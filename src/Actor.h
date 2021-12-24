#ifndef __ACTOR_H__
#define __ACTOR_H__
#include <Global.h>

typedef enum {
	/* 0x00 */ ACTORCAT_SWITCH,
	/* 0x01 */ ACTORCAT_BG,
	/* 0x02 */ ACTORCAT_PLAYER,
	/* 0x03 */ ACTORCAT_EXPLOSIVE,
	/* 0x04 */ ACTORCAT_NPC,
	/* 0x05 */ ACTORCAT_ENEMY,
	/* 0x06 */ ACTORCAT_PROP,
	/* 0x07 */ ACTORCAT_ITEMACTION,
	/* 0x08 */ ACTORCAT_MISC,
	/* 0x09 */ ACTORCAT_BOSS,
	/* 0x0A */ ACTORCAT_DOOR,
	/* 0x0B */ ACTORCAT_CHEST
} ActorCategory;

#define OBJECT_ID_MAX 0x0192
#define ACTOR_ID_MAX  0x0192

typedef struct {
	/* 0x00 */ u16 id;
	/* 0x02 */ u8  category;
	/* 0x04 */ u32 flags;
	/* 0x08 */ u16 objectId;
	/* 0x0C */ u32 instanceSize;
	/* 0x10 */ u32 init;
	/* 0x14 */ u32 destroy;
	/* 0x18 */ u32 update;
	/* 0x1C */ u32 draw;
} ActorInit; // size = 0x20

void Actor_GetActorInit();

#endif