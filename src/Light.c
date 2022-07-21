#include "Editor.h"

void Light_SetLightInfo(LightInfo* info) {
	Light light;
	
	if (info->type != LIGHT_DIRECTIONAL)
		return;
	
	memcpy(light.dir.col, info->params.dir.color, 3);
	memcpy(light.dir.dir, info->params.dir.dir, 3);
	n64_bind_light(&light, NULL);
}

void Light_SetDirLight(s8 dir[3], u8 col[3]) {
	Light light = { 0 };
	
	memcpy(light.dir.col, col, 3);
	memcpy(light.dir.dir, dir, 3);
	n64_bind_light(&light, NULL);
}

void Light_SetAmbLight(u8 col[3]) {
	Ambient light = { 0 };
	
	memcpy(light.l.col, col, 3);
	n64_bind_light(NULL, &light);
}

void Light_SetFog(s32 near, s32 far, u8 col[3]) {
	s16 fogMultiply;
	s16 fogOffset;
	
	if (far == 0)
		far = 1000;
	
	if (near >= 1000) {
		fogMultiply = 0;
		fogOffset = 0;
	} else if (near >= 997) {
		fogMultiply = 32767;
		fogOffset = -32512;
	} else if (near < 0) {
		fogMultiply = 0;
		fogOffset = 255;
	} else {
		fogMultiply = ((500 * 0x100) / (far - near));
		fogOffset = ((500 - near) * 0x100 / (far - near));
	}
	
	//crustify
	n64_set_fog(
		(f32[2]) {
			fogMultiply, fogOffset
		},
	
		(f32[3]) {
			(f32)col[0] / 255,
			(f32)col[1] / 255,
			(f32)col[2] / 255,
		}
	);
	//uncrustify
}