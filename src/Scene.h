#ifndef __Z64_SCENE_H__
#define __Z64_SCENE_H__

#include <ext_lib.h>
#include <nano_grid.h>
#include <ext_collision.h>
#include "Types.h"
#include "BgCheck.h"
#include "Light.h"
#include "Actor.h"

typedef struct {
	Vec3f* point;
	u32    num;
} PathList;

typedef struct {
	u32  index;
	u32  frame;
	bool nightFlag;
	bool wait;
} AnimOoT;

typedef enum {
	ROOM_CACHE_BUILD = 1 << 0,
	ROOM_SELECTED    = 1 << 1,
} RoomState;

typedef enum {
	SCENE_DRAW_COLLISION = 1 << 0,
	SCENE_DRAW_FOG       = 1 << 1,
	SCENE_DRAW_CULLING   = 1 << 2,
} SceneState;

typedef struct RoomMesh {
	u32  uuid;
	bool initialized;
	
	struct {
		u32* opa;
		u32* xlu;
		u32  num;
	} disp;
	
	const char* name;
	u8*       segment;
	u32       sizeSegment;
	TriBuffer triBuf;
	
	Vec3f center;
	f32   size;
} RoomMesh;

typedef struct RoomHeader {
	const u32 id;
	RoomMesh* mesh;
	
	Arli actorList;
	Arli objectList;
	Arli roomLight;
	
	bool timeGlobal;
	u8   timeHour;
	u8   timeMinute;
	u8   timeSpeed;
	
	struct {
		s8 x, y, z;
		u8 strength;
	} wind;
	
	struct {
		u8   val1;
		u8   val2;
		bool lensMode;
		bool disableWarpSongs;
		bool bindPointlights;
	} behaviour;
	
	struct {
		bool disableSky;
		bool disableSunMoon;
	} skyBox;
	
	u8 echo;
} RoomHeader;

typedef struct SceneHeader {
	Arli spawnList;
	Arli pathList;
	Arli transitionList;
	Arli envList;
	Arli exitList;
	
	struct {
		u8 hour;
		u8 min;
		u8 speed;
	} time;
	
	struct {
		u8 specID;
		u8 bgmID;
		u8 sfxID;
	} sound;
	
	u8 skyId;
	u8 cloudState;
	u8 envState;
	
	u8  camType;
	u8  area;
	u8  naviMsg;
	u16 keepObject;
} SceneHeader;

typedef struct Room {
	u8* segment;
	
	const u32  id;
	RoomState  state;
	RoomHeader header[0x10];
} Room;

typedef struct Scene {
	u8* segment;
	u32 sizeSegment;
	
	SceneState  state;
	SceneHeader header[0x10];
	Room room[32];
	
	struct {
		ElCombo roomCombo;
		Arli    roomNameList;
		f32     glowFactor;
	} ui;
	
	u8 numHeader;
	u8 curHeader;
	
	u8 numRoom;
	u8 curRoom;
	
	Actor* curActor;
	Actor* prevActor[64];
	
	u32 colSegment;
	CollisionMesh colMesh;
	AnimOoT       animOoT;
	bool kill;
	
	struct {
		RoomMesh entry[32 * 4];
		u32      num;
		Vec3f    rayPos;
		bool     rayHit;
	} mesh;
	
	struct {
		int roomId;
		int headerId;
	} next;
	
	s8 headerInherit[0xF];
} Scene;

extern void (*sSceneDrawConfigs[53])(AnimOoT* this);

void Scene_Init(Scene* this);
void Scene_LoadScene(Scene* this, const char* file);
void Scene_LoadRoom(Scene* this, const char* file);
void Scene_Kill(Scene* this);
void Scene_Free(Scene* this);

void Scene_Update(Scene* this);
void Scene_ViewportUpdate(Scene* this, View3D* view);
void Scene_Draw(Scene* this, View3D* view);

void Scene_CacheBuild(Scene* this);

SceneHeader* Scene_GetCurSceneHeader(Scene* this);
SceneHeader* Scene_GetSceneHeader(Scene* this, u8 num);
RoomHeader* Scene_GetRoomHeader(Scene* this, u8 num);
void Scene_SetState(Scene* this, SceneState state, bool set);

void Scene_SetRoom(Scene* this, s32 roomID);

void Room_Draw(RoomMesh* roomMesh);

Room* Scene_RaycastRoom(Scene* scene, RayLine* ray, Vec3f* out);

#ifndef __clang__
#define Scene_GetCurSceneHeader(this)  ({ \
		_log("Scene_GetCurSceneHeader"); \
		Scene_GetCurSceneHeader(this); })
#define Scene_GetRoomHeader(this, num) ({ \
		_log("Scene_GetRoomHeader"); \
		Scene_GetRoomHeader(this, num); })
#endif

enum {
	SCENE_CMD_SPAWN_LIST            = 0x00,
	SCENE_CMD_ACTOR_LIST            = 0x01,
	SCENE_CMD_COLLISION_HEADER      = 0x03,
	SCENE_CMD_ROOM_LIST             = 0x04,
	SCENE_CMD_WIND_SETTINGS         = 0x05,
	SCENE_CMD_ENTRANCE_LIST         = 0x06,
	SCENE_CMD_SPECIAL_FILES         = 0x07,
	SCENE_CMD_ROOM_BEHAVIOR         = 0x08,
	SCENE_CMD_UNDEFINED_9           = 0x09,
	SCENE_CMD_MESH_HEADER           = 0x0A,
	SCENE_CMD_OBJECT_LIST           = 0x0B,
	SCENE_CMD_LIGHT_LIST            = 0x0C,
	SCENE_CMD_PATH_LIST             = 0x0D,
	SCENE_CMD_TRANSITION_ACTOR_LIST = 0x0E,
	SCENE_CMD_LIGHT_SETTINGS_LIST   = 0x0F,
	SCENE_CMD_TIME_SETTINGS         = 0x10,
	SCENE_CMD_SKYBOX_SETTINGS       = 0x11,
	SCENE_CMD_SKYBOX_DISABLES       = 0x12,
	SCENE_CMD_EXIT_LIST             = 0x13,
	SCENE_CMD_END                   = 0x14,
	SCENE_CMD_SOUND_SETTINGS        = 0x15,
	SCENE_CMD_ECHO_SETTINGS         = 0x16,
	SCENE_CMD_CUTSCENE_DATA         = 0x17,
	SCENE_CMD_ALTERNATE_HEADER_LIST = 0x18,
	SCENE_CMD_MISC_SETTINGS         = 0x19,
	SCENE_CMD_RENDER_INIT           = 0x1A,
	SCENE_CMD_MAX,
};

#endif /* __Z64_SCENE_H__ */
