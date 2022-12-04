#include "Editor.h"
#include "BgCheck.h"

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
    void32 imagePtr;           // 0x08
    u32    unknown;            // 0x0C
    u32    unknown2;           // 0x10
    u16    bgWidth;            // 0x14
    u16    bgHeight;           // 0x16
    u8     imageFormat;        // 0x18
    u8     imageSize;          // 0x19
    u16    imagePal;           // 0x1A
    u16    imageFlip;          // 0x1C
} MeshHeader1Single;

typedef struct StructBE {
    MeshHeader1Base base;
    u8     bgCnt;
    void32 bgRecordPtr;
} MeshHeader1Multi;

typedef struct StructBE {
    u16    unknown;            // 0x00
    s8     bgID;               // 0x02
    void32 imagePtr; // 0x04
    u32    unknown2;           // 0x08
    u32    unknown3;           // 0x0C
    u16    bgWidth;            // 0x10
    u16    bgHeight;           // 0x12
    u8     imageFmt;           // 0x14
    u8     imageSize;          // 0x15
    u16    imagePal;           // 0x16
    u16    imageFlip;          // 0x18
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

static void Scene_ExecuteCommands(Scene* this, RoomHeader* room) {
    u8* segment;
    SceneCmd* cmd;
    
    gSegment[2] = this->segment;
    gSegment[3] = room ? room->mesh->segment : NULL;
    
    if (!room)
        segment = gSegment[2];
    else
        segment = gSegment[3];
    
    for (cmd = (void*)segment; cmd->base.code != SCENE_CMD_ID_END; cmd++) {
        if (cmd->base.code < SCENE_CMD_ID_MAX) {
            Log("[%02X] %08X [%s]", cmd->base.code, (u8*)cmd - segment, sSceneCmds[cmd->base.code].name);
            
            if (sSceneCmds[cmd->base.code].func)
                sSceneCmds[cmd->base.code].func(this, room, cmd);
            
            else if (cmd->base.code != SCENE_CMD_ID_ALTERNATE_HEADER_LIST)
                printf_warning("SceneCmd %02X", cmd->base.code);
        }
    }
}

static void Scene_SetHeaderNum(Scene* this) {
    SceneCmd* cmd = gSegment[2] = this->segment;
    
    this->numHeader = 1;
    
    for (;; cmd++) {
        if (cmd->base.code == SCENE_CMD_ID_ALTERNATE_HEADER_LIST) {
            break;
        }
        
        if (cmd->base.code == SCENE_CMD_ID_END)
            return;
    }
}

static bool Scene_OnRoomChange(PropList* list, PropListChange type, s32 index) {
    Scene* this = list->udata1;
    
    switch (type) {
        case PROP_SET:
            this->curRoom = index;
            
            break;
        case PROP_GET:
            break;
    }
    
    return true;
}

void Scene_Init(Scene* this) {
    
    // Init `Room` struct ID
    for (var i = 0; i < ArrayCount(this->room); i++) {
        u32* id = (u32*)&this->room[i];
        
        *id = i;
    }
    
    Element_Combo_SetPropList(&this->ui.roomCombo, &this->ui.roomList);
    PropList_SetOnChangeCallback(&this->ui.roomList, Scene_OnRoomChange, this, NULL);
}

void Scene_LoadScene(Scene* this, const char* file) {
    MemFile mem = MemFile_Initialize();
    
    MemFile_LoadFile(&mem, file);
    this->segment = mem.data;
    mem.data = NULL;
    MemFile_Free(&mem);
    
    Scene_SetHeaderNum(this);
    Scene_ExecuteCommands(this, NULL);
    
    this->state |= SCENE_DRAW_FOG | SCENE_DRAW_CULLING;
}

void Scene_LoadRoom(Scene* this, const char* file) {
    MemFile mem = MemFile_Initialize();
    RoomMesh* mesh = Scene_NewEntry(this->mesh);
    RoomHeader* room = &this->room[this->numRoom++].header[0];
    
    MemFile_LoadFile(&mem, file);
    mesh->segment = mem.data;
    room->mesh = mesh;
    mem.data = NULL;
    
    MemFile_Free(&mem);
    
    Scene_ExecuteCommands(this, room);
    
    PropList_Add(&this->ui.roomList, x_fmt("Room%02X", this->numRoom - 1));
}

void Scene_Kill(Scene* this) {
    this->kill = true;
}

void Scene_Free(Scene* this) {
    for (var i = 0; i < this->mesh.num; i++) {
        RoomMesh* mesh = &this->mesh.entry[i];
        
        TriBuffer_Free(&mesh->triBuf);
        Free(mesh->segment);
        Free(mesh->disp.opa);
        Free(mesh->disp.xlu);
    }
    
    CollisionMesh_Free(&this->colMesh);
    Free(this->segment);
    PropList_Free(&this->ui.roomList);
    
    memset(this, 0, sizeof(*this));
    n64_clearCache();
    Scene_Init(this);
}

static void Scene_Light(Scene* this) {
    // RoomHeader* roomHeader = Scene_GetRoomHeader(this, this->curRoom);
    SceneHeader* header = Scene_GetSceneHeader(this);
    EnvLightSettings* env = header->envList.entry;
    s8 l1n[3], l2n[3];
    u16 fogNear;
    u8 curEnv = this->curEnv;
    
    Assert(env != NULL);
    
    env += curEnv;
    memcpy(l1n, env->light1Dir, 3);
    memcpy(l2n, env->light2Dir, 3);
    
    this->animOoT.nightFlag = false;
    if (/* this->indoorLight == false && */ curEnv < 4) {
        u16 time;
        
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
    
    this->mesh.rayHit = Room_Raycast(this, &r, &this->mesh.rayPos);
}

void Scene_Draw(Scene* this, View3D* view) {
    AnimOoT* animOoT = &this->animOoT;
    
    Scene_Light(this);
    
    if (animOoT->wait == false) {
        Time_Start(TIME_SCENE_ANIM);
        animOoT->wait = true;
    } else {
        if (Time_Get(TIME_SCENE_ANIM) > 1.0f / 20.0f) {
            animOoT->wait = false;
            animOoT->frame++;
        }
    }
    
    for (s32 i = 0; i < this->numRoom; i++) {
        RoomHeader* roomHdr = &this->room[i].header[this->curHeader];
        
        Log("RoomHeader %d", i);
        gSegment[2] = this->segment;
        gSPSegment(POLY_OPA_DISP++, 0x02, this->segment);
        gSPSegment(POLY_XLU_DISP++, 0x02, this->segment);
        
        if (sSceneDrawConfigs[this->animOoT.index])
            sSceneDrawConfigs[this->animOoT.index](&this->animOoT);
        
        Room_Draw(roomHdr->mesh);
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
    
    void N64_TriangleCallback (void* userData, const n64_triangleCallbackData* triData) {
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
        
        n64_set_triangleCallbackFunc(mesh, N64_TriangleCallback);
        n64_draw_buffers();
        
        printf_info("RoomHeader TriCount: %d", mesh->triBuf.num);
        
        BoundBox box;
        f64 x = 0.0, y = 0.0, z = 0.0;
        
        for (var j = 0; j < mesh->triBuf.num; j++) {
            Triangle* tri = &mesh->triBuf.head[j];
            
            if (j == 0) {
                box = BoundBox_New(tri->v[0]);
                BoundBox_Adjust(&box, tri->v[1]);
                BoundBox_Adjust(&box, tri->v[2]);
            } else {
                BoundBox_Adjust(&box, tri->v[0]);
                BoundBox_Adjust(&box, tri->v[1]);
                BoundBox_Adjust(&box, tri->v[2]);
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
    PropList_Set(&this->ui.roomList, roomID);
}

// # # # # # # # # # # # # # # # # # # # #
// # RoomHeader                                #
// # # # # # # # # # # # # # # # # # # # #

void Room_Draw(RoomMesh* this) {
    gSPDisplayList(POLY_OPA_DISP++, gSetupDList(0x19));
    gDPSetEnvColor(POLY_OPA_DISP++, 0x80, 0x80, 0x80, 0x80);
    gSPDisplayList(POLY_XLU_DISP++, gSetupDList(0x19));
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

Room* Room_Raycast(Scene* scene, RayLine* ray, Vec3f* out) {
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
    SpawnActor* spawn = hdr->spawnList.entry;
    
    for (s32 i = 0; i < cmd->actorList.num; i++) {
        spawn[i].actor.id = entryList[i].id;
        spawn[i].actor.param = entryList[i].param;
        spawn[i].actor.pos = Math_Vec3f_New(UnfoldVec3(entryList[i].pos));
        spawn[i].actor.rot = Math_Vec3s_New(UnfoldVec3(entryList[i].rot));
    }
    
    hdr->spawnList.num = cmd->actorList.num;
}

static void Scene_Cmd01_ActorList(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    ActorEntry* entryList = SEGMENTED_TO_VIRTUAL(cmd->actorList.segment);
    Actor* actor = room->actorList.entry;
    
    for (s32 i = 0; i < cmd->actorList.num; i++) {
        actor[i].id = entryList[i].id;
        actor[i].param = entryList[i].param;
        actor[i].pos = Math_Vec3f_New(UnfoldVec3(entryList[i].pos));
        actor[i].rot = Math_Vec3s_New(UnfoldVec3(entryList[i].rot));
    }
    
    room->actorList.num = cmd->actorList.num;
}

static void Scene_Cmd03_CollisionHeader(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    CollisionHeader* col = MemDup(SEGMENTED_TO_VIRTUAL(cmd->colHeader.segment), sizeof(CollisionHeader));
    
    col->vtxList = SEGMENTED_TO_VIRTUAL(col->vtxList32);
    col->polyList = SEGMENTED_TO_VIRTUAL(col->polyList32);
    col->surfaceTypeList = SEGMENTED_TO_VIRTUAL(col->surfaceTypeList32);
    CollisionMesh_Generate(col, &scene->colMesh);
    Free(col);
}

static void Scene_Cmd04_RoomList(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    // play->numRooms = cmd->roomList.num;
    // play->roomList = SEGMENTED_TO_VIRTUAL(cmd->roomList.segment);
}

static void Scene_Cmd05_Wind(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
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

static void Scene_Cmd06_EntranceList(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    // play->setupEntranceList = SEGMENTED_TO_VIRTUAL(cmd->entranceList.segment);
}

static void Scene_Cmd07_SpecialFiles(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    // if (cmd->specialFiles.keepObjectId != OBJECT_INVALID) {
    //  play->objectCtx.subKeepIndex = Object_Spawn(&play->objectCtx, cmd->specialFiles.keepObjectId);
    //  gSegments[5] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[play->objectCtx.subKeepIndex].segment);
    // }
    //
    // if (cmd->specialFiles.cUpElfMsgNum != 0) {
    //  play->cUpElfMsgs = Play_LoadFile(play, &sNaviMsgFiles[cmd->specialFiles.cUpElfMsgNum - 1]);
    // }
}

static void Scene_Cmd08_RoomBehaviour(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    // play->roomCtx.curRoom.behaviorType1 = cmd->roomBehavior.gpFlag1;
    // play->roomCtx.curRoom.behaviorType2 = cmd->roomBehavior.gpFlag2 & 0xFF;
    // play->roomCtx.curRoom.lensMode = (cmd->roomBehavior.gpFlag2 >> 8) & 1;
    // play->msgCtx.disableWarpSongs = (cmd->roomBehavior.gpFlag2 >> 0xA) & 1;
}

static void Scene_Cmd0A_MeshHeader(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    RoomMesh* mesh = room->mesh;
    MeshHeader* header = SEGMENTED_TO_VIRTUAL(cmd->mesh.segment);
    
    if (header->base.type == 0) {
        PolygonType0* polygon = &header->polygon0;
        PolygonDlist* polygonDlist = SEGMENTED_TO_VIRTUAL(polygon->start);
        
        mesh->disp.xlu = New(u32[polygon->num]);
        mesh->disp.opa = New(u32[polygon->num]);
        mesh->disp.num = polygon->num;
        Assert(mesh->disp.xlu && mesh->disp.opa);
        
        for (s32 i = 0; i < polygon->num; i++, polygonDlist++) {
            mesh->disp.opa[i] = polygonDlist->opa;
            mesh->disp.xlu[i] = polygonDlist->xlu;
        }
    } else if (header->base.type == 2) {
        PolygonType2* polygon = &header->polygon2;
        PolygonDlist2* polygonDlist = SEGMENTED_TO_VIRTUAL(polygon->start);
        
        mesh->disp.xlu = New(u32[polygon->num]);
        mesh->disp.opa = New(u32[polygon->num]);
        mesh->disp.num = polygon->num;
        Assert(mesh->disp.xlu && mesh->disp.opa);
        
        for (s32 i = 0; i < polygon->num; i++, polygonDlist++) {
            mesh->disp.opa[i] = polygonDlist->opa;
            mesh->disp.xlu[i] = polygonDlist->xlu;
        }
    }
}

static void Scene_Cmd0B_ObjectList(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
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
    //  if (status->id != *objectEntry) {
    //      status2 = &play->objectCtx.status[i];
    //      for (j = i; j < play->objectCtx.num; j++) {
    //          status2->id = OBJECT_INVALID;
    //          status2++;
    //      }
    //      play->objectCtx.num = i;
    //      func_80031A28(play, &play->actorCtx);
    //
    //      continue;
    //  }
    //
    //  i++;
    //  k++;
    //  objectEntry++;
    //  status++;
    // }
    //
    // ASSERT(
    //  cmd->objectList.num <= OBJECT_EXCHANGE_BANK_MAX,
    //  "scene_info->object_bank.num <= OBJECT_EXCHANGE_BANK_MAX",
    //  "../z_scene.c",
    //  705
    // );
    //
    // while (k < cmd->objectList.num) {
    //  nextPtr = func_800982FC(&play->objectCtx, i, *objectEntry);
    //  if (i < OBJECT_EXCHANGE_BANK_MAX - 1) {
    //      firstStatus[i + 1].segment = nextPtr;
    //  }
    //  i++;
    //  k++;
    //  objectEntry++;
    // }
    //
    // play->objectCtx.num = i;
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
    
    memcpy(header->envList.entry, env, sizeof(EnvLightSettings) * cmd->lightSettingList.num);
    header->envList.num = cmd->lightSettingList.num;
    
    header->envList.prop = PropList_Init(0);
    for (var i = 0; i < header->envList.num; i++)
        PropList_Add(&header->envList.prop, x_fmt("Env%02X", i));
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
    //     Assert(scene->curHeader < ArrayCount(room->header));
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
    // play->setupExitList = SEGMENTED_TO_VIRTUAL(cmd->exitList.segment);
}

static void Scene_Cmd15_Sound(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    // play->sequenceCtx.seqId = cmd->soundSettings.seqId;
    // play->sequenceCtx.natureAmbienceId = cmd->soundSettings.natureAmbienceId;
    //
    // if (gSaveContext.seqId == (u8)NA_BGM_DISABLED) {
    //  Audio_QueueSeqCmd(cmd->soundSettings.specId | 0xF0000000);
    // }
}

static void Scene_Cmd16_Echo(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    // play->roomCtx.curRoom.echo = cmd->echoSettings.echo;
}

static void Scene_Cmd17_Cutscene(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    // osSyncPrintf("\ngame_play->demo_play.data=[%x]", play->csCtx.segment);
    // play->csCtx.segment = SEGMENTED_TO_VIRTUAL(cmd->cutsceneData.segment);
}

static void Scene_Cmd19_Misc(Scene* scene, RoomHeader* room, SceneCmd* cmd) {
    // YREG(15) = cmd->miscSettings.cameraMovement;
    // gSaveContext.worldMapArea = cmd->miscSettings.area;
    //
    // if ((play->sceneNum == SCENE_SHOP1) || (play->sceneNum == SCENE_SYATEKIJYOU)) {
    //  if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
    //      gSaveContext.worldMapArea = 1;
    //  }
    // }
    //
    // if (((play->sceneNum >= SCENE_SPOT00) && (play->sceneNum <= SCENE_GANON_TOU)) ||
    //  ((play->sceneNum >= SCENE_ENTRA) && (play->sceneNum <= SCENE_SHRINE_R))) {
    //  if (gSaveContext.cutsceneIndex < 0xFFF0) {
    //      gSaveContext.worldMapAreaData |= gBitFlags[gSaveContext.worldMapArea];
    //      osSyncPrintf(
    //          "０００  ａｒｅａ＿ａｒｒｉｖａｌ＝%x (%d)\n",
    //          gSaveContext.worldMapAreaData,
    //          gSaveContext.worldMapArea
    //      );
    //  }
    // }
}
