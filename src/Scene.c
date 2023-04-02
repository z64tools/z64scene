#include "Editor.h"
#include "BgCheck.h"
#include <ext_zip.h>

#if 1
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
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ u32 segment;
} SCmdSpawnList;

typedef struct StructBE {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  num;
    /* 0x04 */ u32 segment;
} SCmdActorList;

typedef struct StructBE {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ u32 segment;
} SCmdUnused02;

typedef struct StructBE {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ u32 segment;
} SCmdColHeader;

typedef struct StructBE {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  num;
    /* 0x04 */ u32 segment;
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
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ u32 segment;
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
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ u32 segment;
} SCmdMesh;

typedef struct StructBE {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  num;
    /* 0x04 */ u32 segment;
} SCmdObjectList;

typedef struct StructBE {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  num;
    /* 0x04 */ u32 segment;
} SCmdLightList;

typedef struct StructBE {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ u32 segment;
} SCmdPathList;

typedef struct StructBE {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  num;
    /* 0x04 */ u32 segment;
} SCmdTransiActorList;

typedef struct StructBE {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  num;
    /* 0x04 */ u32 segment;
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
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ u32 segment;
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
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ u32 segment;
} SCmdCutsceneData;

typedef struct StructBE {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ u32 segment;
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
    u8  numEntries;
    u32 dListStart;
    u32 dListEnd;
} MeshHeader0;

typedef struct StructBE {
    u32 opaqueDList;
    u32 translucentDList;
} MeshEntry0;

typedef struct StructBE {
    MeshHeaderBase base;
    u8  format;
    u32 entryRecord;
} MeshHeader1Base;

typedef struct StructBE {
    MeshHeader1Base base;
    u32 imagePtr;              // 0x08
    u32 unknown;               // 0x0C
    u32 unknown2;              // 0x10
    u16 bgWidth;               // 0x14
    u16 bgHeight;              // 0x16
    u8  imageFormat;           // 0x18
    u8  imageSize;             // 0x19
    u16 imagePal;              // 0x1A
    u16 imageFlip;             // 0x1C
} MeshHeader1Single;

typedef struct StructBE {
    MeshHeader1Base base;
    u8  bgCnt;
    u32 bgRecordPtr;
} MeshHeader1Multi;

typedef struct StructBE {
    u16 unknown;               // 0x00
    s8  bgID;                  // 0x02
    u32 imagePtr; // 0x04
    u32 unknown2;              // 0x08
    u32 unknown3;              // 0x0C
    u16 bgWidth;               // 0x10
    u16 bgHeight;              // 0x12
    u8  imageFmt;              // 0x14
    u8  imageSize;             // 0x15
    u16 imagePal;              // 0x16
    u16 imageFlip;             // 0x18
} BackgroundRecord;

typedef struct StructBE {
    s16 playerXMax, playerZMax;
    s16 playerXMin, playerZMin;
    u32 opaqueDList;
    u32 translucentDList;
} MeshEntry2;

typedef struct StructBE {
    MeshHeaderBase base;
    u8  numEntries;
    u32 dListStart;
    u32 dListEnd;
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
    /* 0x00 */ u8  count;      // number of points in the path
    /* 0x04 */ u32 points; // Segment Address to the array of points
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
    /* 0x00 */ u32 opa;
    /* 0x04 */ u32 xlu;
} PolygonDlist; // size = 0x8

typedef struct StructBE {
    /* 0x00 */ u8 type;
} PolygonBase;

typedef struct StructBE {
    /* 0x00 */ PolygonBase base;
    /* 0x01 */ u8  num;        // number of dlist entries
    /* 0x04 */ u32 start;
    /* 0x08 */ u32 end;
} PolygonType0; // size = 0xC

typedef struct StructBE {
    /* 0x00 */ Vec3s_BE pos;
    /* 0x06 */ s16      unk_06;
    /* 0x08 */ u32      opa;
    /* 0x0C */ u32      xlu;
} PolygonDlist2; // size = 0x8

typedef struct StructBE {
    /* 0x00 */ PolygonBase base;
    /* 0x01 */ u8  num;        // number of dlist entries
    /* 0x04 */ u32 start;
    /* 0x08 */ u32 end;
} PolygonType2; // size = 0xC

typedef union StructBE {
    PolygonBase  base;
    PolygonType0 polygon0;
    PolygonType2 polygon2;
} MeshHeader; // "Ground Shape"

