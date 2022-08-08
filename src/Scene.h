#ifndef __Z64_SCENE_H__
#define __Z64_SCENE_H__

#include <ExtLib.h>
#include <ExtGui/GeoGrid.h>
#include <ExtGui/Collision.h>
#include "Types.h"
#include "BgCheck.h"
#include "Light.h"
#include "Actor.h"

typedef enum {
	ROOM_CACHE_BUILD = 1 << 0,
	ROOM_SELECTED    = 1 << 1,
} RoomState;

typedef struct RoomMesh {
	DataNode  data;
	void*     header;
	void*     roomFile;
	TriBuffer triBuf;
} RoomMesh;

typedef struct RoomHeader {
	RoomMesh*  mesh;
	ActorList* actorList;
	u8 indoorLight;
} RoomHeader;

typedef struct Room {
	MemFile file;
	void*   segment;
	
	RoomState  state;
	RoomHeader header[0x10];
} Room;

typedef enum {
	SCENE_DRAW_COLLISION = 1 << 0,
	SCENE_DRAW_FOG       = 1 << 1,
	SCENE_DRAW_CULLING   = 1 << 2,
} SceneState;

typedef struct {
	LightList* lightList;
} SceneHeader;

typedef struct {
	MemFile file;
	void*   segment;
	
	SceneHeader header[0x10];
	u8   numHeader;
	u8   curHeader;
	Room room[255];
	u8   numRoom;
	u8   curRoom;
	u8   curEnv;
	
	SceneState state;
	
	DataContext   dataCtx;
	CollisionMesh colMesh;
} Scene;

void Scene_LoadScene(Scene* this, const char* file);
void Scene_LoadRoom(Scene* this, const char* file);
void Scene_Free(Scene* this);
void Scene_ExecuteCommands(Scene* this, Room* room);
void Scene_Draw(Scene* this);
void Scene_CacheBuild(Scene* this);

SceneHeader* Scene_GetSceneHeader(Scene* this);
RoomHeader* Scene_GetRoomHeader(Scene* this, u8 num);
void Scene_SetState(Scene* this, SceneState state, bool set);

void Room_Draw(RoomMesh* roomMesh);

#ifndef __NO_SCENE_TYPES_H__

typedef struct StructBE {
	/* 0x00 */ u32 vromStart;
	/* 0x04 */ u32 vromEnd;
} RomFile; // size = 0x8

typedef struct StructBE {
	/* 0x00 */ RomFile sceneFile;
	/* 0x08 */ RomFile titleFile;
	/* 0x10 */ u8      unk_10;
	/* 0x11 */ u8      drawConfig;
	/* 0x12 */ u8      unk_12;
	/* 0x13 */ u8      unk_13;
} SceneTableEntry; // size = 0x14

typedef struct StructBE {
	/* 0x00 */ u8  code;
	/* 0x01 */ u8  data1;
	/* 0x04 */ u32 data2;
} SCmdBase;

typedef struct StructBE {
	/* 0x00 */ u8     code;
	/* 0x01 */ u8     data1;
	/* 0x04 */ void32 segment;
} SCmdSpawnList;

typedef struct StructBE {
	/* 0x00 */ u8     code;
	/* 0x01 */ u8     num;
	/* 0x04 */ void32 segment;
} SCmdActorList;

typedef struct StructBE {
	/* 0x00 */ u8     code;
	/* 0x01 */ u8     data1;
	/* 0x04 */ void32 segment;
} SCmdUnused02;

typedef struct StructBE {
	/* 0x00 */ u8     code;
	/* 0x01 */ u8     data1;
	/* 0x04 */ void32 segment;
} SCmdColHeader;

typedef struct StructBE {
	/* 0x00 */ u8     code;
	/* 0x01 */ u8     num;
	/* 0x04 */ void32 segment;
} SCmdRoomList;

