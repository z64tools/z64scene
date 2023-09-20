#include "Export.h"
#include <ext_lib.h>
#include "Scene.h"
#include "Gfx.h"

#include "dlcopy/src/zobj.h"

typedef struct Export {
	const char* basename;
	const char* path;
	DispRefList ref;
	Scene*      scene;
	
	Memfile  main;
	Memfile* header[0xF];
	
	struct ExportRoom {
		int      used;
		Memfile  main;
		Memfile* header[0xF];
	}* room;
} Export;

const char* rUnecpectedError = "Unexpected Error!";
const char* rFatalError = "Fatal Error!";
const char* rSuccess = NULL;

static const char* sSceneCmdNames[] = {
	[SCENE_CMD_SPAWN_LIST] =            "SpawnList",
	[SCENE_CMD_ACTOR_LIST] =            "ActorList",
	[SCENE_CMD_COLLISION_HEADER] =      "CollisionHeader",
	[SCENE_CMD_ROOM_LIST] =             "RoomList",
	[SCENE_CMD_WIND_SETTINGS] =         "WindSettings",
	[SCENE_CMD_ENTRANCE_LIST] =         "EntranceList",
	[SCENE_CMD_SPECIAL_FILES] =         "SpecialFiles",
	[SCENE_CMD_ROOM_BEHAVIOR] =         "RoomBehaviour",
	[SCENE_CMD_UNDEFINED_9] =           "Undefined9",
	[SCENE_CMD_MESH_HEADER] =           "MeshHeader",
	[SCENE_CMD_OBJECT_LIST] =           "ObjectList",
	[SCENE_CMD_LIGHT_LIST] =            "LightList",
	[SCENE_CMD_PATH_LIST] =             "PathList",
	[SCENE_CMD_TRANSITION_ACTOR_LIST] = "TransitionActorList",
	[SCENE_CMD_LIGHT_SETTINGS_LIST] =   "LightSettingsList",
	[SCENE_CMD_TIME_SETTINGS] =         "TimeSettings",
	[SCENE_CMD_SKYBOX_SETTINGS] =       "SkyboxSettings",
	[SCENE_CMD_SKYBOX_DISABLES] =       "SkyboxDisables",
	[SCENE_CMD_EXIT_LIST] =             "ExitList",
	[SCENE_CMD_END] =                   "End",
	[SCENE_CMD_SOUND_SETTINGS] =        "SoundSettings",
	[SCENE_CMD_ECHO_SETTINGS] =         "EchoSettings",
	[SCENE_CMD_CUTSCENE_DATA] =         "CutsceneData",
	[SCENE_CMD_ALTERNATE_HEADER_LIST] = "AlternateHeaderList",
	[SCENE_CMD_MISC_SETTINGS] =         "MiscSettings",
	[SCENE_CMD_RENDER_INIT] =           "RenderInit",
};

static int sSceneCommands[] = {
	SCENE_CMD_ALTERNATE_HEADER_LIST,
	SCENE_CMD_SOUND_SETTINGS,
	SCENE_CMD_ROOM_LIST,
	SCENE_CMD_TRANSITION_ACTOR_LIST,
	SCENE_CMD_MISC_SETTINGS,
	SCENE_CMD_COLLISION_HEADER,
	SCENE_CMD_ENTRANCE_LIST,
	SCENE_CMD_SPECIAL_FILES,
	SCENE_CMD_PATH_LIST,
	SCENE_CMD_SPAWN_LIST,
	SCENE_CMD_SKYBOX_SETTINGS,
	SCENE_CMD_EXIT_LIST,
	SCENE_CMD_LIGHT_SETTINGS_LIST,
	SCENE_CMD_CUTSCENE_DATA,
	SCENE_CMD_RENDER_INIT,
	SCENE_CMD_END,
};

static int sRoomCommands[] = {
	SCENE_CMD_ALTERNATE_HEADER_LIST,
	SCENE_CMD_ECHO_SETTINGS,
	SCENE_CMD_ROOM_BEHAVIOR,
	SCENE_CMD_UNDEFINED_9,
	SCENE_CMD_SKYBOX_DISABLES,
	SCENE_CMD_TIME_SETTINGS,
	SCENE_CMD_WIND_SETTINGS,
	SCENE_CMD_MESH_HEADER,
	SCENE_CMD_OBJECT_LIST,
	SCENE_CMD_ACTOR_LIST,
	SCENE_CMD_LIGHT_LIST,
	SCENE_CMD_END,
};

static struct {
	const char* prefix;
} g64;

///////////////////////////////////////////////////////////////////////////////

#define SYM_SCENE_NAME(x, ...) x_fmt("%s_Scene_Set%d_" x, g64.prefix, setId, ## __VA_ARGS__)
#define SYM_ROOM_NAME(x, ...)  x_fmt("%s_Room%d_Set%d_" x, g64.prefix, header->id, setId, ## __VA_ARGS__)
#define SYM_NAME(x, ...)       x_fmt("%s_" x, g64.prefix, ## __VA_ARGS__)

static void WriteEndCmd(Memfile* this) {
	u8 w[8] = { SCENE_CMD_END, 0, 0, 0, 'z', '6', '4', '0' };
	
	Memfile_Write(this, w, sizeof(w));
}

///////////////////////////////////////////////////////////////////////////////

