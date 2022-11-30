#ifndef __Z64_LIGHT_H__
#define __Z64_LIGHT_H__

#include <ext_lib.h>
#include <ext_geogrid.h>
#include "Types.h"

typedef struct StructBE {
    /* 0x00 */ u8  ambientColor[3];
    /* 0x03 */ s8  light1Dir[3];
    /* 0x06 */ u8  light1Color[3];
    /* 0x09 */ s8  light2Dir[3];
    /* 0x0C */ u8  light2Color[3];
    /* 0x0F */ u8  fogColor[3];
    /* 0x12 */ s16 fogNear;
    /* 0x14 */ s16 fogFar;
} EnvLightSettings;

typedef struct {
    DataNode data;
    PropList list;
    EnvLightSettings env[255];
    u32 num;
} LightList;

typedef struct StructBE {
    /* 0x0 */ s16 x;
    /* 0x2 */ s16 y;
    /* 0x4 */ s16 z;
    /* 0x6 */ u8  color[3];
    /* 0x9 */ u8  drawGlow;
    /* 0xA */ s16 radius;
} LightPoint; // size = 0xC

typedef struct StructBE {
    /* 0x0 */ s8 dir[3];
    /* 0x3 */ u8 color[3];
} LightDirectional; // size = 0x6

typedef union StructBE {
    LightPoint       point;
    LightDirectional dir;
} LightParams; // size = 0xC

typedef struct StructBE {
    /* 0x0 */ u8 type;
    /* 0x2 */ LightParams params;
} LightInfo; // size = 0xE

typedef enum {
    /* 0x00 */ LIGHT_POINT_NOGLOW,
    /* 0x01 */ LIGHT_DIRECTIONAL,
    /* 0x02 */ LIGHT_POINT_GLOW
} LightType;

void Light_SetLightInfo(LightInfo* info);
void Light_SetDirLight(s8 dir[3], u8 col[3]);
void Light_SetAmbLight(u8 col[3]);
void Light_SetFog(s32 near, s32 far, u8 col[3]);

#endif