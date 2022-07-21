#include "Editor.h"

#ifndef __FOLD_TYPES__

typedef struct StructBE {
	/* 0x00 */ u32 vromStart;
	/* 0x04 */ u32 vromEnd;
} RomFile; // size = 0x8

typedef struct StructBE {
	/* 0x00 */ RomFile sceneFile;
	/* 0x08 */ RomFile titleFile;
	/* 0x10 */ u8 unk_10;
	/* 0x11 */ u8 drawConfig;
	/* 0x12 */ u8 unk_12;
	/* 0x13 */ u8 unk_13;
} SceneTableEntry; // size = 0x14

typedef struct StructBE {
	/* 0x00 */ u8  code;
	/* 0x01 */ u8  data1;
	/* 0x04 */ u32 data2;
} SCmdBase;

typedef struct StructBE {
	/* 0x00 */ u8 code;
	/* 0x01 */ u8 data1;
	/* 0x04 */ void32 segment;
} SCmdSpawnList;

typedef struct StructBE {
	/* 0x00 */ u8 code;
	/* 0x01 */ u8 num;
	/* 0x04 */ void32 segment;
} SCmdActorList;

typedef struct StructBE {
	/* 0x00 */ u8 code;
	/* 0x01 */ u8 data1;
	/* 0x04 */ void32 segment;
} SCmdUnused02;

typedef struct StructBE {
	/* 0x00 */ u8 code;
	/* 0x01 */ u8 data1;
	/* 0x04 */ void32 segment;
} SCmdColHeader;

typedef struct StructBE {
	/* 0x00 */ u8 code;
	/* 0x01 */ u8 num;
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
	/* 0x00 */ u8 code;
	/* 0x01 */ u8 data1;
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
	/* 0x00 */ u8 code;
	/* 0x01 */ u8 data1;
	/* 0x04 */ void32 segment;
} SCmdMesh;

typedef struct StructBE {
	/* 0x00 */ u8 code;
	/* 0x01 */ u8 num;
	/* 0x04 */ void32 segment;
} SCmdObjectList;

typedef struct StructBE {
	/* 0x00 */ u8 code;
	/* 0x01 */ u8 num;
	/* 0x04 */ void32 segment;
} SCmdLightList;

typedef struct StructBE {
	/* 0x00 */ u8 code;
	/* 0x01 */ u8 data1;
	/* 0x04 */ void32 segment;
} SCmdPathList;

typedef struct StructBE {
	/* 0x00 */ u8 code;
	/* 0x01 */ u8 num;
	/* 0x04 */ void32 segment;
} SCmdTransiActorList;

typedef struct StructBE {
	/* 0x00 */ u8 code;
	/* 0x01 */ u8 num;
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
	/* 0x00 */ u8 code;
	/* 0x01 */ u8 data1;
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
	/* 0x00 */ u8 code;
	/* 0x01 */ u8 data1;
	/* 0x04 */ void32 segment;
} SCmdCutsceneData;

typedef struct StructBE {
	/* 0x00 */ u8 code;
	/* 0x01 */ u8 data1;
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
	u8 numEntries;
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
	void32 imagePtr; // 0x08
	u32    unknown; // 0x0C
	u32    unknown2; // 0x10
	u16    bgWidth; // 0x14
	u16    bgHeight; // 0x16
	u8  imageFormat;       // 0x18
	u8  imageSize;         // 0x19
	u16 imagePal;          // 0x1A
	u16 imageFlip;         // 0x1C
} MeshHeader1Single;

typedef struct StructBE {
	MeshHeader1Base base;
	u8 bgCnt;
	void32 bgRecordPtr;
} MeshHeader1Multi;