static void WriteSceneCmd_AlternateHeaderList(Memfile* this, Scene* scene, SceneHeader* header, u8 setId) {
	if (!(scene->numHeader > 1)) return;
	if (setId != 0) return;
	
	u8 w[4] = { SCENE_CMD_ALTERNATE_HEADER_LIST, scene->numHeader - 1 };
	Memfile* list = Memfile_NewSym(this, SYM_NAME("SceneSetList"), "SCmdBase*", 4);
	
	Memfile_Write(this, w, sizeof(w));
	Memfile_WriteRef(this, list, SYM_32);
	
	for (int i = 1; i < scene->numHeader; i++)
		Memfile_WriteRefByName(list, x_fmt("%s_Scene_Set%dHead", g64.prefix, i), SYM_32);
}

static void WriteSceneCmd_SpawnList(Memfile* this, Scene* scene, SceneHeader* header, u8 setId) {
	u8 w[] = { SCENE_CMD_SPAWN_LIST, header->spawnList.num, 0, 0 };
	Memfile* spawnList = Memfile_NewSym(this, SYM_SCENE_NAME("SpawnList"), "ActorEntry", 4);
	
	Memfile_Write(this, w, sizeof(w));
	Memfile_WriteRef(this, spawnList, SYM_32);
	
	SpawnActor* spawnActor = Arli_Head(&header->spawnList);
	for (int i = 0; i < header->spawnList.num; i++, spawnActor++) {
		ActorEntry w = {
			.id    = spawnActor->actor.id,
			.pos.x = spawnActor->actor.pos.x,
			.pos.y = spawnActor->actor.pos.y,
			.pos.z = spawnActor->actor.pos.z,
			.rot.x = spawnActor->actor.rot.x,
			.rot.y = spawnActor->actor.rot.y,
			.rot.z = spawnActor->actor.rot.z,
			.param = spawnActor->actor.param,
		};
		
		Memfile_Write(spawnList, &w, sizeof(ActorEntry));
	}
}

static void WriteSceneCmd_CollisionHeader(Memfile* this, Scene* scene, SceneHeader* header, u8 setId) {
	u8 w[4] = { SCENE_CMD_COLLISION_HEADER };
	
	Memfile_Write(this, w, sizeof(w));
	Memfile_WriteRefByName(this, SYM_NAME("ColHeader"), SYM_32);
}

static void WriteSceneCmd_RoomList(Memfile* this, Scene* scene, SceneHeader* header, u8 setId) {
	u8 w[4] = { SCENE_CMD_ROOM_LIST, scene->numRoom };
	Memfile* roomList = Memfile_NewSym(this, SYM_SCENE_NAME("RoomList"), "RomFile", 4);
	
	Memfile_Write(this, w, sizeof(w));
	Memfile_WriteRef(this, roomList, SYM_32);
	
	u64 temp[scene->numRoom];
	
	memset(temp, 0xAB, sizeof(temp));
	Memfile_Write(roomList, temp, sizeof(temp));
}

static void WriteSceneCmd_EntranceList(Memfile* this, Scene* scene, SceneHeader* header, u8 setId) {
	u8 w[4] = { SCENE_CMD_ENTRANCE_LIST, header->spawnList.num };
	Memfile* entranceList = Memfile_NewSym(this, SYM_SCENE_NAME("EntranceList"), "EntranceEntry", 4);
	
	Memfile_Write(this, w, sizeof(w));
	Memfile_WriteRef(this, entranceList, SYM_32);
	
	typedef struct {
		u8 spawn;
		u8 room;
	} EntranceEntry;
	
	SpawnActor* spawn = Arli_Head(&header->spawnList);
	for (int i = 0; i < header->spawnList.num; i++, spawn++) {
		EntranceEntry w = { spawn->spawn, spawn->room };
		
		Memfile_Write(entranceList, &w, sizeof(w));
	}
}

static void WriteSceneCmd_SpecialFiles(Memfile* this, Scene* scene, SceneHeader* header, u8 setId) {
	u8 w[6] = { SCENE_CMD_SPECIAL_FILES, header->naviMsg };
	u16 obj = ReadBE(header->keepObject);
	
	Memfile_Write(this, w, sizeof(w));
	Memfile_Write(this, &obj, sizeof(u16));
}

static void WriteSceneCmd_PathList(Memfile* this, Scene* scene, SceneHeader* header, u8 setId) {
	if (!header->pathList.num) return;
	
	u8 w[4] = { SCENE_CMD_PATH_LIST, header->pathList.num };
	Memfile* pathList = Memfile_NewSym(this, SYM_SCENE_NAME("PathList"), "Path", 4);
	
	Memfile_Write(this, w, sizeof(w));
	Memfile_WriteRef(this, pathList, SYM_32);
	
	for (int i = 0; i < header->pathList.num; i++) {
		_log("path %d / %d", i + 1, header->pathList.num);
		Memfile* vecList = Memfile_NewSym(pathList, SYM_SCENE_NAME("PathList%d_VecList", i), "Vec3s", 4);
		PathList* list = Arli_At(&header->pathList, i);
		u8 w[4] = { list->num };
		
		Memfile_Write(pathList, w, sizeof(w));
		Memfile_WriteRef(pathList, vecList, SYM_32);
		
		Vec3s_BE* t = new(Vec3s_BE[list->num]);
		Vec3f* src = list->point;
		Vec3s_BE* dst = t;
		
		for (int k = 0; k < list->num; k++, src++, dst++) {
			_log("%d / %d", k + 1, list->num);
			dst->x = rint(src->x);
			dst->y = rint(src->y);
			dst->z = rint(src->z);
		}
		
		_log("write");
		Memfile_Write(vecList, t, sizeof(Vec3s[list->num]));
		delete(t);
	}
}