typedef struct StructBE {
	/* 0x00 */ u8   code;
	/* 0x01 */ u8   data1;
	/* 0x02 */ char pad[2];
	/* 0x04 */ u8   x;
	/* 0x05 */ u8   y;
	/* 0x06 */ u8   z;
	/* 0x07 */ u8   unk_07;
} SCmdWindSettings;

typedef struct StructBE {
	/* 0x00 */ u8     code;
	/* 0x01 */ u8     data1;
	/* 0x04 */ void32 segment;
} SCmdEntranceList;

typedef struct StructBE {
	/* 0x00 */ u8  code;
	/* 0x01 */ u8  cUpElfMsgNum;
	/* 0x04 */ u32 keepObjectId;
} SCmdSpecialFiles;

typedef struct StructBE {
	/* 0x00 */ u8  code;
	/* 0x01 */ u8  gpFlag1;
	/* 0x04 */ u32 gpFlag2;
} SCmdRoomBehavior;

typedef struct StructBE {
	/* 0x00 */ u8     code;
	/* 0x01 */ u8     data1;
	/* 0x04 */ void32 segment;
} SCmdMesh;

typedef struct StructBE {
	/* 0x00 */ u8     code;
	/* 0x01 */ u8     num;
	/* 0x04 */ void32 segment;
} SCmdObjectList;

typedef struct StructBE {
	/* 0x00 */ u8     code;
	/* 0x01 */ u8     num;
	/* 0x04 */ void32 segment;
} SCmdLightList;

typedef struct StructBE {
	/* 0x00 */ u8     code;
	/* 0x01 */ u8     data1;
	/* 0x04 */ void32 segment;
} SCmdPathList;

typedef struct StructBE {
	/* 0x00 */ u8     code;
	/* 0x01 */ u8     num;
	/* 0x04 */ void32 segment;
} SCmdTransiActorList;

typedef struct StructBE {
	/* 0x00 */ u8     code;
	/* 0x01 */ u8     num;
	/* 0x04 */ void32 segment;
} SCmdLightSettingList;

typedef struct StructBE {
	/* 0x00 */ u8   code;
	/* 0x01 */ u8   data1;
	/* 0x02 */ char pad[2];
	/* 0x04 */ u8   hour;
	/* 0x05 */ u8   min;
	/* 0x06 */ u8   unk_06;
} SCmdTimeSettings;

typedef struct StructBE {
	/* 0x00 */ u8   code;
	/* 0x01 */ u8   data1;
	/* 0x02 */ char pad[2];
	/* 0x04 */ u8   skyboxId;
	/* 0x05 */ u8   unk_05;
	/* 0x06 */ u8   unk_06;
} SCmdSkyboxSettings;

typedef struct StructBE {
	/* 0x00 */ u8   code;
	/* 0x01 */ u8   data1;
	/* 0x02 */ char pad[2];
	/* 0x04 */ u8   unk_04;
	/* 0x05 */ u8   unk_05;
} SCmdSkyboxDisables;

typedef struct StructBE {
	/* 0x00 */ u8  code;
	/* 0x01 */ u8  data1;
	/* 0x04 */ u32 data2;
} SCmdEndMarker;

typedef struct StructBE {
	/* 0x00 */ u8     code;
	/* 0x01 */ u8     data1;
	/* 0x04 */ void32 segment;
} SCmdExitList;

typedef struct StructBE {
	/* 0x00 */ u8   code;
	/* 0x01 */ u8   specId;
	/* 0x02 */ char pad[4];
	/* 0x06 */ u8   natureAmbienceId;
	/* 0x07 */ u8   seqId;
} SCmdSoundSettings;

typedef struct StructBE {
	/* 0x00 */ u8   code;
	/* 0x01 */ u8   data1;
	/* 0x02 */ char pad[5];
	/* 0x07 */ u8   echo;
} SCmdEchoSettings;

typedef struct StructBE {
	/* 0x00 */ u8     code;
	/* 0x01 */ u8     data1;
	/* 0x04 */ void32 segment;
} SCmdCutsceneData;

