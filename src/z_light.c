#include "Editor.h"

void z64_Light_SetFog(s32 near, s32 far, RGB8 fogColor) {
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
			(f32)fogColor.r / 255,
			(f32)fogColor.g / 255,
			(f32)fogColor.b / 255,
		}
	);
	//uncrustify
}