static void WriteSceneCmd_TransitionActorList(Memfile* this, Scene* scene, SceneHeader* header, u8 setId) {
	if (!header->transitionList.num) return;
	
	u8 w[4] = { SCENE_CMD_TRANSITION_ACTOR_LIST, header->transitionList.num };
	Memfile* transList = Memfile_NewSym(this, SYM_SCENE_NAME("TransActorList"), "TransitionActorEntry", 4);
	
	Memfile_Write(this, w, sizeof(w));
	Memfile_WriteRef(this, transList, SYM_32);
	
	typedef struct StructBE {
		struct {
			s8 room;
			s8 bgCamIndex;
		} sides[2];
		s16   id;
		Vec3s pos;
		s16   rotY;
		s16   params;
	} TransitionActorEntry;
	
	TransitionActor* trans = Arli_Head(&header->transitionList);
	for (int i = 0; i < header->transitionList.num; i++, trans++) {
		TransitionActorEntry entry = {
			.id                  = trans->actor.id,
			.params              = trans->actor.param,
			.pos                 = { UnfoldVec3(trans->actor.pos) },
			.rotY                = trans->actor.rot.y,
			.sides[0].room       = trans->side[0].room,
			.sides[0].bgCamIndex = trans->side[0].cam,
			.sides[1].room       = trans->side[1].room,
			.sides[1].bgCamIndex = trans->side[1].cam,
		};
		
		Memfile_Write(transList, &entry, sizeof(TransitionActorEntry));
	}
	
}

static void WriteSceneCmd_LightSettingsList(Memfile* this, Scene* scene, SceneHeader* header, u8 setId) {
	u8 w[4] = { SCENE_CMD_LIGHT_SETTINGS_LIST, header->envList.num };
	Memfile* lightList = Memfile_NewSym(this, SYM_SCENE_NAME("EnvLightList"), "LightSettings", 4);
	
	Memfile_Write(this, w, sizeof(w));
	Memfile_WriteRef(this, lightList, SYM_32);
	
	if (header->envList.num)
		Memfile_Write(lightList, Arli_Head(&header->envList), sizeof(EnvLights[header->envList.num]));
}

static void WriteSceneCmd_SkyboxSettings(Memfile* this, Scene* scene, SceneHeader* header, u8 setId) {
	u8 w[8] = { SCENE_CMD_SKYBOX_SETTINGS, 0, 0, 0, header->skyId, header->cloudState, header->envState };
	
	Memfile_Write(this, w, sizeof(w));
}

static void WriteSceneCmd_ExitList(Memfile* this, Scene* scene, SceneHeader* header, u8 setId) {
	if (!header->exitList.num) return;
	
	u8 w[4] = { SCENE_CMD_EXIT_LIST, header->exitList.num };
	Memfile* list = Memfile_NewSym(this, SYM_SCENE_NAME("ExitList"), "u16", 4);
	
	Memfile_Write(this, w, sizeof(w));
	Memfile_WriteRef(this, list, SYM_32);
	
	u32* exit = Arli_Head(&header->exitList);
	for (int i = 0; i < header->exitList.num; i++, exit++) {
		u32 val = ReadBE(*exit);
		u32* p32 = &val;
		u16* p16 = ((u16*)&val) + 1;
		
		if (!(*exit & 0x8000))
			Memfile_Write(list, p16, 2);
		else
			Memfile_Write(list, p32, 4);
	}
}

static void WriteSceneCmd_SoundSettings(Memfile* this, Scene* scene, SceneHeader* header, u8 setId) {
	u8 w[8] = { SCENE_CMD_SOUND_SETTINGS, header->sound.specID, 0, 0, 0, 0, header->sound.sfxID, header->sound.bgmID };
	
	Memfile_Write(this, w, sizeof(w));
}

static void WriteSceneCmd_CutsceneData(Memfile* this, Scene* scene, SceneHeader* header, u8 setId) {
	
}

static void WriteSceneCmd_MiscSettings(Memfile* this, Scene* scene, SceneHeader* header, u8 setId) {
	u8 w[8] = { SCENE_CMD_MISC_SETTINGS, header->camType, 0, 0, 0, 0, 0, header->area };
	
	Memfile_Write(this, w, sizeof(w));
}

static void WriteSceneCmd_RenderInit(Memfile* this, Scene* scene, SceneHeader* header, u8 setId) {
	u8 w[4] = { SCENE_CMD_RENDER_INIT };
	u8 anim[] = {
		0xFF, 0x00, 0x00, 0x00,
		0x02, 0x02, 0x00, 0xC8,
		0x02, 0x01, 0x20, 0x20,
		0x00, 0x00, 0x00, 0x00
	};
	
	Memfile* list = Memfile_NewSym(this, SYM_SCENE_NAME("RenderInit"), "u8", 4);
	
	Memfile_Write(this, w, sizeof(w));
	Memfile_WriteRef(this, list, SYM_32);
	
	Memfile_Write(list, anim, sizeof(anim));
}