typedef struct StructBE {
	u16    unknown; // 0x00
	s8     bgID; // 0x02
	void32 imagePtr; // 0x04
	u32    unknown2; // 0x08
	u32    unknown3; // 0x0C
	u16    bgWidth; // 0x10
	u16    bgHeight; // 0x12
	u8     imageFmt; // 0x14
	u8     imageSize; // 0x15
	u16    imagePal; // 0x16
	u16    imageFlip; // 0x18
} BackgroundRecord;

typedef struct StructBE {
	s16    playerXMax, playerZMax;
	s16    playerXMin, playerZMin;
	void32 opaqueDList;
	void32 translucentDList;
} MeshEntry2;

typedef struct StructBE {
	MeshHeaderBase base;
	u8 numEntries;
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
	/* 0x00 */ u8 count; // number of points in the path
	/* 0x04 */ void32 points; // Segment Address to the array of points
} PointPath; // size = 0x8

#endif /* __FOLD_TYPES__ */

typedef union StructBE {
	SCmdBase base;
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
	SCmdMesh mesh;
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

void (*sSceneCmdHandlers[SCENE_CMD_ID_MAX])(Scene*, Room*, SceneCmd*);
char* sSceneCmdHandlers_Name[SCENE_CMD_ID_MAX];

// # # # # # # # # # # # # # # # # # # # #
// # Scene                               #
// # # # # # # # # # # # # # # # # # # # #

void Scene_LoadScene(Scene* this, const char* file) {
	Log("Load File [%s]", file);
	MemFile_LoadFile(&this->file, file);
	this->segment = this->file.data;
	
	Scene_ExecuteCommands(this, NULL);
	
	Calloc(this->room, sizeof(Room*) * 255);
}

void Scene_LoadRoom(Scene* this, const char* file) {
	Room* room;
	
	Log("Load File [%s]", file);
	Calloc(room, sizeof(Room));
	MemFile_LoadFile(&room->file, file);
	room->segment = room->file.data;
	
	Scene_ExecuteCommands(this, room);
	
	this->room[this->numRoom++] = room;
}

void Scene_Free(Scene* this) {
	MemFile_Free(&this->file);
	for (s32 i = 0; i < this->numRoom; i++) {
		MemFile_Free(&this->room[i]->file);
		Free(this->room[i]);
	}
	Free(this->room);
	this->numRoom = 0;
	memset(this, 0, sizeof(*this));
	n64_clearShaderCache();
}

void Scene_ExecuteCommands(Scene* this, Room* room) {
	u8* segment;
	
	gSegment[2] = this->segment;
	gSegment[3] = room ? room->segment : NULL;
	
	if (!room) {
		segment = gSegment[2];
		Log("" PRNT_REDD "SCENE" PRNT_RSET ": [%s]", this->file.info.name);
	} else {
		segment = gSegment[3];
		Log("" PRNT_BLUE "ROOM" PRNT_RSET ": [%s]", room->file.info.name);
	}
	
	for (SceneCmd* cmd = (void*)segment; cmd->base.code != SCENE_CMD_ID_END ; cmd++) {
		if (cmd->base.code == SCENE_CMD_ID_END)
			break;
		
		if (cmd->base.code == SCENE_CMD_ID_ALTERNATE_HEADER_LIST && this->setupHeader != 0) {
		} else if (cmd->base.code < SCENE_CMD_ID_MAX) {
			Log("[%02X] %08X [%s]", cmd->base.code, (u8*)cmd - segment, sSceneCmdHandlers_Name[cmd->base.code]);
			if (sSceneCmdHandlers[cmd->base.code])
				sSceneCmdHandlers[cmd->base.code](this, room, cmd);
			
			else if (cmd->base.code != SCENE_CMD_ID_ALTERNATE_HEADER_LIST)
				printf_warning("SceneCmd %02X", cmd->base.code);
		}
	}
}

void Scene_Draw(Scene* this) {
	static EnvLightSettings* prevEnv;
	EnvLightSettings* env = this->env + this->setupEnv;
	s8 l1n[3], l2n[3];
	
	Assert(env != NULL);
	
	memcpy(l1n, env->light1Dir, 3);
	memcpy(l2n, env->light2Dir, 3);
	
	if (this->useDaylight && this->setupEnv < 4) {
		u16 time;
		
		switch (this->setupEnv) {
			case 0:
				time = 0x6000;
				break;
				
			case 1:
				time = 0x8001;
				break;
				
			case 2:
				time = 0xB556;
				break;
				
			case 3:
				time = 0xFFFF;
				break;
		}
		
		l1n[0] = Math_SinS((time - 0x8000)) * 120.0f;
		l1n[1] = Math_CosS((time - 0x8000)) * 120.0f;
		l1n[2] = Math_CosS((time - 0x8000)) * 20.0f;
		l2n[0] = -l1n[0];
		l2n[1] = -l1n[1];
		l2n[2] = -l1n[2];
	}
	
	gSegment[2] = this->segment;
	gSPSegment(POLY_OPA_DISP++, 0x02, this->segment);
	
	if (prevEnv != env) {
		printf_hex("EnvLight", env, sizeof(*env), 0);
	}
	
	Light_SetAmbLight(env->ambientColor);
	Light_SetFog(env->fogNear & 0x3FF, 0, env->fogColor);
	Light_SetDirLight(l1n, env->light1Color);
	Light_SetDirLight(l2n, env->light2Color);
	
	for (s32 i = 0; i < this->numRoom; i++)
		Room_Draw(this, this->room[i]);
	
	prevEnv = env;
}

// # # # # # # # # # # # # # # # # # # # #
// # Room                                #
// # # # # # # # # # # # # # # # # # # # #

typedef struct StructBE {
	/* 0x00 */ void32 opa;
	/* 0x04 */ void32 xlu;
} PolygonDlist; // size = 0x8

typedef struct StructBE {
	/* 0x00 */ u8 type;
} PolygonBase;

typedef struct StructBE {
	/* 0x00 */ PolygonBase base;
	/* 0x01 */ u8 num;     // number of dlist entries
	/* 0x04 */ void32 start;
	/* 0x08 */ void32 end;
} PolygonType0; // size = 0xC

typedef struct StructBE {
	/* 0x00 */ Vec3s  pos;
	/* 0x06 */ s16    unk_06;
	/* 0x08 */ void32 opa;
	/* 0x0C */ void32 xlu;
} PolygonDlist2; // size = 0x8

typedef struct StructBE {
	/* 0x00 */ PolygonBase base;
	/* 0x01 */ u8 num;     // number of dlist entries
	/* 0x04 */ void32 start;
	/* 0x08 */ void32 end;
} PolygonType2; // size = 0xC

typedef union StructBE {
	PolygonBase  base;
	PolygonType0 polygon0;
	PolygonType2 polygon2;
} MeshHeader; // "Ground Shape"

void Room_Draw(Scene* scene, Room* room) {
	MeshHeader* header = room->mesh;
	
	Assert(header != NULL);
	
	n64_set_onlyZmode(ZMODE_ALL);
	n64_set_onlyGeoLayer(GEOLAYER_ALL);
	gSPDisplayList(POLY_OPA_DISP++, gSetupDList(0x19));
	gDPSetEnvColor(POLY_OPA_DISP++, 0x80, 0x80, 0x80, 0x80);
	
	Matrix_Push();
	Matrix_Translate(0, 0, 0, MTXMODE_NEW);
	
	gSegment[3] = room->segment;
	gSPSegment(POLY_OPA_DISP++, 0x03, room->segment);
	
	gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
	
	if (header->base.type == 0) {
		PolygonType0* polygon = &header->polygon0;
		PolygonDlist* polygonDlist = SEGMENTED_TO_VIRTUAL(polygon->start);
		
		for (s32 i = 0; i < polygon->num; i++, polygonDlist++) {
			if (polygonDlist->opa != 0)
				gSPDisplayList(POLY_OPA_DISP++, polygonDlist->opa);
			
			if (polygonDlist->xlu != 0)
				gSPDisplayList(POLY_XLU_DISP++, polygonDlist->xlu);
		}
	} else if (header->base.type == 2) {
		PolygonType2* polygon = &header->polygon2;
		PolygonDlist2* polygonDlist = SEGMENTED_TO_VIRTUAL(polygon->start);
		
		for (s32 i = 0; i < polygon->num; i++, polygonDlist++) {
			if (polygonDlist->opa != 0)
				gSPDisplayList(POLY_OPA_DISP++, polygonDlist->opa);
			
			if (polygonDlist->xlu != 0)
				gSPDisplayList(POLY_XLU_DISP++, polygonDlist->xlu);
		}
	}
	
	Matrix_Pop();
}

// # # # # # # # # # # # # # # # # # # # #
// # Commands                            #
// # # # # # # # # # # # # # # # # # # # #

static void Scene_CommandSpawnList(Scene* scene, Room* room, SceneCmd* cmd) {
	// ActorEntry* linkEntry = play->linkActorEntry =
	// 	(ActorEntry*)SEGMENTED_TO_VIRTUAL(cmd->spawnList.segment) + play->setupEntranceList[play->curSpawn].spawn;
	// s16 linkObjectId;
	//
	// play->linkAgeOnLoad = ((void)0, gSaveContext.linkAge);
	//
	// linkObjectId = gLinkObjectIds[((void)0, gSaveContext.linkAge)];
	//
	// gActorOverlayTable[linkEntry->id].initInfo->objectId = linkObjectId;
	// Object_Spawn(&play->objectCtx, linkObjectId);
}

static void Scene_CommandActorList(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->numSetupActors = cmd->actorList.num;
	// play->setupActorList = SEGMENTED_TO_VIRTUAL(cmd->actorList.segment);
}

static void Scene_CommandUnused2(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->unk_11DFC = SEGMENTED_TO_VIRTUAL(cmd->unused02.segment);
}

static void Scene_CommandCollisionHeader(Scene* scene, Room* room, SceneCmd* cmd) {
	// CollisionHeader* colHeader = SEGMENTED_TO_VIRTUAL(cmd->colHeader.segment);
	//
	// colHeader->vtxList = SEGMENTED_TO_VIRTUAL(colHeader->vtxList);
	// colHeader->polyList = SEGMENTED_TO_VIRTUAL(colHeader->polyList);
	// colHeader->surfaceTypeList = SEGMENTED_TO_VIRTUAL(colHeader->surfaceTypeList);
	// colHeader->cameraDataList = SEGMENTED_TO_VIRTUAL(colHeader->cameraDataList);
	// colHeader->waterBoxes = SEGMENTED_TO_VIRTUAL(colHeader->waterBoxes);
	//
	// BgCheck_Allocate(&play->colCtx, play, colHeader);
}

static void Scene_CommandRoomList(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->numRooms = cmd->roomList.num;
	// play->roomList = SEGMENTED_TO_VIRTUAL(cmd->roomList.segment);
}

static void Scene_CommandEntranceList(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->setupEntranceList = SEGMENTED_TO_VIRTUAL(cmd->entranceList.segment);
}

static void Scene_CommandSpecialFiles(Scene* scene, Room* room, SceneCmd* cmd) {
	// if (cmd->specialFiles.keepObjectId != OBJECT_INVALID) {
	// 	play->objectCtx.subKeepIndex = Object_Spawn(&play->objectCtx, cmd->specialFiles.keepObjectId);
	// 	gSegments[5] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[play->objectCtx.subKeepIndex].segment);
	// }
	//
	// if (cmd->specialFiles.cUpElfMsgNum != 0) {
	// 	play->cUpElfMsgs = Play_LoadFile(play, &sNaviMsgFiles[cmd->specialFiles.cUpElfMsgNum - 1]);
	// }
}

static void Scene_CommandRoomBehavior(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->roomCtx.curRoom.behaviorType1 = cmd->roomBehavior.gpFlag1;
	// play->roomCtx.curRoom.behaviorType2 = cmd->roomBehavior.gpFlag2 & 0xFF;
	// play->roomCtx.curRoom.lensMode = (cmd->roomBehavior.gpFlag2 >> 8) & 1;
	// play->msgCtx.disableWarpSongs = (cmd->roomBehavior.gpFlag2 >> 0xA) & 1;
}

static void Scene_CommandMeshHeader(Scene* scene, Room* room, SceneCmd* cmd) {
	if (room == NULL)
		printf_warning("Scene_CommandMeshHeader: No Room Provided...");
	
	else
		room->mesh = SEGMENTED_TO_VIRTUAL(cmd->mesh.segment);
}

static void Scene_CommandObjectList(Scene* scene, Room* room, SceneCmd* cmd) {
	// s32 i;
	// s32 j;
	// s32 k;
	// ObjectStatus* status;
	// ObjectStatus* status2;
	// ObjectStatus* firstStatus;
	// s16* objectEntry = SEGMENTED_TO_VIRTUAL(cmd->objectList.segment);
	// void* nextPtr;
	//
	// k = 0;
	// i = play->objectCtx.unk_09;
	// firstStatus = &play->objectCtx.status[0];
	// status = &play->objectCtx.status[i];
	//
	// while (i < play->objectCtx.num) {
	// 	if (status->id != *objectEntry) {
	// 		status2 = &play->objectCtx.status[i];
	// 		for (j = i; j < play->objectCtx.num; j++) {
	// 			status2->id = OBJECT_INVALID;
	// 			status2++;
	// 		}
	// 		play->objectCtx.num = i;
	// 		func_80031A28(play, &play->actorCtx);
	//
	// 		continue;
	// 	}
	//
	// 	i++;
	// 	k++;
	// 	objectEntry++;
	// 	status++;
	// }
	//
	// ASSERT(
	// 	cmd->objectList.num <= OBJECT_EXCHANGE_BANK_MAX,
	// 	"scene_info->object_bank.num <= OBJECT_EXCHANGE_BANK_MAX",
	// 	"../z_scene.c",
	// 	705
	// );
	//
	// while (k < cmd->objectList.num) {
	// 	nextPtr = func_800982FC(&play->objectCtx, i, *objectEntry);
	// 	if (i < OBJECT_EXCHANGE_BANK_MAX - 1) {
	// 		firstStatus[i + 1].segment = nextPtr;
	// 	}
	// 	i++;
	// 	k++;
	// 	objectEntry++;
	// }
	//
	// play->objectCtx.num = i;
}

static void Scene_CommandLightList(Scene* scene, Room* room, SceneCmd* cmd) {
	// s32 i;
	// LightInfo* lightInfo = SEGMENTED_TO_VIRTUAL(cmd->lightList.segment);
	//
	// for (i = 0; i < cmd->lightList.num; i++) {
	// 	LightContext_InsertLight(play, &play->lightCtx, lightInfo);
	// 	lightInfo++;
	// }
}

static void Scene_CommandPathList(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->setupPathList = SEGMENTED_TO_VIRTUAL(cmd->pathList.segment);
}

static void Scene_CommandTransitionActorList(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->transiActorCtx.numActors = cmd->transiActorList.num;
	// play->transiActorCtx.list = SEGMENTED_TO_VIRTUAL(cmd->transiActorList.segment);
}

static void Scene_CommandLightSettingsList(Scene* scene, Room* room, SceneCmd* cmd) {
	scene->numEnv = cmd->lightSettingList.num;
	scene->env = SEGMENTED_TO_VIRTUAL(cmd->lightSettingList.segment);
	Log("Light List: %08X", (u8*)scene->env - (u8*)scene->segment);
}

static void Scene_CommandSkyboxSettings(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->skyboxId = cmd->skyboxSettings.skyboxId;
	// play->envCtx.skyboxConfig = play->envCtx.changeSkyboxNextConfig = cmd->skyboxSettings.unk_05;
	// play->envCtx.lightMode = cmd->skyboxSettings.unk_06;
}

static void Scene_CommandSkyboxDisables(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->envCtx.skyboxDisabled = cmd->skyboxDisables.unk_04;
// play->envCtx.sunMoonDisabled = cmd->skyboxDisables.unk_05;
}

static void Scene_CommandTimeSettings(Scene* scene, Room* room, SceneCmd* cmd) {
	// if ((cmd->timeSettings.hour != 0xFF) && (cmd->timeSettings.min != 0xFF)) {
	// 	gSaveContext.skyboxTime = gSaveContext.dayTime =
	// 		((cmd->timeSettings.hour + (cmd->timeSettings.min / 60.0f)) * 60.0f) / ((f32)(24 * 60) / 0x10000);
	// }
	//
	// if (cmd->timeSettings.unk_06 != 0xFF) {
	// 	play->envCtx.sceneTimeSpeed = cmd->timeSettings.unk_06;
	// } else {
	// 	play->envCtx.sceneTimeSpeed = 0;
	// }
	//
	// if (gSaveContext.sunsSongState == SUNSSONG_INACTIVE) {
	// 	gTimeSpeed = play->envCtx.sceneTimeSpeed;
	// }
	//
	// play->envCtx.sunPos.x = -(Math_SinS(((void)0, gSaveContext.dayTime) - CLOCK_TIME(12, 0)) * 120.0f) * 25.0f;
	// play->envCtx.sunPos.y = (Math_CosS(((void)0, gSaveContext.dayTime) - CLOCK_TIME(12, 0)) * 120.0f) * 25.0f;
	// play->envCtx.sunPos.z = (Math_CosS(((void)0, gSaveContext.dayTime) - CLOCK_TIME(12, 0)) * 20.0f) * 25.0f;
	//
	// if (((play->envCtx.sceneTimeSpeed == 0) && (gSaveContext.cutsceneIndex < 0xFFF0)) ||
	// 	(gSaveContext.entranceIndex == ENTR_SPOT06_8)) {
	// 	gSaveContext.skyboxTime = ((void)0, gSaveContext.dayTime);
	//
	// 	if ((gSaveContext.skyboxTime > CLOCK_TIME(4, 0)) && (gSaveContext.skyboxTime < CLOCK_TIME(6, 30))) {
	// 		gSaveContext.skyboxTime = CLOCK_TIME(5, 0) + 1;
	// 	} else if ((gSaveContext.skyboxTime >= CLOCK_TIME(6, 30)) && (gSaveContext.skyboxTime <= CLOCK_TIME(8, 0))) {
	// 		gSaveContext.skyboxTime = CLOCK_TIME(8, 0) + 1;
	// 	} else if ((gSaveContext.skyboxTime >= CLOCK_TIME(16, 0)) && (gSaveContext.skyboxTime <= CLOCK_TIME(17, 0))) {
	// 		gSaveContext.skyboxTime = CLOCK_TIME(17, 0) + 1;
	// 	} else if ((gSaveContext.skyboxTime >= CLOCK_TIME(18, 0) + 1) &&
	// 		(gSaveContext.skyboxTime <= CLOCK_TIME(19, 0))) {
	// 		gSaveContext.skyboxTime = CLOCK_TIME(19, 0) + 1;
	// 	}
	// }
}

static void Scene_CommandWindSettings(Scene* scene, Room* room, SceneCmd* cmd) {
	// s8 x = cmd->windSettings.x;
	// s8 y = cmd->windSettings.y;
	// s8 z = cmd->windSettings.z;
	//
	// play->envCtx.windDirection.x = x;
	// play->envCtx.windDirection.y = y;
	// play->envCtx.windDirection.z = z;
	//
	// play->envCtx.windSpeed = cmd->windSettings.unk_07;
}

static void Scene_CommandExitList(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->setupExitList = SEGMENTED_TO_VIRTUAL(cmd->exitList.segment);
}

static void Scene_CommandUndefined9(Scene* scene, Room* room, SceneCmd* cmd) {
}

static void Scene_CommandSoundSettings(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->sequenceCtx.seqId = cmd->soundSettings.seqId;
	// play->sequenceCtx.natureAmbienceId = cmd->soundSettings.natureAmbienceId;
	//
	// if (gSaveContext.seqId == (u8)NA_BGM_DISABLED) {
	// 	Audio_QueueSeqCmd(cmd->soundSettings.specId | 0xF0000000);
	// }
}

static void Scene_CommandEchoSettings(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->roomCtx.curRoom.echo = cmd->echoSettings.echo;
}

static void Scene_CommandCutsceneData(Scene* scene, Room* room, SceneCmd* cmd) {
	// osSyncPrintf("\ngame_play->demo_play.data=[%x]", play->csCtx.segment);
	// play->csCtx.segment = SEGMENTED_TO_VIRTUAL(cmd->cutsceneData.segment);
}

static void Scene_CommandMiscSettings(Scene* scene, Room* room, SceneCmd* cmd) {
	// YREG(15) = cmd->miscSettings.cameraMovement;
	// gSaveContext.worldMapArea = cmd->miscSettings.area;
	//
	// if ((play->sceneNum == SCENE_SHOP1) || (play->sceneNum == SCENE_SYATEKIJYOU)) {
	// 	if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
	// 		gSaveContext.worldMapArea = 1;
	// 	}
	// }
	//
	// if (((play->sceneNum >= SCENE_SPOT00) && (play->sceneNum <= SCENE_GANON_TOU)) ||
	// 	((play->sceneNum >= SCENE_ENTRA) && (play->sceneNum <= SCENE_SHRINE_R))) {
	// 	if (gSaveContext.cutsceneIndex < 0xFFF0) {
	// 		gSaveContext.worldMapAreaData |= gBitFlags[gSaveContext.worldMapArea];
	// 		osSyncPrintf(
	// 			"０００  ａｒｅａ＿ａｒｒｉｖａｌ＝%x (%d)\n",
	// 			gSaveContext.worldMapAreaData,
	// 			gSaveContext.worldMapArea
	// 		);
	// 	}
	// }
}

void (*sSceneCmdHandlers[SCENE_CMD_ID_MAX])(Scene*, Room*, SceneCmd*) = {
	Scene_CommandSpawnList, // SCENE_CMD_ID_SPAWN_LIST
	Scene_CommandActorList, // SCENE_CMD_ID_ACTOR_LIST
	Scene_CommandUnused2,  // SCENE_CMD_ID_UNUSED_2
	Scene_CommandCollisionHeader, // SCENE_CMD_ID_COLLISION_HEADER
	Scene_CommandRoomList, // SCENE_CMD_ID_ROOM_LIST
	Scene_CommandWindSettings, // SCENE_CMD_ID_WIND_SETTINGS
	Scene_CommandEntranceList, // SCENE_CMD_ID_ENTRANCE_LIST
	Scene_CommandSpecialFiles, // SCENE_CMD_ID_SPECIAL_FILES
	Scene_CommandRoomBehavior, // SCENE_CMD_ID_ROOM_BEHAVIOR
	Scene_CommandUndefined9, // SCENE_CMD_ID_UNDEFINED_9
	Scene_CommandMeshHeader, // SCENE_CMD_ID_MESH_HEADER
	Scene_CommandObjectList, // SCENE_CMD_ID_OBJECT_LIST
	Scene_CommandLightList, // SCENE_CMD_ID_LIGHT_LIST
	Scene_CommandPathList, // SCENE_CMD_ID_PATH_LIST
	Scene_CommandTransitionActorList, // SCENE_CMD_ID_TRANSITION_ACTOR_LIST
	Scene_CommandLightSettingsList, // SCENE_CMD_ID_LIGHT_SETTINGS_LIST
	Scene_CommandTimeSettings, // SCENE_CMD_ID_TIME_SETTINGS
	Scene_CommandSkyboxSettings, // SCENE_CMD_ID_SKYBOX_SETTINGS
	Scene_CommandSkyboxDisables, // SCENE_CMD_ID_SKYBOX_DISABLES
	Scene_CommandExitList, // SCENE_CMD_ID_EXIT_LIST
	NULL,                  // SCENE_CMD_ID_END
	Scene_CommandSoundSettings, // SCENE_CMD_ID_SOUND_SETTINGS
	Scene_CommandEchoSettings, // SCENE_CMD_ID_ECHO_SETTINGS
	Scene_CommandCutsceneData, // SCENE_CMD_ID_CUTSCENE_DATA
	NULL, // SCENE_CMD_ID_ALTERNATE_HEADER_LIST
	Scene_CommandMiscSettings, // SCENE_CMD_ID_MISC_SETTINGS
};

char* sSceneCmdHandlers_Name[SCENE_CMD_ID_MAX] = {
	"Scene_CommandSpawnList", // SCENE_CMD_ID_SPAWN_LIST
	"Scene_CommandActorList", // SCENE_CMD_ID_ACTOR_LIST
	"Scene_CommandUnused2", // SCENE_CMD_ID_UNUSED_2
	"Scene_CommandCollisionHeader", // SCENE_CMD_ID_COLLISION_HEADER
	"Scene_CommandRoomList", // SCENE_CMD_ID_ROOM_LIST
	"Scene_CommandWindSettings", // SCENE_CMD_ID_WIND_SETTINGS
	"Scene_CommandEntranceList", // SCENE_CMD_ID_ENTRANCE_LIST
	"Scene_CommandSpecialFiles", // SCENE_CMD_ID_SPECIAL_FILES
	"Scene_CommandRoomBehavior", // SCENE_CMD_ID_ROOM_BEHAVIOR
	"Scene_CommandUndefined9", // SCENE_CMD_ID_UNDEFINED_9
	"Scene_CommandMeshHeader", // SCENE_CMD_ID_MESH_HEADER
	"Scene_CommandObjectList", // SCENE_CMD_ID_OBJECT_LIST
	"Scene_CommandLightList", // SCENE_CMD_ID_LIGHT_LIST
	"Scene_CommandPathList", // SCENE_CMD_ID_PATH_LIST
	"Scene_CommandTransitionActorList", // SCENE_CMD_ID_TRANSITION_ACTOR_LIST
	"Scene_CommandLightSettingsList", // SCENE_CMD_ID_LIGHT_SETTINGS_LIST
	"Scene_CommandTimeSettings", // SCENE_CMD_ID_TIME_SETTINGS
	"Scene_CommandSkyboxSettings", // SCENE_CMD_ID_SKYBOX_SETTINGS
	"Scene_CommandSkyboxDisables", // SCENE_CMD_ID_SKYBOX_DISABLES
	"Scene_CommandExitList", // SCENE_CMD_ID_EXIT_LIST
	"NULL",                // SCENE_CMD_ID_END
	"Scene_CommandSoundSettings", // SCENE_CMD_ID_SOUND_SETTINGS
	"Scene_CommandEchoSettings", // SCENE_CMD_ID_ECHO_SETTINGS
	"Scene_CommandCutsceneData", // SCENE_CMD_ID_CUTSCENE_DATA
	"SetupIndex", // SCENE_CMD_ID_ALTERNATE_HEADER_LIST
	"Scene_CommandMiscSettings", // SCENE_CMD_ID_MISC_SETTINGS
};