typedef struct StructBE {
	/* 0x00 */ u8     code;
	/* 0x01 */ u8     data1;
	/* 0x04 */ void32 segment;
} SCmdAltHeaders;

typedef struct StructBE {
	/* 0x00 */ u8  code;
	/* 0x01 */ u8  cameraMovement;
	/* 0x04 */ u32 area;
} SCmdMiscSettings;

typedef struct StructBE {
	u8 headerType;
} MeshHeaderBase;

typedef struct StructBE {
	MeshHeaderBase base;
	u8     numEntries;
	void32 dListStart;
	void32 dListEnd;
} MeshHeader0;

typedef struct StructBE {
	void32 opaqueDList;
	void32 translucentDList;
} MeshEntry0;

typedef struct StructBE {
	MeshHeaderBase base;
	u8  format;
	u32 entryRecord;
} MeshHeader1Base;

typedef struct StructBE {
	MeshHeader1Base base;
	void32 imagePtr;       // 0x08
	u32    unknown;        // 0x0C
	u32    unknown2;       // 0x10
	u16    bgWidth;        // 0x14
	u16    bgHeight;       // 0x16
	u8     imageFormat;    // 0x18
	u8     imageSize;      // 0x19
	u16    imagePal;       // 0x1A
	u16    imageFlip;      // 0x1C
} MeshHeader1Single;

typedef struct StructBE {
	MeshHeader1Base base;
	u8     bgCnt;
	void32 bgRecordPtr;
} MeshHeader1Multi;

typedef struct StructBE {
	u16    unknown;        // 0x00
	s8     bgID;           // 0x02
	void32 imagePtr; // 0x04
	u32    unknown2;       // 0x08
	u32    unknown3;       // 0x0C
	u16    bgWidth;        // 0x10
	u16    bgHeight;       // 0x12
	u8     imageFmt;       // 0x14
	u8     imageSize;      // 0x15
	u16    imagePal;       // 0x16
	u16    imageFlip;      // 0x18
} BackgroundRecord;

typedef struct StructBE {
	s16    playerXMax, playerZMax;
	s16    playerXMin, playerZMin;
	void32 opaqueDList;
	void32 translucentDList;
} MeshEntry2;

typedef struct StructBE {
	MeshHeaderBase base;
	u8     numEntries;
	void32 dListStart;
	void32 dListEnd;
} MeshHeader2;

typedef struct StructBE {
	/* 0x00 */ u8  ambientColor[3];
	/* 0x03 */ s8  diffuseDir1[3];
	/* 0x06 */ u8  diffuseColor1[3];
	/* 0x09 */ s8  diffuseDir2[3];
	/* 0x0C */ u8  diffuseColor2[3];
	/* 0x0F */ u8  fogColor[3];
	/* 0x12 */ u16 fogNear;
	/* 0x14 */ u16 fogFar;
} LightSettings; // size = 0x16

typedef struct StructBE {
	/* 0x00 */ u8     count; // number of points in the path
	/* 0x04 */ void32 points; // Segment Address to the array of points
} PointPath; // size = 0x8

typedef union StructBE {
	SCmdBase             base;
	SCmdSpawnList        spawnList;
	SCmdActorList        actorList;
	SCmdUnused02         unused02;
	SCmdRoomList         roomList;
	SCmdEntranceList     entranceList;
	SCmdObjectList       objectList;
	SCmdLightList        lightList;
	SCmdPathList         pathList;
	SCmdTransiActorList  transiActorList;
	SCmdLightSettingList lightSettingList;
	SCmdExitList         exitList;
	SCmdColHeader        colHeader;
	SCmdMesh             mesh;
	SCmdSpecialFiles     specialFiles;
	SCmdCutsceneData     cutsceneData;
	SCmdRoomBehavior     roomBehavior;
	SCmdWindSettings     windSettings;
	SCmdTimeSettings     timeSettings;
	SCmdSkyboxSettings   skyboxSettings;
	SCmdSkyboxDisables   skyboxDisables;
	SCmdEndMarker        endMarker;
	SCmdSoundSettings    soundSettings;
	SCmdEchoSettings     echoSettings;
	SCmdMiscSettings     miscSettings;
	SCmdAltHeaders       altHeaders;
} SceneCmd; // size = 0x8