///////////////////////////////////////////////////////////////////////////////

static void WriteRoomCmd_AlternateHeaderList(Memfile* this, Scene* scene, RoomHeader* header, u8 setId) {
	if (!(scene->numHeader > 1)) return;
	if (setId != 0) return;
	
	u8 w[4] = { SCENE_CMD_ALTERNATE_HEADER_LIST, scene->numHeader - 1 };
	Memfile* list = Memfile_NewSym(this, SYM_NAME("Room_SetList"), "SCmdBase*", 4);
	
	Memfile_Write(this, w, sizeof(w));
	Memfile_WriteRef(this, list, SYM_32);
	
	for (int i = 1; i < scene->numHeader; i++)
		Memfile_WriteRefByName(list, x_fmt("%s_Room%d_Set%dHead", g64.prefix, header->id, i), SYM_32);
}

static void WriteRoomCmd_ActorList(Memfile* this, Scene* scene, RoomHeader* header, u8 setId) {
	if (!header->actorList.num) return;
	
	u8 w[4] = { SCENE_CMD_ACTOR_LIST, header->actorList.num };
	Memfile* list = Memfile_NewSym(this, SYM_ROOM_NAME("ActorList"), "ActorEntry", 4);
	
	Memfile_Write(this, w, sizeof(w));
	Memfile_WriteRef(this, list, SYM_32);
	
	Actor* actor = Arli_Head(&header->actorList);
	
	for (int i = 0; i < header->actorList.num; i++, actor++) {
		ActorEntry w = {
			.id    = actor->id,
			.param = actor->param,
			.pos   = { UnfoldVec3(actor->pos) },
			.rot   = { UnfoldVec3(actor->rot) },
		};
		
		Memfile_Write(list, &w, sizeof(ActorEntry));
	}
}

static void WriteRoomCmd_WindSettings(Memfile* this, Scene* scene, RoomHeader* header, u8 setId) {
	if (!header->wind.strength) return;
	
	u8 w[8] = { SCENE_CMD_WIND_SETTINGS, 0, 0, 0, UnfoldVec3(header->wind), header->wind.strength };
	
	Memfile_Write(this, w, sizeof(w));
}

static void WriteRoomCmd_RoomBehaviour(Memfile* this, Scene* scene, RoomHeader* header, u8 setId) {
	u8 w[8] = {
		SCENE_CMD_ROOM_BEHAVIOR,
		header->behaviour.val1,
		0,
		0,
		0,
		0,
		((!!header->behaviour.lensMode) | (!!header->behaviour.disableWarpSongs) << 2),
		header->behaviour.val2
	};
	
	Memfile_Write(this, w, sizeof(w));
}

static void WriteRoomCmd_Undefined9(Memfile* this, Scene* scene, RoomHeader* header, u8 setId) {
	return;
	
	u8 w[8] = { SCENE_CMD_UNDEFINED_9 };
	
	Memfile_Write(this, w, sizeof(w));
}

static void WriteRoomCmd_MeshHeader(Memfile* this, Scene* scene, RoomHeader* header, u8 setId) {
	u8 w[4] = { SCENE_CMD_MESH_HEADER };
	
	Memfile_Write(this, w, sizeof(w));
	Memfile_WriteRefByName(this, SYM_NAME("RoomMeshHeader%08X", header->mesh->uuid), SYM_32);
}

static void WriteRoomCmd_ObjectList(Memfile* this, Scene* scene, RoomHeader* header, u8 setId) {
	if (!header->objectList.num) return;
	
	u8 w[4] = { SCENE_CMD_OBJECT_LIST, header->objectList.num };
	Memfile* list = Memfile_NewSym(this, SYM_ROOM_NAME("ObjList"), "u16", 4);
	
	Memfile_Write(this, w, sizeof(w));
	Memfile_WriteRef(this, list, SYM_32);
	
	u16* objID = Arli_Head(&header->objectList);
	for (int i = 0; i < header->objectList.num; i++, objID++) {
		u16 v = ReadBE(*objID);
		
		Memfile_Write(list, &v, sizeof(u16));
	}
}

static void WriteRoomCmd_LightList(Memfile* this, Scene* scene, RoomHeader* header, u8 setId) {
	// TODO
}

static void WriteRoomCmd_TimeSettings(Memfile* this, Scene* scene, RoomHeader* header, u8 setId) {
	u8 w[8] = { SCENE_CMD_TIME_SETTINGS };
	
	w[4] = header->timeHour;
	w[5] = header->timeMinute;
	w[6] = header->timeSpeed;
	
	if (header->timeGlobal)
		w[4] = w[5] = 0xFF;
	
	Memfile_Write(this, w, sizeof(w));
}

static void WriteRoomCmd_SkyboxDisables(Memfile* this, Scene* scene, RoomHeader* header, u8 setId) {
	u8 w[8] = { SCENE_CMD_SKYBOX_DISABLES };
	
	w[4] = header->skyBox.disableSky;
	w[5] = header->skyBox.disableSunMoon;
	
	Memfile_Write(this, w, sizeof(w));
}

