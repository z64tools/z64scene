#ifndef __Z64_SCENE_H__
#define __Z64_SCENE_H__

#include <ext_lib.h>
#include <ext_geogrid.h>
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
    struct {
        u32* opa;
        u32* xlu;
        u32  num;
    } disp;
    
    u8*       segment;
    TriBuffer triBuf;
    
    Vec3f center;
    f32   size;
} RoomMesh;

typedef struct RoomHeader {
    RoomMesh* mesh;
    
    struct {
        Actor entry[255];
        u32   num;
    } actorList;
    
    struct {
        Vec3s dir;
        s16   strength;
    } wind;
    
    struct {
        u8   behaviour;
        u8   linkIdleAnim;
        bool lensMode;
        bool disableWarpSongs;
    } behaviour;
    
    struct {
        LightParams entry[64];
        u32 num;
    } roomLight;
    
    struct {
        u8   type;
        u8   config;
        u8   lightMode;
        bool disableSky;
        bool disableSunMoon;
    } skyBox;
    
    u8 echo;
} RoomHeader;

typedef struct SceneHeader {
    struct {
        SpawnActor entry[64];
        u32 num;
    } spawnList;
    
    struct {
        PathList entry[64];
        u32      num;
    } pathList;
    
    struct {
        TransitionActor entry[255];
        u32 num;
    } transitionList;
    
    struct {
        EnvLightSettings entry[255];
        u32      num;
        PropList prop;
    } envList;
    
    struct {
        u8 hour;
        u8 min;
        u8 speed;
    } time;
    
    struct {
        u16 exit[32];
        u32 num;
    } exitList;
    
    struct {
        u8 specID;
        u8 bgmID;
        u8 sfxID;
    } sound;
    
    u8  camType;
    u8  area;
    u8  naviMsg;
    u16 keepObject;
} SceneHeader;

typedef struct Room {
    const u32  id;
    RoomState  state;
    RoomHeader header[0x10];
} Room;

typedef struct Scene {
    u8* segment;
    
    SceneState  state;
    SceneHeader header[0x10];
    Room room[32];
    
    struct {
        ElCombo  roomCombo;
        PropList roomList;
    } ui;
    
    u8 numHeader;
    u8 curHeader;
    
    u8 numRoom;
    u8 curRoom;
    u8 curEnv;
    
    Actor* curActor;
    Actor* prevActor[64];
    
    CollisionMesh colMesh;
    AnimOoT       animOoT;
    bool  kill;
    Gizmo gizmo;
    
    struct {
        RoomMesh entry[255];
        u32      num;
        Vec3f    rayPos;
        bool     rayHit;
    } mesh;
} Scene;

#define Scene_NewEntry(this) & (this).entry[(this).num++]

extern void (*sSceneDrawConfigs[53])(AnimOoT* this);

void Scene_Init(Scene* this);
void Scene_LoadScene(Scene* this, const char* file);
void Scene_LoadRoom(Scene* this, const char* file);
void Scene_Kill(Scene* this);
void Scene_Free(Scene* this);

void Scene_Update(Scene* this, View3D* view);
void Scene_Draw(Scene* this, View3D* view);

void Scene_CacheBuild(Scene* this);

SceneHeader* Scene_GetSceneHeader(Scene* this);
RoomHeader* Scene_GetRoomHeader(Scene* this, u8 num);
void Scene_SetState(Scene* this, SceneState state, bool set);

void Scene_SetRoom(Scene* this, s32 roomID);

void Room_Draw(RoomMesh* roomMesh);
Room* Scene_RaycastRoom(Scene* scene, RayLine* ray, Vec3f* out);

#endif /* __Z64_SCENE_H__ */