typedef enum {
	/* 0x00 */ SCENE_CMD_ID_SPAWN_LIST,
	/* 0x01 */ SCENE_CMD_ID_ACTOR_LIST,
	/* 0x02 */ SCENE_CMD_ID_UNUSED_2,
	/* 0x03 */ SCENE_CMD_ID_COLLISION_HEADER,
	/* 0x04 */ SCENE_CMD_ID_ROOM_LIST,
	/* 0x05 */ SCENE_CMD_ID_WIND_SETTINGS,
	/* 0x06 */ SCENE_CMD_ID_ENTRANCE_LIST,
	/* 0x07 */ SCENE_CMD_ID_SPECIAL_FILES,
	/* 0x08 */ SCENE_CMD_ID_ROOM_BEHAVIOR,
	/* 0x09 */ SCENE_CMD_ID_UNDEFINED_9,
	/* 0x0A */ SCENE_CMD_ID_MESH_HEADER,
	/* 0x0B */ SCENE_CMD_ID_OBJECT_LIST,
	/* 0x0C */ SCENE_CMD_ID_LIGHT_LIST,
	/* 0x0D */ SCENE_CMD_ID_PATH_LIST,
	/* 0x0E */ SCENE_CMD_ID_TRANSITION_ACTOR_LIST,
	/* 0x0F */ SCENE_CMD_ID_LIGHT_SETTINGS_LIST,
	/* 0x10 */ SCENE_CMD_ID_TIME_SETTINGS,
	/* 0x11 */ SCENE_CMD_ID_SKYBOX_SETTINGS,
	/* 0x12 */ SCENE_CMD_ID_SKYBOX_DISABLES,
	/* 0x13 */ SCENE_CMD_ID_EXIT_LIST,
	/* 0x14 */ SCENE_CMD_ID_END,
	/* 0x15 */ SCENE_CMD_ID_SOUND_SETTINGS,
	/* 0x16 */ SCENE_CMD_ID_ECHO_SETTINGS,
	/* 0x17 */ SCENE_CMD_ID_CUTSCENE_DATA,
	/* 0x18 */ SCENE_CMD_ID_ALTERNATE_HEADER_LIST,
	/* 0x19 */ SCENE_CMD_ID_MISC_SETTINGS,
	/* 0x20 */ SCENE_CMD_ID_MAX
} SceneCommandTypeID;

typedef struct StructBE {
	/* 0x00 */ void32 opa;
	/* 0x04 */ void32 xlu;
} PolygonDlist; // size = 0x8

typedef struct StructBE {
	/* 0x00 */ u8 type;
} PolygonBase;

typedef struct StructBE {
	/* 0x00 */ PolygonBase base;
	/* 0x01 */ u8     num; // number of dlist entries
	/* 0x04 */ void32 start;
	/* 0x08 */ void32 end;
} PolygonType0; // size = 0xC

typedef struct StructBE {
	/* 0x00 */ Vec3s_BE pos;
	/* 0x06 */ s16      unk_06;
	/* 0x08 */ void32   opa;
	/* 0x0C */ void32   xlu;
} PolygonDlist2; // size = 0x8

typedef struct StructBE {
	/* 0x00 */ PolygonBase base;
	/* 0x01 */ u8     num; // number of dlist entries
	/* 0x04 */ void32 start;
	/* 0x08 */ void32 end;
} PolygonType2; // size = 0xC

typedef union StructBE {
	PolygonBase  base;
	PolygonType0 polygon0;
	PolygonType2 polygon2;
} MeshHeader; // "Ground Shape"

#endif /* __FOLD_TYPES__ */
#endif /* __Z64_SCENE_H__ */