static void WriteRoomCmd_EchoSettings(Memfile* this, Scene* scene, RoomHeader* header, u8 setId) {
	u8 w[8] = { SCENE_CMD_ECHO_SETTINGS };
	
	w[7] = header->echo;
	
	Memfile_Write(this, w, sizeof(w));
}

///////////////////////////////////////////////////////////////////////////////

static void (*sSceneCmdWrite[])(Memfile*, Scene*, SceneHeader*, u8) = {
	[SCENE_CMD_ALTERNATE_HEADER_LIST] = WriteSceneCmd_AlternateHeaderList,
	[SCENE_CMD_END] =                   (void*)WriteEndCmd,
	
	[SCENE_CMD_SPAWN_LIST] =            WriteSceneCmd_SpawnList,
	[SCENE_CMD_COLLISION_HEADER] =      WriteSceneCmd_CollisionHeader,
	[SCENE_CMD_ROOM_LIST] =             WriteSceneCmd_RoomList,
	[SCENE_CMD_ENTRANCE_LIST] =         WriteSceneCmd_EntranceList,
	[SCENE_CMD_SPECIAL_FILES] =         WriteSceneCmd_SpecialFiles,
	[SCENE_CMD_PATH_LIST] =             WriteSceneCmd_PathList,
	[SCENE_CMD_TRANSITION_ACTOR_LIST] = WriteSceneCmd_TransitionActorList,
	[SCENE_CMD_LIGHT_SETTINGS_LIST] =   WriteSceneCmd_LightSettingsList,
	[SCENE_CMD_SKYBOX_SETTINGS] =       WriteSceneCmd_SkyboxSettings,
	[SCENE_CMD_EXIT_LIST] =             WriteSceneCmd_ExitList,
	[SCENE_CMD_SOUND_SETTINGS] =        WriteSceneCmd_SoundSettings,
	[SCENE_CMD_CUTSCENE_DATA] =         WriteSceneCmd_CutsceneData,
	[SCENE_CMD_MISC_SETTINGS] =         WriteSceneCmd_MiscSettings,
	[SCENE_CMD_RENDER_INIT] =           WriteSceneCmd_RenderInit,
};

static void (*sRoomCmdWrite[])(Memfile*, Scene*, RoomHeader*, u8) = {
	[SCENE_CMD_ALTERNATE_HEADER_LIST] = WriteRoomCmd_AlternateHeaderList,
	[SCENE_CMD_END] =                   (void*)WriteEndCmd,
	
	[SCENE_CMD_ACTOR_LIST] =            WriteRoomCmd_ActorList,
	[SCENE_CMD_WIND_SETTINGS] =         WriteRoomCmd_WindSettings,
	[SCENE_CMD_ROOM_BEHAVIOR] =         WriteRoomCmd_RoomBehaviour,
	[SCENE_CMD_UNDEFINED_9] =           WriteRoomCmd_Undefined9,
	[SCENE_CMD_MESH_HEADER] =           WriteRoomCmd_MeshHeader,
	[SCENE_CMD_OBJECT_LIST] =           WriteRoomCmd_ObjectList,
	[SCENE_CMD_LIGHT_LIST] =            WriteRoomCmd_LightList,
	[SCENE_CMD_TIME_SETTINGS] =         WriteRoomCmd_TimeSettings,
	[SCENE_CMD_SKYBOX_DISABLES] =       WriteRoomCmd_SkyboxDisables,
	[SCENE_CMD_ECHO_SETTINGS] =         WriteRoomCmd_EchoSettings,
};

///////////////////////////////////////////////////////////////////////////////

Export* Export_New(Scene* scene, const char* file) {
	Export* this = new(Export);
	
	this->basename = basename(file);
	this->path = strdup(x_path(file));
	this->ref = DispRefList_GatherRef(scene);
	this->scene = scene;
	
	g64.prefix = fmt("g%s", x_externify(x_basename(file)));
	
	this->room = new(struct ExportRoom[scene->numRoom]);
	
	return this;
}

///////////////////////////////////////////////////////////////////////////////