static void Scene_Cmd00_SpawnList(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd01_ActorList(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd03_CollisionHeader(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd04_RoomList(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd05_Wind(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd06_EntranceList(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd07_SpecialFiles(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd08_RoomBehaviour(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd0A_MeshHeader(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd0B_ObjectList(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd0C_LightList(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd0D_PathList(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd0E_TransActorList(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd0F_EnvList(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd10_Time(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd11_Skybox(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd12_SkyboxDisables(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd13_ExitList(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd15_Sound(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd16_Echo(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd17_Cutscene(Scene* scene, RoomHeader* room, SceneCmd* cmd);
static void Scene_Cmd19_Misc(Scene* scene, RoomHeader* room, SceneCmd* cmd);
#endif

struct {
    void (*func)(Scene*, RoomHeader*, SceneCmd*);
    const char* name;
} sSceneCmds[] = {
    [0x00] = Scene_Cmd00_SpawnList,       "Scene_Cmd00_SpawnList",
    [0x01] = Scene_Cmd01_ActorList,       "Scene_Cmd01_ActorList",
    [0x02] = NULL,                        "NULL",
    [0x03] = Scene_Cmd03_CollisionHeader, "Scene_Cmd03_CollisionHeader",
    [0x04] = Scene_Cmd04_RoomList,        "Scene_Cmd04_RoomList",
    [0x05] = Scene_Cmd05_Wind,            "Scene_Cmd05_Wind",
    [0x06] = Scene_Cmd06_EntranceList,    "Scene_Cmd06_EntranceList",
    [0x07] = Scene_Cmd07_SpecialFiles,    "Scene_Cmd07_SpecialFiles",
    [0x08] = Scene_Cmd08_RoomBehaviour,   "Scene_Cmd08_RoomBehaviour",
    [0x09] = NULL,                        "NULL",
    [0x0A] = Scene_Cmd0A_MeshHeader,      "Scene_Cmd0A_MeshHeader",
    [0x0B] = Scene_Cmd0B_ObjectList,      "Scene_Cmd0B_ObjectList",
    [0x0C] = Scene_Cmd0C_LightList,       "Scene_Cmd0C_LightList",
    [0x0D] = Scene_Cmd0D_PathList,        "Scene_Cmd0D_PathList",
    [0x0E] = Scene_Cmd0E_TransActorList,  "Scene_Cmd0E_TransActorList",
    [0x0F] = Scene_Cmd0F_EnvList,         "Scene_Cmd0F_EnvList",
    [0x10] = Scene_Cmd10_Time,            "Scene_Cmd10_Time",
    [0x11] = Scene_Cmd11_Skybox,          "Scene_Cmd11_Skybox",
    [0x12] = Scene_Cmd12_SkyboxDisables,  "Scene_Cmd12_SkyboxDisables",
    [0x13] = Scene_Cmd13_ExitList,        "Scene_Cmd13_ExitList",
    [0x14] = NULL,                        (PRNT_REDD "Terminate"),
    [0x15] = Scene_Cmd15_Sound,           "Scene_Cmd15_Sound",
    [0x16] = Scene_Cmd16_Echo,            "Scene_Cmd16_Echo",
    [0x17] = Scene_Cmd17_Cutscene,        "Scene_Cmd17_Cutscene",
    [0x18] = NULL,                        (PRNT_REDD "Header"),
    [0x19] = Scene_Cmd19_Misc,            "Scene_Cmd19_Misc",
};

// # # # # # # # # # # # # # # # # # # # #
// # Scene                               #
// # # # # # # # # # # # # # # # # # # # #

void Scene_SaveProject(Scene* this, const char* file) {
    
}

void Scene_WriteToml(Scene* this) {
    Scene_LoadScene(this, "testmap/scene.zscene");
    for (var i = 0; i < 1; i++)
        Scene_LoadRoom(this, x_fmt("testmap/room_%d.zroom", i));
    
    Scene_SaveProject(this, "");
}

static void Scene_ExecuteCommands(Scene* this, Room* room) {
    u8* segment;
    SceneCmd* cmd;
    
    for (var header = 0; header < this->numHeader; header++) {
        gSegment[2] = this->segment;
        gSegment[3] = room ? room->segment : NULL;
        
        if (!room) {
            segment = gSegment[2];
            _log("" PRNT_YELW "SCENE");
        } else {
            segment = gSegment[3];
            _log("" PRNT_BLUE "ROOM");
        }
        
        cmd = (void*)segment;
        this->curHeader = header;
        
        if (header > 0) {
            _assert(cmd->base.code == SCENE_CMD_ID_ALTERNATE_HEADER_LIST);
            
            u32* headers = SEGMENTED_TO_VIRTUAL(cmd->altHeaders.segment);
            
            if (headers[header] == 0)
                continue;
            
            cmd = SEGMENTED_TO_VIRTUAL(ReadBE(headers[header]));
        }
        
        for (; cmd->base.code != SCENE_CMD_ID_END; cmd++) {
            if (cmd->base.code < SCENE_CMD_ID_MAX) {
                _log("[%02X] %08X [%s]", cmd->base.code, (u8*)cmd - segment, sSceneCmds[cmd->base.code].name);
                
                if (sSceneCmds[cmd->base.code].func)
                    sSceneCmds[cmd->base.code].func(this, &room->header[header], cmd);
                
                else if (cmd->base.code != SCENE_CMD_ID_ALTERNATE_HEADER_LIST)
                    warn("SceneCmd %02X", cmd->base.code);
            }
        }
    }
    
    this->curHeader = 0;
}

static void Scene_SetHeaderNum(Scene* this) {
    SceneCmd* cmd = gSegment[2] = this->segment;
    u32 headerOffset = 0;
    u32* cmdOffset = NULL;
    u32 numCmdOffset = 0;
    
    this->numHeader = 1;
    
    nested(void, ReadThrough, (SceneCmd * cmd)) {
        nested_var(headerOffset);
        nested_var(numCmdOffset);
        nested_var(cmdOffset);
        
        for (;; cmd++) {
            if (cmd->base.code == SCENE_CMD_ID_END)
                break;
            
            if (cmd->base.code == SCENE_CMD_ID_ALTERNATE_HEADER_LIST) {
                headerOffset = cmd->base.data2;
                
                continue;
            }
            
            switch ((u32)(cmd->base.data2 & 0xFFE00000)) {
                case 0x02000000:
                case 0x03000000:
                    cmdOffset = realloc(cmdOffset, sizeof(u32[numCmdOffset + 1]));
                    cmdOffset[numCmdOffset++] = cmd->base.data2;
                default:
                    (void)0;
            }
        }
    };
    nested(int, sort, (const void* void_a, const void* void_b)) {
        const u32* a = void_a;
        const u32* b = void_b;
        
        return *a - *b;
    };
    
    ReadThrough(cmd);
    
    if (headerOffset == 0) {
        vfree(cmdOffset);
        return;
    }
    
    _assert(cmdOffset != NULL);
    qsort(cmdOffset, numCmdOffset, sizeof(u32), (void*)sort);
    
    for (var i = 0; i < numCmdOffset; i++)
        info("%08X", cmdOffset[i]);
    
    for (var i = 0; i < numCmdOffset; i++) {
        if (cmdOffset[i] > headerOffset) {
            this->numHeader = (cmdOffset[i] - headerOffset) / sizeof(u32);
            info("%d Headers", this->numHeader);
            
            break;
        }
    }
    vfree(cmdOffset);
}

void Scene_Init(Scene* this) {
    
    for (var i = 0; i < ArrCount(this->room); i++) {
        u32* id = (u32*)&this->room[i];
        
        *id = i;
        
        for (var j = 0; j < ArrCount(this->room[i].header); j++) {
            RoomHeader* room = &this->room[i].header[j];
            
            room->roomLight = Arli_New(LightParams);
            Arli_Alloc(&room->roomLight, 64);
            
            room->actorList = Arli_New(Actor);
            Arli_Alloc(&room->actorList, 256);
            
            room->objectList = Arli_New(u16);
            Arli_Alloc(&room->objectList, 16);
            
            nested(const char*, ActorEntry_ElemName, (Arli * this, size_t index)) {
                Actor* actor = Arli_At(this, index);
                
                return x_fmt("%d: 0x%04X", index, actor->id);
            };
            
            nested(const char*, ObjectEntry_ElemName, (Arli * this, size_t index)) {
                u16* object = Arli_At(this, index);
                
                return x_fmt("%04X", *object);
            };
            
            Arli_SetElemNameCallback(&room->actorList, (void*)ActorEntry_ElemName);
            Arli_SetElemNameCallback(&room->objectList, (void*)ObjectEntry_ElemName);
        }
    }
    
    for (var i = 0; i < ArrCount(this->header); i++) {
        SceneHeader* scene = &this->header[i];
        
        scene->spawnList = Arli_New(SpawnActor);
        Arli_Alloc(&scene->spawnList, 64);
        
        scene->pathList = Arli_New(PathList);
        Arli_Alloc(&scene->pathList, 64);
        
        scene->transitionList = Arli_New(TransitionActor);
        Arli_Alloc(&scene->transitionList, 255);
        
        scene->envList = Arli_New(EnvLightSettings);
        Arli_Alloc(&scene->envList, 255);
        
        scene->exitList = Arli_New(u16);
        Arli_Alloc(&scene->exitList, 32);
    }
    
    this->ui.roomNameList = Arli_New(char[64]);
    Element_Combo_SetArli(&this->ui.roomCombo, &this->ui.roomNameList);
}

static void* Scene_ExObj_GetHeader(Memfile* file) {
    u8* data = &file->cast.u8[file->size - 0x30];
    
    for (var i = 0; i < 0x20; i += 0x10)
        if (!memcmp(data + i, "z64convert", 10))
            return data + i;
    
    return NULL;
}

void Scene_LoadScene_ExObj(Scene* this, const char* file) {
    Memfile mem = Memfile_New();
    
    Memfile_LoadBin(&mem, file);
    
    this->segment = mem.data;
    this->sizeSegment = mem.size;
    
    mem.data = NULL;
    Memfile_Free(&mem);
}

void Scene_LoadRoom_ExObj(Scene* this, const char* file) {
    Memfile mem = Memfile_New();
    
    // RoomMesh* mesh = Scene_NewEntry(this->mesh);
    // Room* room = &this->room[this->numRoom++];
    
    Memfile_LoadBin(&mem, file);
    
    PointerCast data = { Scene_ExObj_GetHeader(&mem) };
    
    if (data.p) {
        union {
            struct {
                u16 footer : 1;
                u16 names  : 1;
                u16 end    : (6 + 5);
                u16 skel   : 1;
                u16 col    : 1;
                u16 dl     : 1;
            };
            u16 _short;
        } mask;
        
        SegmentSet(0, mem.data);
        
        data.u8 += strlen("z64convert");
        mask._short = ReadBE(data.u16[0]);
        warn("footer:%d names:%d skel:%d col:%d dl:%d", mask.footer, mask.names, mask.skel, mask.col, mask.dl);
        errr("%04X", mask._short);
    }
    
    this->segment = mem.data;
    this->sizeSegment = mem.size;
    
    mem.data = NULL;
    Memfile_Free(&mem);
}

void Scene_LoadScene(Scene* this, const char* file) {
    Memfile mem = Memfile_New();
    
    Memfile_LoadBin(&mem, file);
    this->segment = mem.data;
    this->sizeSegment = mem.size;
    mem.data = NULL;
    Memfile_Free(&mem);
    
    Scene_SetHeaderNum(this);
    Scene_ExecuteCommands(this, NULL);
    
    this->state |= SCENE_DRAW_FOG | SCENE_DRAW_CULLING;
}

void Scene_LoadRoom(Scene* this, const char* file) {
    Memfile mem = Memfile_New();
    RoomMesh* mesh = Scene_NewEntry(this->mesh);
    Room* room = &this->room[this->numRoom++];
    
    Memfile_LoadBin(&mem, file);
    
    mesh->sizeSegment = mem.size;
    mesh->segment = mem.data;
    mesh->name = filename(file);
    
    room->segment = mem.data;
    mem.data = NULL;
    
    for (var i = 0; i < this->numHeader; i++)
        room->header[i].mesh = mesh;
    
    Memfile_Free(&mem);
    Scene_ExecuteCommands(this, room);
    
    *((u32*)&room->id) = this->numRoom - 1;
    
    Arli_Add(&this->ui.roomNameList, x_fmt("Room%02X", this->numRoom - 1));
}

void Scene_Kill(Scene* this) {
    this->kill = true;
}

void Scene_Free(Scene* this) {
    for (var i = 0; i < this->mesh.num; i++) {
        RoomMesh* mesh = &this->mesh.entry[i];
        
        TriBuffer_Free(&mesh->triBuf);
        vfree(mesh->segment, mesh->disp.opa, mesh->disp.xlu, mesh->name);
    }
    
    CollisionMesh_Free(&this->colMesh);
    vfree(this->segment);
    Arli_Free(&this->ui.roomNameList);
    
    memset(this, 0, sizeof(*this));
    n64_clearCache();
    Scene_Init(this);
}

static void Scene_Light(Scene* this) {
    // RoomHeader* roomHeader = Scene_GetRoomHeader(this, this->curRoom);
    SceneHeader* header = Scene_GetSceneHeader(this);
    s8 l1n[3], l2n[3];
    u16 fogNear;
    u8 curEnv = this->curEnv;
    Arli* envList = &header->envList;
    EnvLightSettings* env;
    
    env = Arli_At(envList, curEnv);
    _assert(env != NULL);
    
    env += curEnv;
    memcpy(l1n, env->light1Dir, 3);
    memcpy(l2n, env->light2Dir, 3);
    
    this->animOoT.nightFlag = false;
    if (/* this->indoorLight == false && */ curEnv < 4) {
        u16 time = 0;
        
        switch (curEnv) {
            case 0:
                time = 0x6000;
                this->animOoT.nightFlag = true;
                break;
                
            case 1:
                time = 0x8001;
                break;
                
            case 2:
                time = 0xB556;
                break;
                
            case 3:
                time = 0xFFFF;
                this->animOoT.nightFlag = true;
                break;
        }
        
        l1n[0] = SinS((time - 0x8000)) * 120.0f;
        l1n[1] = CosS((time - 0x8000)) * 120.0f;
        l1n[2] = CosS((time - 0x8000)) * 20.0f;
        l2n[0] = -l1n[0];
        l2n[1] = -l1n[1];
        l2n[2] = -l1n[2];
    }
    
    if (this->state & SCENE_DRAW_FOG)
        fogNear = env->fogNear & 0x3FF;
    else
        fogNear = 1000;
    
    Light_SetAmbLight(env->ambientColor);
    Light_SetFog(fogNear, 0, env->fogColor);
    Light_SetDirLight(l1n, env->light1Color);
    Light_SetDirLight(l2n, env->light2Color);
}

void Scene_Update(Scene* this, View3D* view) {
    RayLine r = View_GetCursorRayLine(view);
    
    this->mesh.rayHit = Scene_RaycastRoom(this, &r, &this->mesh.rayPos);
}

void Scene_Draw(Scene* this, View3D* view) {
    AnimOoT* animOoT = &this->animOoT;
    
    Scene_Light(this);
    
    if (animOoT->wait == false) {
        time_start(TIME_SCENE_ANIM);
        animOoT->wait = true;
    } else {
        if (time_get(TIME_SCENE_ANIM) > 1.0f / 20.0f) {
            animOoT->wait = false;
            animOoT->frame++;
        }
    }
    
    for (s32 i = 0; i < this->numRoom; i++) {
        RoomHeader* roomHdr = &this->room[i].header[this->curHeader];
        
        _log("RoomHeader %d", i);
        gSegment[2] = this->segment;
        gSPSegment(POLY_OPA_DISP++, 0x02, this->segment);
        gSPSegment(POLY_XLU_DISP++, 0x02, this->segment);
        
        if (sSceneDrawConfigs[this->animOoT.index])
            sSceneDrawConfigs[this->animOoT.index](&this->animOoT);
        
        if (i == this->curRoom && this->ui.glowFactor > EPSILON) {
            gXPSetHighlightColor(POLY_OPA_DISP++, 3, 252, 240, (u8)(0xFF * this->ui.glowFactor), DODGE);
            gXPSetHighlightColor(POLY_XLU_DISP++, 3, 252, 240, (u8)(0xFF * this->ui.glowFactor), DODGE);
        }
        
        Room_Draw(roomHdr->mesh);
        
        if (i == this->curRoom && this->ui.glowFactor > EPSILON) {
            gXPClearHighlightColor(POLY_OPA_DISP++);
            gXPClearHighlightColor(POLY_XLU_DISP++);
            Math_SmoothStepToF(&this->ui.glowFactor, 0.0f, 0.125f, 0.008f, 0.001f);
        }
        
        if (i == this->curRoom)
            Actor_Draw_RoomHeader(roomHdr, view);
        
        n64_draw_buffers();
    }
    
    if (this->state & SCENE_DRAW_COLLISION) {
        CollisionMesh_Draw(&this->colMesh);
        n64_draw_buffers();
    }
}

void Scene_CacheBuild(Scene* this) {
    MtxF mtx;
    
    nested(void, N64_TriangleCallback, (void* userData, const n64_triangleCallbackData * triData)) {
        RoomMesh* this = userData;
        Triangle* tri = &this->triBuf.head[this->triBuf.num];
        
        memcpy(tri->v, triData, sizeof(float) * 3 * 3);
        tri->cullBackface = triData->cullBackface;
        tri->cullFrontface = triData->cullFrontface;
        this->triBuf.num++;
        
        if (this->triBuf.num == this->triBuf.max)
            TriBuffer_Realloc(&this->triBuf);
    };
    
    Matrix_Push();
    Matrix_Scale(1.0, 1.0, 1.0, MTXMODE_NEW);
    Matrix_Get(&mtx);
    Matrix_Pop();
    
    n64_setMatrix_model(&mtx);
    n64_setMatrix_view(&mtx);
    n64_setMatrix_projection(&mtx);
    n64_set_culling(false);
    
    for (var i = 0 ; i < this->mesh.num; i++) {
        RoomMesh* mesh = &this->mesh.entry[i];
        TriBuffer_Alloc(&mesh->triBuf, 256);
        
        n64_graph_init();
        gSegment[2] = this->segment;
        gSPSegment(POLY_OPA_DISP++, 0x02, this->segment);
        gSPSegment(POLY_XLU_DISP++, 0x02, this->segment);
        
        Room_Draw(mesh);
        
        n64_set_triangleCallbackFunc(mesh, (void*)N64_TriangleCallback);
        n64_draw_buffers();
        
        info("RoomHeader TriCount: %d", mesh->triBuf.num);
        
        BoundBox box;
        f64 x = 0.0, y = 0.0, z = 0.0;
        
        for (var j = 0; j < mesh->triBuf.num; j++) {
            Triangle* tri = &mesh->triBuf.head[j];
            
            if (j == 0) {
                box = BoundBox_New3F(tri->v[0]);
                BoundBox_Adjust3F(&box, tri->v[1]);
                BoundBox_Adjust3F(&box, tri->v[2]);
            } else {
                BoundBox_Adjust3F(&box, tri->v[0]);
                BoundBox_Adjust3F(&box, tri->v[1]);
                BoundBox_Adjust3F(&box, tri->v[2]);
            }
            
            x += (tri->v[0].x + tri->v[1].x + tri->v[2].x) / 3;
            y += (tri->v[0].y + tri->v[1].y + tri->v[2].y) / 3;
            z += (tri->v[0].z + tri->v[1].z + tri->v[2].z) / 3;
        }
        
        mesh->center.x = x / mesh->triBuf.num;
        mesh->center.y = y / mesh->triBuf.num;
        mesh->center.z = z / mesh->triBuf.num;
        mesh->size = (box.xMax - box.xMin) + (box.yMax - box.yMin) + (box.zMax - box.zMin);
        mesh->size /= 3.0f;
    }
    
    n64_set_triangleCallbackFunc(0, 0);
}

SceneHeader* Scene_GetSceneHeader(Scene* this) {
    return &this->header[this->curHeader];
}

RoomHeader* Scene_GetRoomHeader(Scene* this, u8 num) {
    return &this->room[num].header[this->curHeader];
}

void Scene_SetState(Scene* this, SceneState state, bool set) {
    if (set)
        this->state |= state;
    else
        this->state &= ~state;
}

void Scene_SetRoom(Scene* this, s32 roomID) {
    Arli_Set(&this->ui.roomNameList, roomID);
    this->curRoom = roomID;
}

// # # # # # # # # # # # # # # # # # # # #
// # RoomHeader                          #
// # # # # # # # # # # # # # # # # # # # #

void Room_Draw(RoomMesh* this) {
    gSPDisplayList(POLY_OPA_DISP++, gSetupDList(0x19));
    gSPDisplayList(POLY_XLU_DISP++, gSetupDList(0x19));
    gDPSetEnvColor(POLY_OPA_DISP++, 0x80, 0x80, 0x80, 0x80);
    gDPSetEnvColor(POLY_XLU_DISP++, 0x80, 0x80, 0x80, 0x80);
    
    Matrix_Push();
    Matrix_Translate(0, 0, 0, MTXMODE_NEW);
    
    gSegment[3] = this->segment;
    gSPSegment(POLY_OPA_DISP++, 0x03, this->segment);
    gSPSegment(POLY_XLU_DISP++, 0x03, this->segment);
    gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPMatrix(POLY_XLU_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
    
    Matrix_Pop();
    
    for (s32 i = 0; i < this->disp.num; i++) {
        if (this->disp.opa[i] != 0)
            gSPDisplayList(POLY_OPA_DISP++, this->disp.opa[i]);
        
        if (this->disp.xlu[i] != 0)
            gSPDisplayList(POLY_XLU_DISP++, this->disp.xlu[i]);
    }
}

Room* Scene_RaycastRoom(Scene* scene, RayLine* ray, Vec3f* out) {
    s32 id = -1;
    
    for (s32 i = 0; i < scene->numRoom; i++) {
        RoomHeader* room = Scene_GetRoomHeader(scene, i);
        
        if (Col3D_LineVsTriBuffer(ray, &room->mesh->triBuf, out, NULL))
            id = i;
    }
    
    if (id < 0)
        return NULL;
    
    return &scene->room[id];
}

// # # # # # # # # # # # # # # # # # # # #
// # Commands                            #
// # # # # # # # # # # # # # # # # # # # #

static void Scene_Cmd00_SpawnList(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    ActorEntry* entryList = SEGMENTED_TO_VIRTUAL(cmd->spawnList.segment);
    SceneHeader* hdr = &scene->header[scene->curHeader];
    Arli* spawnList = &hdr->spawnList;
    
    Arli_Clear(spawnList);
    
    for (s32 i = 0; i < cmd->spawnList.data1; i++) {
        Arli_AddVar(spawnList, (SpawnActor) {
            .actor.id = entryList[i].id,
            .actor.param = entryList[i].param,
            .actor.pos = Math_Vec3f_New(UnfoldVec3(entryList[i].pos)),
            .actor.rot = Math_Vec3s_New(UnfoldVec3(entryList[i].rot)),
        });
    }
}

static void Scene_Cmd01_ActorList(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    ActorEntry* entryList = SEGMENTED_TO_VIRTUAL(cmd->actorList.segment);
    Arli* list = &room->actorList;
    
    Arli_Clear(list);
    Arli_Alloc(list, 255);
    
    for (s32 i = 0; i < cmd->actorList.num; i++) {
        Arli_AddVar(list, (Actor) {
            .id = entryList[i].id,
            .param = entryList[i].param,
            .pos = Math_Vec3f_New(UnfoldVec3(entryList[i].pos)),
            .rot = Math_Vec3s_New(UnfoldVec3(entryList[i].rot)),
        });
    }
}

static void Scene_Cmd03_CollisionHeader(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    if (scene->colMesh.vtxBuf != NULL)
        return;
    
    CollisionHeader* col = memdup(SEGMENTED_TO_VIRTUAL(cmd->colHeader.segment), sizeof(CollisionHeader));
    
    col->vtxList = SEGMENTED_TO_VIRTUAL(col->vtxList32);
    col->polyList = SEGMENTED_TO_VIRTUAL(col->polyList32);
    col->surfaceTypeList = SEGMENTED_TO_VIRTUAL(col->surfaceTypeList32);
    CollisionMesh_Generate(col, &scene->colMesh);
    vfree(col);
}

static void Scene_Cmd04_RoomList(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    // play->numRooms = cmd->roomList.num;
    // play->roomList = SEGMENTED_TO_VIRTUAL(cmd->roomList.segment);
}

static void Scene_Cmd05_Wind(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    room->wind.dir.x = (s8)cmd->windSettings.x;
    room->wind.dir.y = (s8)cmd->windSettings.y;
    room->wind.dir.z = (s8)cmd->windSettings.z;
    room->wind.strength = cmd->windSettings.unk_07;
}

static void Scene_Cmd06_EntranceList(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    struct {
        u8 spawn;
        u8 room;
    }* entranceList = SEGMENTED_TO_VIRTUAL(cmd->entranceList.segment);
    
    SceneHeader* hdr = &scene->header[scene->curHeader];
    
    if (hdr->spawnList.num == 0) {
        SceneCmd* ncmd = cmd;
        
        while (ncmd->base.code != 0x00 && ncmd->base.code != 0x14)
            ncmd++;
        
        _assert(ncmd->base.code != 0x14);
        
        Scene_Cmd00_SpawnList(scene, room, ncmd);
    }
    
    for (var i = 0; i < hdr->spawnList.num; i++) {
        SpawnActor* spawn = Arli_At(&hdr->spawnList, entranceList[i].spawn);
        
        _log("%d / %d", entranceList[i].spawn, hdr->spawnList.num);
        spawn->room = entranceList[i].room;
    }
    
    _log("OK");
}

static void Scene_Cmd07_SpecialFiles(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    scene->header[scene->curHeader].naviMsg = cmd->specialFiles.cUpElfMsgNum;
    scene->header[scene->curHeader].keepObject = cmd->specialFiles.keepObjectId;
}

static void Scene_Cmd08_RoomBehaviour(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    room->behaviour.behaviour = cmd->roomBehavior.gpFlag1;
    room->behaviour.linkIdleAnim = cmd->roomBehavior.gpFlag2 & 0xFF;
    room->behaviour.lensMode = (cmd->roomBehavior.gpFlag2 >> 8) & 1;
    room->behaviour.disableWarpSongs = (cmd->roomBehavior.gpFlag2 >> 0xA) & 1;
}

static void Scene_Cmd0A_MeshHeader(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    RoomMesh* mesh = room->mesh;
    MeshHeader* header = SEGMENTED_TO_VIRTUAL(cmd->mesh.segment);
    
    if (header->base.type == 0) {
        PolygonType0* polygon = &header->polygon0;
        PolygonDlist* polygonDlist = SEGMENTED_TO_VIRTUAL(polygon->start);
        
        mesh->disp.xlu = new(u32[polygon->num]);
        mesh->disp.opa = new(u32[polygon->num]);
        mesh->disp.num = polygon->num;
        _assert(mesh->disp.xlu && mesh->disp.opa);
        
        for (s32 i = 0; i < polygon->num; i++, polygonDlist++) {
            mesh->disp.opa[i] = polygonDlist->opa;
            mesh->disp.xlu[i] = polygonDlist->xlu;
        }
    } else if (header->base.type == 2) {
        PolygonType2* polygon = &header->polygon2;
        PolygonDlist2* polygonDlist = SEGMENTED_TO_VIRTUAL(polygon->start);
        
        mesh->disp.xlu = new(u32[polygon->num]);
        mesh->disp.opa = new(u32[polygon->num]);
        mesh->disp.num = polygon->num;
        _assert(mesh->disp.xlu && mesh->disp.opa);
        
        for (s32 i = 0; i < polygon->num; i++, polygonDlist++) {
            mesh->disp.opa[i] = polygonDlist->opa;
            mesh->disp.xlu[i] = polygonDlist->xlu;
        }
    }
}

static void Scene_Cmd0B_ObjectList(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    u16* objList = SEGMENTED_TO_VIRTUAL(cmd->objectList.segment);
    Arli* list = &room->objectList;
    
    Arli_Clear(list);
    
    for (var i = 0; i < cmd->objectList.num; i++)
        Arli_AddVar(list, (u16)ReadBE(objList[i]));
}

static void Scene_Cmd0C_LightList(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    // s32 i;
    // LightInfo* lightInfo = SEGMENTED_TO_VIRTUAL(cmd->lightList.segment);
    //
    // for (i = 0; i < cmd->lightList.num; i++) {
    //  LightContext_InsertLight(play, &play->lightCtx, lightInfo);
    //  lightInfo++;
    // }
}

static void Scene_Cmd0D_PathList(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    // play->setupPathList = SEGMENTED_TO_VIRTUAL(cmd->pathList.segment);
}

static void Scene_Cmd0E_TransActorList(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    // play->transiActorCtx.numActors = cmd->transiActorList.num;
    // play->transiActorCtx.list = SEGMENTED_TO_VIRTUAL(cmd->transiActorList.segment);
}

static void Scene_Cmd0F_EnvList(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    SceneHeader* header = &scene->header[scene->curHeader];
    EnvLightSettings* env = SEGMENTED_TO_VIRTUAL(cmd->lightSettingList.segment);
    Arli* list = &header->envList;
    
    Arli_Clear(list);
    Arli_Alloc(list, cmd->lightSettingList.num);
    Arli_AddX(list, cmd->lightSettingList.num, env);
}

static void Scene_Cmd10_Time(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    // if ((cmd->timeSettings.hour != 0xFF) && (cmd->timeSettings.min != 0xFF)) {
    //  gSaveContext.skyboxTime = gSaveContext.dayTime =
    //      ((cmd->timeSettings.hour + (cmd->timeSettings.min / 60.0f)) * 60.0f) / ((f32)(24 * 60) / 0x10000);
    // }
    //
    // if (cmd->timeSettings.unk_06 != 0xFF) {
    //  play->envCtx.sceneTimeSpeed = cmd->timeSettings.unk_06;
    // } else {
    //  play->envCtx.sceneTimeSpeed = 0;
    // }
    //
    // if (gSaveContext.sunsSongState == SUNSSONG_INACTIVE) {
    //  gTimeSpeed = play->envCtx.sceneTimeSpeed;
    // }
    //
    // play->envCtx.sunPos.x = -(Math_SinS(((void)0, gSaveContext.dayTime) - CLOCK_TIME(12, 0)) * 120.0f) * 25.0f;
    // play->envCtx.sunPos.y = (Math_CosS(((void)0, gSaveContext.dayTime) - CLOCK_TIME(12, 0)) * 120.0f) * 25.0f;
    // play->envCtx.sunPos.z = (Math_CosS(((void)0, gSaveContext.dayTime) - CLOCK_TIME(12, 0)) * 20.0f) * 25.0f;
    //
    // if (((play->envCtx.sceneTimeSpeed == 0) && (gSaveContext.cutsceneIndex < 0xFFF0)) ||
    //  (gSaveContext.entranceIndex == ENTR_SPOT06_8)) {
    //  gSaveContext.skyboxTime = ((void)0, gSaveContext.dayTime);
    //
    //  if ((gSaveContext.skyboxTime > CLOCK_TIME(4, 0)) && (gSaveContext.skyboxTime < CLOCK_TIME(6, 30))) {
    //      gSaveContext.skyboxTime = CLOCK_TIME(5, 0) + 1;
    //  } else if ((gSaveContext.skyboxTime >= CLOCK_TIME(6, 30)) && (gSaveContext.skyboxTime <= CLOCK_TIME(8, 0))) {
    //      gSaveContext.skyboxTime = CLOCK_TIME(8, 0) + 1;
    //  } else if ((gSaveContext.skyboxTime >= CLOCK_TIME(16, 0)) && (gSaveContext.skyboxTime <= CLOCK_TIME(17, 0))) {
    //      gSaveContext.skyboxTime = CLOCK_TIME(17, 0) + 1;
    //  } else if ((gSaveContext.skyboxTime >= CLOCK_TIME(18, 0) + 1) &&
    //      (gSaveContext.skyboxTime <= CLOCK_TIME(19, 0))) {
    //      gSaveContext.skyboxTime = CLOCK_TIME(19, 0) + 1;
    //  }
    // }
}

static void Scene_Cmd11_Skybox(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    // if (room) {
    //     RoomHeader* header = &room->header[scene->curHeader];
    //
    //     _assert(scene->curHeader < ArrCount(room->header));
    //
    //     // play->skyboxId = cmd->skyboxSettings.skyboxId;
    //     // play->envCtx.skyboxConfig = play->envCtx.changeSkyboxNextConfig = cmd->skyboxSettings.unk_05;
    //     // play->envCtx.lightMode = cmd->skyboxSettings.unk_06;
    //
    //     header->indoorLight = cmd->skyboxSettings.unk_06;
    // } else
    //     scene->indoorLight = cmd->skyboxSettings.unk_06;
}

static void Scene_Cmd12_SkyboxDisables(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    // play->envCtx.skyboxDisabled = cmd->skyboxDisables.unk_04;
    // play->envCtx.sunMoonDisabled = cmd->skyboxDisables.unk_05;
}

static void Scene_Cmd13_ExitList(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    u16* exitList = SEGMENTED_TO_VIRTUAL(cmd->exitList.segment);
    SceneHeader* header = Scene_GetSceneHeader(scene);
    
    Arli_Clear(&header->exitList);
    
    for (var i = 0;; i++) {
        u16 exit = ReadBE(exitList[i]);
        Arli_Add(&header->exitList, &exit);
        
        if (exit & 0xF000)
            break;
    }
}

static void Scene_Cmd15_Sound(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    scene->header[scene->curHeader].sound.bgmID = cmd->soundSettings.seqId;
    scene->header[scene->curHeader].sound.sfxID = cmd->soundSettings.natureAmbienceId;
    scene->header[scene->curHeader].sound.specID = cmd->soundSettings.specId;
}

static void Scene_Cmd16_Echo(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    room->echo = cmd->echoSettings.echo;
}

static void Scene_Cmd17_Cutscene(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    // osSyncPrintf("\ngame_play->demo_play.data=[%x]", play->csCtx.segment);
    // play->csCtx.segment = SEGMENTED_TO_VIRTUAL(cmd->cutsceneData.segment);
}

static void Scene_Cmd19_Misc(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    scene->header[scene->curHeader].camType = cmd->miscSettings.cameraMovement;
    scene->header[scene->curHeader].area = cmd->miscSettings.area;
}