static void Export_WriteColHeader(Memfile* main, Scene* scene) {
	gSegment[2] = scene->segment;
	Memfile* collision = Memfile_NewSym(main, SYM_NAME("ColHeader"), "CollisionHeader", 4);
	CollisionHeader* colHeader = SEGMENTED_TO_VIRTUAL(scene->colSegment);
	Memfile* vtxList = Memfile_NewSym(collision, SYM_NAME("ColVtxList"), "Vec3s", 4);
	Memfile* polyList = Memfile_NewSym(collision, SYM_NAME("ColPolyList"), "CollisionPoly", 4);
	Memfile* surfList = Memfile_NewSym(collision, SYM_NAME("ColSurfaceList"), "SurfaceType", 4);
	Memfile* camList = Memfile_NewSym(collision, SYM_NAME("ColCamList"), "BgCamInfo", 4);
	Memfile* waterList = Memfile_NewSym(collision, SYM_NAME("ColWaterBoxList"), "WaterBox", 4);
	
	Memfile_Write(collision, &colHeader->minBounds, sizeof(Vec3s));
	Memfile_Write(collision, &colHeader->maxBounds, sizeof(Vec3s));
	Memfile_Write(collision, pval16(ReadBE(colHeader->numVertices)), sizeof(u16));
	Memfile_WritePad(collision, 2);
	Memfile_WriteRef(collision, vtxList, SYM_32);
	Memfile_Write(collision, pval16(ReadBE(colHeader->numPolygons)), sizeof(u16));
	Memfile_WritePad(collision, 2);
	Memfile_WriteRef(collision, polyList, SYM_32);
	Memfile_WriteRef(collision, surfList, SYM_32);
	Memfile_WriteRef(collision, camList, SYM_32);
	Memfile_Write(collision, pval16( ReadBE(colHeader->numWaterBoxes)), sizeof(u16));
	Memfile_WritePad(collision, 2);
	Memfile_WriteRef(collision, waterList, SYM_32);
	
	Memfile_Write(vtxList, SEGMENTED_TO_VIRTUAL(colHeader->vtxList32), sizeof(s16[colHeader->numVertices * 3]));
	
	if (colHeader->numWaterBoxes)
		Memfile_Write(waterList, SEGMENTED_TO_VIRTUAL(colHeader->waterBoxe32s), 0x10 * colHeader->numWaterBoxes);
	
	typedef struct StructBE {
		u16 type;
		union {
			u16 vtxData[3];
			struct {
				u16 flags_vIA;
				u16 flags_vIB;
				u16 vIC;
			};
		};
		Vec3s normal;
		s16   dist;
	} CollisionPoly;
	
	CollisionPoly* poly = SEGMENTED_TO_VIRTUAL(colHeader->polyList32);
	
	Memfile_Write(polyList, poly, sizeof(CollisionPoly[colHeader->numPolygons]));
	
	u16 surfaceNum = 0;
	u8 camNum = 1;
	
	for (int i = 0; i < colHeader->numPolygons; i++, poly++)
		surfaceNum = Max(surfaceNum, poly->type + 1);
	
	u32* surface = SEGMENTED_TO_VIRTUAL(colHeader->surfaceTypeList32);
	
	Memfile_Write(surfList, surface, sizeof(u32[surfaceNum * 2]));
	
	for (int i = 0; i < surfaceNum; i++, surface += 2) {
		u32 v = ReadBE(*surface);
		
		camNum = Max(camNum, rmask(v, 0xFF) + 1);
	}
	
	for (int k = 0; k < scene->numHeader; k++) {
		for (int i = 0; i < scene->header[i].spawnList.num; i++) {
			SpawnActor* spawn = Arli_At(&scene->header[i].spawnList, i);
			u8 id = spawn->actor.param;
			
			if (id != 0xFF) camNum = Max(camNum, id + 1);
		}
	}
	
	typedef struct StructBE {
		u16 setting;
		s16 count;
		u32 bgCamFuncData32;
	} BgCamInfo;
	
	typedef struct StructBE {
		Vec3s pos;
		Vec3s rot;
		s16   fov;
		union {
			s16 roomImageOverrideBgCamIndex;
			s16 timer;
			s16 flags;
		};
		s16 unk_10;
	} BgCamFuncData;
	
	BgCamInfo* camInfo = SEGMENTED_TO_VIRTUAL(colHeader->bgCamList32);
	
	for (int i = 0; i < camNum; i++, camInfo++) {
		_log("%d / %d", i + 1, camNum);
		Memfile* data = Memfile_NewSym(camList, SYM_NAME("ColBgCamData%d", i), "BgCamFuncData", 4);
		
		Memfile_Write(camList, camInfo, sizeof(u16[2]));
		Memfile_WriteRef(camList, data, SYM_32);
		
		_log("write data from: %08X", camInfo->bgCamFuncData32);
		if (camInfo->bgCamFuncData32) {
			BgCamFuncData* camData = SEGMENTED_TO_VIRTUAL(camInfo->bgCamFuncData32);
			
			Memfile_Write(data, camData, sizeof(BgCamFuncData));
			Memfile_Align(data, 4);
		}
	}
}

static void Export_WriteDispRefList(Memfile* main, DispRefList* list) {
	DispRef* ref = list->ref;
	
	for (int i = 0; i < list->num; i++, ref++) {
		const char* name;
		const char* imName[] = {
			"RGBA", "YUV", "CI", "IA", "I"
		};
		const char* sizName[] = {
			"4", "8", "16", "32"
		};
		
		if (ref->palette)
			name = SYM_NAME("Pal%08X", ref->segment);
		else
			name = SYM_NAME("Tex%08X_%s%s", ref->segment, imName[ref->fmt], sizName[ref->bitsiz]);
		
		Memfile* data = Memfile_NewSym(main, name, "u8", 8);
		
		Memfile_Write(data, ref->data, ref->size);
	}
}

static void Export_UpdateDispRefList(Memfile* main, DispRefList* list) {
	DispRef* ref = list->ref;
	
	for (int i = 0; i < list->num; i++, ref++) {
		const char* name;
		const char* imName[] = {
			"RGBA", "YUV", "CI", "IA", "I"
		};
		const char* sizName[] = {
			"4", "8", "16", "32"
		};
		
		if (ref->palette)
			name = SYM_NAME("Pal%08X", ref->segment);
		else
			name = SYM_NAME("Tex%08X_%s%s", ref->segment, imName[ref->fmt], sizName[ref->bitsiz]);
		
		ref->new = Memfile_GetSymOffset(main, name);
		
		if (ref->new < 0) {
			warn("No new reference found to %s", name);
			ref->new = 0;
		} else
			ref->new |= 0x02000000;
	}
}

///////////////////////////////////////////////////////////////////////////////

static void Callback_SceneSegment(void* udata, u64* offset) {
	u32* v = (void*)offset;
	
	*v |= 0x02000000;
	
	if (!udata) SwapBE(*v);
}

static void Callback_RoomSegment(void* udata, u64* offset) {
	u32* v = (void*)offset;
	
	*v |= 0x03000000;
	if (!udata) SwapBE(*v);
}

static int Callback_TypeSize(void* _, const char* type, size_t size) {
	f32 sz = 0;
	
	if (streq(type, "u8"))
		sz = size;
	else if (streq(type, "u16"))
		sz = (f32)size / 2;
	else if (streq(type, "u32"))
		sz = (f32)size / 4;
	else if (streq(type, "ActorEntry"))
		sz = (f32)size / 0x10;
	else if (streq(type, "SCmdBase*"))
		sz = (f32)size / 4;
	else if (streq(type, "Vec3s"))
		sz = (f32)size / sizeof(s16[3]);
	else if (streq(type, "RomFile"))
		sz = (f32)size / sizeof(u32[2]);
	else if (streq(type, "ActorEntry"))
		sz = (f32)size / sizeof(ActorEntry);
	else if (streq(type, "CollisionHeader"))
		sz = (f32)size / 0x2C;
	else if (streq(type, "CollisionPoly"))
		sz = (f32)size / 0x10;
	else if (streq(type, "SurfaceType"))
		sz = (f32)size / sizeof(u32[2]);
	else if (streq(type, "BgCamInfo"))
		sz = (f32)size / 0x8;
	else if (streq(type, "EntranceEntry"))
		sz = (f32)size / 0x2;
	else if (streq(type, "TransitionActorEntry"))
		sz = (f32)size / 0x10;
	else if (streq(type, "LightSettings"))
		sz = (f32)size / 0x16;
	else if (streq(type, "RoomShapeNormal"))
		sz = (f32)size / 0xC;
	else if (streq(type, "RoomShapeDListsEntry"))
		sz = (f32)size / 0x8;
	else if (streq(type, "BgCamFuncData"))
		sz = 1;
	
	if (wrapf(sz, 0, 1) != 0.0f)
		warn("Irregular size: %s 0x%08X", type, size);
	
	return rint(sz);
}

///////////////////////////////////////////////////////////////////////////////
#define HEADER_NUM    this->scene->numHeader
#define SCENE_CMD_NUM ArrCount(sSceneCommands)
#define ROOM_NUM      this->scene->numRoom
#define ROOM_CMD_NUM  ArrCount(sRoomCommands)

const char* Export_Scene(Export* this) {
	
	this->main = Memfile_New();
	
	for (int k = 0; k < HEADER_NUM; k++)
		this->header[k] = Memfile_NewSym(&this->main, x_fmt("%s_Set%dHead", g64.prefix, k), NULL, 4);
	
	for (int k = 0; k < HEADER_NUM; k++) {
		if (this->scene->headerInherit[k] > -1)
			continue;
		
		for (int i = 0; i < SCENE_CMD_NUM; i++) {
			int index = sSceneCommands[i];
			
			_log("SceneCommand(%s)", sSceneCmdNames[index]);
			sSceneCmdWrite[index](
				this->header[k],
				this->scene,
				Scene_GetSceneHeader(this->scene, k),
				k);
		}
	}
	
	Export_WriteColHeader(&this->main, this->scene);
	Export_WriteDispRefList(&this->main, &this->ref);
	
	Memfile_Link(&this->main, SYM_O_MERGE_IDENTICAL, (void*)Callback_SceneSegment, NULL);
	const char* sym = Memfile_PrintSymLinker(&this->main, (void*)Callback_SceneSegment, this->scene);
	const char* header = Memfile_PrintSymHeader(&this->main, (void*)Callback_TypeSize, NULL);
	puts(sym);
	puts(header);
	
	delete(sym, header);
	
	Export_UpdateDispRefList(&this->main, &this->ref);
	
	Memfile_SaveBin(&this->main, this->path);
	
	DispRefList_Print(&this->ref);
	
	return rSuccess;
}

#include "dlcopy/src/displaylist.h"

const char* Export_Room(Export* this, int roomID) {
	struct ExportRoom* ctx = &this->room[roomID];
	Room* room = &this->scene->room[roomID];
	Arli meshPointer = Arli_New(RoomMesh*);
	
	ctx->used = true;
	ctx->main = Memfile_New();
	
	for (int k = 0; k < HEADER_NUM; k++) {
		ctx->header[k] = Memfile_NewSym(&ctx->main, x_fmt("%s_Room%d_Set%dHead", g64.prefix, roomID, k), NULL, 4);
		
		if (!Arli_Find(&meshPointer, &room->header[k].mesh))
			Arli_Add(&meshPointer, &room->header[k].mesh);
	}
	
	for (int k = 0; k < HEADER_NUM; k++) {
		if (this->scene->headerInherit[k] > -1) {
			_log("INHERIT: %d -> %d", k, this->scene->headerInherit[k]);
			continue;
		}
		
		for (int i = 0; i < ROOM_CMD_NUM; i++) {
			int index = sRoomCommands[i];
			
			_log("Room%dCommand(%s)", roomID, sSceneCmdNames[index]);
			sRoomCmdWrite[index](ctx->header[k], this->scene, &room->header[k], k);
		}
	}
	
	for (int k = 0; k < meshPointer.num; k++) {
		RoomMesh* mesh = *((RoomMesh**)Arli_At(&meshPointer, k));
		_log("Write RoomMesh%d %08X", k, mesh->uuid);
		
		Memfile* list = Memfile_NewSym(&ctx->main, SYM_NAME("RoomMeshHeader%08X", mesh->uuid), "RoomShapeNormal", 4);
		Memfile* disp = Memfile_NewSym(list, SYM_NAME("RoomMeshList%08X", mesh->uuid), "RoomShapeDListsEntry", 4);
		
		Memfile_Write(list, pval8(0), 1);
		Memfile_Write(list, pval8(mesh->disp.num), 1);
		Memfile_WritePad(list, 2);
		Memfile_WriteRef(list, disp, SYM_32);
		Memfile_WritePad(list, 4);
		
		u32* opa = mesh->disp.opa;
		u32* xlu = mesh->disp.xlu;
		
		for (int i = 0; i < mesh->disp.num; i++, opa++, xlu++) {
			Memfile_Write(disp, pval32(ReadBE(*opa)), 4);
			Memfile_Write(disp, pval32(ReadBE(*xlu)), 4);
		}
	}
	
	Memfile_Link(&ctx->main, SYM_O_MERGE_IDENTICAL, (void*)Callback_RoomSegment, NULL);
	const char* sym = Memfile_PrintSymLinker(&ctx->main, (void*)Callback_RoomSegment, this->scene);
	const char* header = Memfile_PrintSymHeader(&ctx->main, (void*)Callback_TypeSize, NULL);
	puts(sym);
	puts(header);
	
	delete(sym, header);
	
	Arli procOffset = Arli_New(u32);
	Arli newOffset = Arli_New(u32);
	
	Memfile_Realloc(&ctx->main, ctx->main.size + MbToBin(2));
	Memfile_Align(&ctx->main, 8);
	
	for (int k = 0; k < meshPointer.num; k++) {
		RoomMesh* mesh = *((RoomMesh**)Arli_At(&meshPointer, k));
		const char* symName = SYM_NAME("RoomMeshList%08X", mesh->uuid);
		off_t offset = Memfile_GetSymOffset(&ctx->main, symName);
		
		if (offset < 0) {
			warn("Couldn't find sym: %s", symName);
			
			continue;
		}
		
		_log("processing: %s", symName);
		
		typedef struct StructBE {
			u32 opa;
			u32 xlu;
		} Disp;
		
		Disp* dtbl = Memfile_Seek(&ctx->main, offset);
		
		ZObj source = {
			.buffer        = mesh->segment,
			.segmentNumber = 3,
			.capacity      = mesh->sizeSegment,
			.limit         = mesh->sizeSegment,
		};
		ZObj dst = {
			.buffer        = ctx->main.data,
			.segmentNumber = 3,
			.capacity      = ctx->main.memSize,
			.limit         = ctx->main.size,
		};
		
		for (int i = 0; i < mesh->disp.num; i++, dtbl++) {
			u32 opa = 0;
			u32 xlu = 0;
			u32* p;
			
			#define PROCESS_DISP(DISP) do { \
						if (dtbl->DISP) { \
							if ((p = Arli_Find(&procOffset, pval32(dtbl->DISP)))) { \
								warn("already copied: %08X", dtbl->DISP); \
								int index = Arli_IndexOf(&procOffset, p); \
								DISP = *((u32*)Arli_At(&newOffset, index)); \
							} \
							else { \
								if (DisplayList_Copy(&source, dtbl->DISP, &dst, &DISP)) { \
									warn("DlCopy(%08X): %s", dtbl->DISP, DisplayList_ErrMsg()); \
									DISP = 0; \
								} else { \
									Arli_Add(&procOffset, pval32(dtbl->DISP)); \
									Arli_Add(&newOffset, pval32(DISP)); \
								} \
							} \
						} \
			} \
					while (0)
			
			PROCESS_DISP(opa);
			PROCESS_DISP(xlu);
			
			dtbl->opa = opa;
			dtbl->xlu = xlu;
		}
		
		ctx->main.size = dst.limit;
	}
	
	gSegment[3] = ctx->main.data;
	
	for (int i = 0; i < newOffset.num; i++) {
		u32* offset = Arli_At(&newOffset, i);
		
		DisplayRefList_UpdateRef(&this->ref, SEGMENTED_TO_VIRTUAL(*offset));
	}
	
	Arli_Free(&procOffset);
	Arli_Free(&newOffset);
	Arli_Free(&meshPointer);
	
	Memfile_SaveBin(&ctx->main, x_fmt("%s/room_%d.zroom", this->path, roomID));
	
	return rSuccess;
}

void Export_Free(Export* this) {
	Memfile_Free(&this->main);
	
	for (int i = 0; i < this->scene->numRoom; i++)
		Memfile_Free(&this->room[i].main);
	
	DispRefList_Free(&this->ref);
	delete(this->room, this->path, g64.prefix, this);
}
