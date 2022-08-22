#include <Editor.h>
#include <n64.h>

static Gfx* Gfx_TexScroll(u32 x, u32 y, s32 w, s32 h) {
	Gfx* displayList = n64_graph_alloc(3 * sizeof(Gfx));
	
	x %= 2048;
	y %= 2048;
	
	gDPTileSync(displayList);
	gDPSetTileSize(displayList + 3, 0, x, y, (x + ((w - 1) << 2)), (y + ((h - 1) << 2)));
	gSPEndDisplayList(displayList + 4);
	
	return displayList;
}

static Gfx* Gfx_TwoTexScroll(s32 tile1, u32 x1, u32 y1, s32 width1, s32 height1, s32 tile2, u32 x2, u32 y2, s32 width2, s32 height2) {
	Gfx* displayList = n64_graph_alloc(5 * sizeof(Gfx));
	
	x1 %= 2048;
	y1 %= 2048;
	x2 %= 2048;
	y2 %= 2048;
	
	gDPTileSync(displayList);
	gDPSetTileSize(displayList + 1, tile1, x1, y1, (x1 + ((width1 - 1) << 2)), (y1 + ((height1 - 1) << 2)));
	gDPTileSync(displayList + 2);
	gDPSetTileSize(displayList + 3, tile2, x2, y2, (x2 + ((width2 - 1) << 2)), (y2 + ((height2 - 1) << 2)));
	gSPEndDisplayList(displayList + 4);
	
	return displayList;
}

static Gfx* Gfx_TwoTexScrollEnvColor(s32 tile1, u32 x1, u32 y1, s32 width1, s32 height1, s32 tile2, u32 x2, u32 y2, s32 width2, s32 height2, s32 r, s32 g, s32 b, s32 a) {
	Gfx* displayList = n64_graph_alloc(6 * sizeof(Gfx));
	
	x1 %= 2048;
	y1 %= 2048;
	x2 %= 2048;
	y2 %= 2048;
	
	gDPTileSync(displayList);
	gDPSetTileSize(displayList + 1, tile1, x1, y1, (x1 + ((width1 - 1) << 2)), (y1 + ((height1 - 1) << 2)));
	gDPTileSync(displayList + 2);
	gDPSetTileSize(displayList + 3, tile2, x2, y2, (x2 + ((width2 - 1) << 2)), (y2 + ((height2 - 1) << 2)));
	gDPSetEnvColor(displayList + 4, r, g, b, a);
	gSPEndDisplayList(displayList + 5);
	
	return displayList;
}

static Gfx* Gfx_TwoTexScrollPrimColor(s32 tile1, u32 x1, u32 y1, s32 width1, s32 height1, s32 tile2, u32 x2, u32 y2, s32 width2, s32 height2, s32 r, s32 g, s32 b, s32 a) {
	Gfx* displayList = n64_graph_alloc(6 * sizeof(Gfx));
	
	x1 %= 2048;
	y1 %= 2048;
	x2 %= 2048;
	y2 %= 2048;
	
	gDPTileSync(displayList);
	gDPSetTileSize(displayList + 1, tile1, x1, y1, (x1 + ((width1 - 1) << 2)), (y1 + ((height1 - 1) << 2)));
	gDPTileSync(displayList + 2);
	gDPSetTileSize(displayList + 3, tile2, x2, y2, (x2 + ((width2 - 1) << 2)), (y2 + ((height2 - 1) << 2)));
	gDPSetPrimColor(displayList + 4, 0, 0, r, g, b, a);
	gSPEndDisplayList(displayList + 5);
	
	return displayList;
}

static void32 D_8012A2F8[] = {
	0x0200BA18,
	0x0200CA18,
};

static void Scene_DrawConfigYdan(AnimOoT* this) {
	gSPSegment(
		POLY_XLU_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			127 - (this->frame % 128),
			(this->frame * 1) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32
		)
	);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
	
	gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(D_8012A2F8[this->nightFlag]));
}

static void Scene_DrawConfigYdanBoss(AnimOoT* this) {
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		Gfx_TwoTexScroll(
			0,
			(this->frame * 2) % 256,
			0,
			64,
			32,
			1,
			0,
			(this->frame * 2) % 128,
			64,
			32
		)
	);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void32 gDCEntranceTextures[] = {
	0x02012378,
	0x02013378,
};
static void32 sDCLavaFloorTextures[] = {
	0x02011F78, 0x02014778, 0x02014378, 0x02013F78,
	0x02014B78, 0x02013B78, 0x02012F78, 0x02012B78,
};

static void Scene_DrawConfigDdan(AnimOoT* this) {
	Gfx* displayListHead = n64_graph_alloc(2 * sizeof(Gfx[3]));
	
	gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gDCEntranceTextures[this->nightFlag]));
	gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(sDCLavaFloorTextures[(s32)(this->frame & 14) >> 1]));
	gSPSegment(
		POLY_XLU_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			(this->frame * 1) % 256,
			0,
			64,
			32,
			1,
			0,
			(this->frame * 1) % 128,
			64,
			32
		)
	);
	gSPSegment(
		POLY_OPA_DISP++,
		0x0A,
		Gfx_TwoTexScroll(
			0,
			0,
			(this->frame * 1) % 128,
			32,
			32,
			1,
			0,
			(this->frame * 2) % 128,
			32,
			32
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
	
	gSPSegment(POLY_OPA_DISP++, 0x0B, displayListHead);
	gDPPipeSync(displayListHead++);
	gDPSetEnvColor(displayListHead++, 255, 255, 255, 0xFF); // Dodongo Eye
	gSPEndDisplayList(displayListHead++);
	
	gSPSegment(POLY_OPA_DISP++, 0x0C, displayListHead);
	gDPPipeSync(displayListHead++);
	gDPSetEnvColor(displayListHead++, 255, 255, 255, 0xFF); // Dodongo Eye
	gSPEndDisplayList(displayListHead);
}

static void Scene_DrawConfigTokinoma(AnimOoT* this) {
	f32 temp;
	Gfx* displayListHead = n64_graph_alloc(18 * sizeof(Gfx));
	
	temp = 1.0f; // ?
	
	gSPSegment(POLY_XLU_DISP++, 0x08, displayListHead);
	gSPSegment(POLY_OPA_DISP++, 0x08, displayListHead);
	gDPSetPrimColor(
		displayListHead++,
		0,
		0,
		255 - (u8)(185.0f * temp),
		255 - (u8)(145.0f * temp),
		255 - (u8)(105.0f * temp),
		255
	);
	gSPEndDisplayList(displayListHead++);
	
	gSPSegment(POLY_XLU_DISP++, 0x09, displayListHead);
	gSPSegment(POLY_OPA_DISP++, 0x09, displayListHead);
	gDPSetPrimColor(
		displayListHead++,
		0,
		0,
		76 + (u8)(6.0f * temp),
		76 + (u8)(34.0f * temp),
		76 + (u8)(74.0f * temp),
		255
	);
	gSPEndDisplayList(displayListHead++);
	
	gSPSegment(POLY_OPA_DISP++, 0x0A, displayListHead);
	gSPSegment(POLY_XLU_DISP++, 0x0A, displayListHead);
	gDPPipeSync(displayListHead++);
	gDPSetEnvColor(displayListHead++, 0, 0, 0, temp * 255);
	gSPEndDisplayList(displayListHead++);
	
	gSPSegment(POLY_OPA_DISP++, 0x0B, displayListHead);
	gSPSegment(POLY_XLU_DISP++, 0x0B, displayListHead);
	gDPSetPrimColor(
		displayListHead++,
		0,
		0,
		89 + (u8)(166.0f * temp),
		89 + (u8)(166.0f * temp),
		89 + (u8)(166.0f * temp),
		255
	);
	gDPPipeSync(displayListHead++);
	gDPSetEnvColor(displayListHead++, 0, 0, 0, temp * 255);
	gSPEndDisplayList(displayListHead++);
	
	gSPSegment(POLY_OPA_DISP++, 0x0C, displayListHead);
	gSPSegment(POLY_XLU_DISP++, 0x0C, displayListHead);
	gDPSetPrimColor(
		displayListHead++,
		0,
		0,
		255 + (u8)(179.0f * temp),
		255 + (u8)(179.0f * temp),
		255 + (u8)(179.0f * temp),
		255
	);
	gDPPipeSync(displayListHead++);
	gDPSetEnvColor(displayListHead++, 0, 0, 0, temp * 255);
	gSPEndDisplayList(displayListHead++);
	
	gSPSegment(POLY_OPA_DISP++, 0x0D, displayListHead);
	gSPSegment(POLY_XLU_DISP++, 0x0D, displayListHead);
	gDPPipeSync(displayListHead++);
	gDPSetEnvColor(displayListHead++, 0, 0, 0, temp * 255);
	gSPEndDisplayList(displayListHead);
}

static void Scene_DrawConfigKakusiana(AnimOoT* this) {
	gSPSegment(POLY_XLU_DISP++, 0x08, Gfx_TexScroll( 0, (this->frame * 1) % 64, 256, 16));
	gSPSegment(
		POLY_XLU_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			127 - (this->frame % 128),
			(this->frame * 1) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32
		)
	);
	gSPSegment(
		POLY_OPA_DISP++,
		0x0A,
		Gfx_TwoTexScroll( 0, 0, 0, 32, 32, 1, 0, 127 - (this->frame * 1) % 128, 32, 32)
	);
	gSPSegment(POLY_OPA_DISP++, 0x0B, Gfx_TexScroll( 0, (this->frame * 1) % 128, 32, 32));
	gSPSegment(
		POLY_XLU_DISP++,
		0x0C,
		Gfx_TwoTexScroll(
			0,
			0,
			(this->frame * 50) % 2048,
			8,
			512,
			1,
			0,
			(this->frame * 60) % 2048,
			8,
			512
		)
	);
	gSPSegment(
		POLY_OPA_DISP++,
		0x0D,
		Gfx_TwoTexScroll( 0, 0, 0, 32, 64, 1, 0, (this->frame * 1) % 128, 32, 32)
	);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigKenjyanoma(AnimOoT* this) {
	gSPSegment(POLY_XLU_DISP++, 0x08, Gfx_TexScroll( 0, (this->frame * 2) % 256, 64, 64));
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gSPSegment(
		POLY_OPA_DISP++,
		0x0A,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32
		)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			127 - (this->frame * 1) % 128,
			(this->frame * 1) % 256,
			32,
			64,
			1,
			0,
			0,
			32,
			128
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigGreatFairyFountain(AnimOoT* this) {
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 3) % 256,
			32,
			64,
			1,
			this->frame % 128,
			(this->frame * 3) % 256,
			32,
			64
		)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 3) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 3) % 128,
			32,
			32
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigGraveExitLightShining(AnimOoT* this) {
	gSPSegment(POLY_XLU_DISP++, 0x08, Gfx_TexScroll( 0, this->frame % 64, 256, 16));
	gSPSegment(POLY_OPA_DISP++, 0x08, Gfx_TexScroll( 0, this->frame % 64, 256, 16));
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigFairyFountain(AnimOoT* this) {
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 3) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 3) % 128,
			32,
			32
		)
	);
	gSPSegment(POLY_XLU_DISP++, 0x09, Gfx_TexScroll( 0, this->frame % 64, 256, 16));
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigHakadan(AnimOoT* this) {
	gSPSegment(
		POLY_OPA_DISP++,
		0x08,
		Gfx_TwoTexScroll(
			0,
			(this->frame * 2) % 128,
			0,
			32,
			32,
			1,
			(this->frame * 2) % 128,
			0,
			32,
			32
		)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		Gfx_TwoTexScroll(
			0,
			(this->frame * 2) % 128,
			0,
			32,
			32,
			1,
			(this->frame * 2) % 128,
			0,
			32,
			32
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void32 sThievesHideoutEntranceTextures[] = {
	0x0200BD20,
	0x0200B920,
};

static void Scene_DrawConfigGerudoway(AnimOoT* this) {
	gSPSegment(POLY_OPA_DISP++, 0x09, Gfx_TexScroll( 0, (this->frame * 3) % 128, 32, 32));
	
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		SEGMENTED_TO_VIRTUAL(sThievesHideoutEntranceTextures[this->nightFlag])
	);
}

static void32 D_8012A330[] = {
	0x02014C30,
	0x02015830,
};

static void Scene_DrawConfigMizusin(AnimOoT* this) {
	gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(D_8012A330[this->nightFlag]));
	
	gSPSegment(
		POLY_OPA_DISP++,
		0x08,
		Gfx_TwoTexScrollEnvColor(0, this->frame * 1, 0, 32, 32, 1, 0, 0, 32, 32, 0, 0, 0, 127)
	);
	
	gSPSegment(
		POLY_OPA_DISP++,
		0x09,
		Gfx_TwoTexScrollEnvColor(0, this->frame * 1, 0, 32, 32, 1, 0, 0, 32, 32, 0, 0, 0, 160)
	);
	
	gSPSegment(
		POLY_OPA_DISP++,
		0x0A,
		Gfx_TwoTexScrollEnvColor(0, (this->frame * 1) % 128, 0, 32, 32, 1, 0, 0, 32, 32, 0, 0, 0, 160)
	);
	gSPSegment(
		POLY_OPA_DISP++,
		0x0B,
		Gfx_TwoTexScrollEnvColor(0, this->frame * 3, 0, 32, 32, 1, 0, 0, 32, 32, 0, 0, 0, 185)
	);
	
	gSPSegment(
		POLY_XLU_DISP++,
		0x0C,
		Gfx_TwoTexScrollEnvColor(0, this->frame * 1, this->frame * 1, 32, 32, 1, 0, 127 - (this->frame * 1), 32, 32, 0, 0, 0, 128)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x0D,
		Gfx_TwoTexScrollEnvColor(0, this->frame * 4, 0, 32, 32, 1, this->frame * 4, 0, 32, 32, 0, 0, 0, 128)
	);
}

static void Scene_DrawConfigMizusinBs(AnimOoT* this) {
	gSPSegment(
		POLY_OPA_DISP++,
		0x08,
		Gfx_TwoTexScroll( 0, this->frame * 1, 0, 32, 32, 1, 0, 0, 32, 32)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 0xFF);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 145);
}

static void Scene_DrawConfigSyatekijyou(AnimOoT* this) {
	gSPSegment(POLY_OPA_DISP++, 0x08, Gfx_TexScroll( 0, this->frame % 64, 4, 16));
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigHairalNiwa(AnimOoT* this) {
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 3) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 3) % 128,
			32,
			32
		)
	);
	
	gSPSegment(POLY_XLU_DISP++, 0x09, Gfx_TexScroll( 0, (this->frame * 10) % 256, 32, 64));
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static s16 sintable[0x400] = {
	0x0000, 0x0032, 0x0064, 0x0096, 0x00C9, 0x00FB, 0x012D,
	0x0160, 0x0192, 0x01C4, 0x01F7, 0x0229, 0x025B, 0x028E,
	0x02C0, 0x02F2, 0x0324, 0x0357, 0x0389, 0x03BB, 0x03EE,
	0x0420, 0x0452, 0x0484, 0x04B7, 0x04E9, 0x051B, 0x054E,
	0x0580, 0x05B2, 0x05E4, 0x0617, 0x0649, 0x067B, 0x06AD,
	0x06E0, 0x0712, 0x0744, 0x0776, 0x07A9, 0x07DB, 0x080D,
	0x083F, 0x0871, 0x08A4, 0x08D6, 0x0908, 0x093A, 0x096C,
	0x099F, 0x09D1, 0x0A03, 0x0A35, 0x0A67, 0x0A99, 0x0ACB,
	0x0AFE, 0x0B30, 0x0B62, 0x0B94, 0x0BC6, 0x0BF8, 0x0C2A,
	0x0C5C, 0x0C8E, 0x0CC0, 0x0CF2, 0x0D25, 0x0D57, 0x0D89,
	0x0DBB, 0x0DED, 0x0E1F, 0x0E51, 0x0E83, 0x0EB5, 0x0EE7,
	0x0F19, 0x0F4B, 0x0F7C, 0x0FAE, 0x0FE0, 0x1012, 0x1044,
	0x1076, 0x10A8, 0x10DA, 0x110C, 0x113E, 0x116F, 0x11A1,
	0x11D3, 0x1205, 0x1237, 0x1269, 0x129A, 0x12CC, 0x12FE,
	0x1330, 0x1361, 0x1393, 0x13C5, 0x13F6, 0x1428, 0x145A,
	0x148C, 0x14BD, 0x14EF, 0x1520, 0x1552, 0x1584, 0x15B5,
	0x15E7, 0x1618, 0x164A, 0x167B, 0x16AD, 0x16DF, 0x1710,
	0x1741, 0x1773, 0x17A4, 0x17D6, 0x1807, 0x1839, 0x186A,
	0x189B, 0x18CD, 0x18FE, 0x1930, 0x1961, 0x1992, 0x19C3,
	0x19F5, 0x1A26, 0x1A57, 0x1A88, 0x1ABA, 0x1AEB, 0x1B1C,
	0x1B4D, 0x1B7E, 0x1BAF, 0x1BE1, 0x1C12, 0x1C43, 0x1C74,
	0x1CA5, 0x1CD6, 0x1D07, 0x1D38, 0x1D69, 0x1D9A, 0x1DCB,
	0x1DFC, 0x1E2D, 0x1E5D, 0x1E8E, 0x1EBF, 0x1EF0, 0x1F21,
	0x1F52, 0x1F82, 0x1FB3, 0x1FE4, 0x2015, 0x2045, 0x2076,
	0x20A7, 0x20D7, 0x2108, 0x2139, 0x2169, 0x219A, 0x21CA,
	0x21FB, 0x222B, 0x225C, 0x228C, 0x22BD, 0x22ED, 0x231D,
	0x234E, 0x237E, 0x23AE, 0x23DF, 0x240F, 0x243F, 0x2470,
	0x24A0, 0x24D0, 0x2500, 0x2530, 0x2560, 0x2591, 0x25C1,
	0x25F1, 0x2621, 0x2651, 0x2681, 0x26B1, 0x26E1, 0x2711,
	0x2740, 0x2770, 0x27A0, 0x27D0, 0x2800, 0x2830, 0x285F,
	0x288F, 0x28BF, 0x28EE, 0x291E, 0x294E, 0x297D, 0x29AD,
	0x29DD, 0x2A0C, 0x2A3C, 0x2A6B, 0x2A9B, 0x2ACA, 0x2AF9,
	0x2B29, 0x2B58, 0x2B87, 0x2BB7, 0x2BE6, 0x2C15, 0x2C44,
	0x2C74, 0x2CA3, 0x2CD2, 0x2D01, 0x2D30, 0x2D5F, 0x2D8E,
	0x2DBD, 0x2DEC, 0x2E1B, 0x2E4A, 0x2E79, 0x2EA8, 0x2ED7,
	0x2F06, 0x2F34, 0x2F63, 0x2F92, 0x2FC0, 0x2FEF, 0x301E,
	0x304C, 0x307B, 0x30A9, 0x30D8, 0x3107, 0x3135, 0x3163,
	0x3192, 0x31C0, 0x31EF, 0x321D, 0x324B, 0x3279, 0x32A8,
	0x32D6, 0x3304, 0x3332, 0x3360, 0x338E, 0x33BC, 0x33EA,
	0x3418, 0x3446, 0x3474, 0x34A2, 0x34D0, 0x34FE, 0x352B,
	0x3559, 0x3587, 0x35B5, 0x35E2, 0x3610, 0x363D, 0x366B,
	0x3698, 0x36C6, 0x36F3, 0x3721, 0x374E, 0x377C, 0x37A9,
	0x37D6, 0x3803, 0x3831, 0x385E, 0x388B, 0x38B8, 0x38E5,
	0x3912, 0x393F, 0x396C, 0x3999, 0x39C6, 0x39F3, 0x3A20,
	0x3A4D, 0x3A79, 0x3AA6, 0x3AD3, 0x3B00, 0x3B2C, 0x3B59,
	0x3B85, 0x3BB2, 0x3BDE, 0x3C0B, 0x3C37, 0x3C64, 0x3C90,
	0x3CBC, 0x3CE9, 0x3D15, 0x3D41, 0x3D6D, 0x3D99, 0x3DC5,
	0x3DF1, 0x3E1D, 0x3E49, 0x3E75, 0x3EA1, 0x3ECD, 0x3EF9,
	0x3F25, 0x3F50, 0x3F7C, 0x3FA8, 0x3FD3, 0x3FFF, 0x402B,
	0x4056, 0x4082, 0x40AD, 0x40D8, 0x4104, 0x412F, 0x415A,
	0x4186, 0x41B1, 0x41DC, 0x4207, 0x4232, 0x425D, 0x4288,
	0x42B3, 0x42DE, 0x4309, 0x4334, 0x435F, 0x4389, 0x43B4,
	0x43DF, 0x4409, 0x4434, 0x445F, 0x4489, 0x44B4, 0x44DE,
	0x4508, 0x4533, 0x455D, 0x4587, 0x45B1, 0x45DC, 0x4606,
	0x4630, 0x465A, 0x4684, 0x46AE, 0x46D8, 0x4702, 0x472C,
	0x4755, 0x477F, 0x47A9, 0x47D2, 0x47FC, 0x4826, 0x484F,
	0x4879, 0x48A2, 0x48CC, 0x48F5, 0x491E, 0x4948, 0x4971,
	0x499A, 0x49C3, 0x49EC, 0x4A15, 0x4A3E, 0x4A67, 0x4A90,
	0x4AB9, 0x4AE2, 0x4B0B, 0x4B33, 0x4B5C, 0x4B85, 0x4BAD,
	0x4BD6, 0x4BFE, 0x4C27, 0x4C4F, 0x4C78, 0x4CA0, 0x4CC8,
	0x4CF0, 0x4D19, 0x4D41, 0x4D69, 0x4D91, 0x4DB9, 0x4DE1,
	0x4E09, 0x4E31, 0x4E58, 0x4E80, 0x4EA8, 0x4ED0, 0x4EF7,
	0x4F1F, 0x4F46, 0x4F6E, 0x4F95, 0x4FBD, 0x4FE4, 0x500B,
	0x5032, 0x505A, 0x5081, 0x50A8, 0x50CF, 0x50F6, 0x511D,
	0x5144, 0x516B, 0x5191, 0x51B8, 0x51DF, 0x5205, 0x522C,
	0x5253, 0x5279, 0x52A0, 0x52C6, 0x52EC, 0x5313, 0x5339,
	0x535F, 0x5385, 0x53AB, 0x53D1, 0x53F7, 0x541D, 0x5443,
	0x5469, 0x548F, 0x54B5, 0x54DA, 0x5500, 0x5525, 0x554B,
	0x5571, 0x5596, 0x55BB, 0x55E1, 0x5606, 0x562B, 0x5650,
	0x5675, 0x569B, 0x56C0, 0x56E5, 0x5709, 0x572E, 0x5753,
	0x5778, 0x579D, 0x57C1, 0x57E6, 0x580A, 0x582F, 0x5853,
	0x5878, 0x589C, 0x58C0, 0x58E5, 0x5909, 0x592D, 0x5951,
	0x5975, 0x5999, 0x59BD, 0x59E1, 0x5A04, 0x5A28, 0x5A4C,
	0x5A6F, 0x5A93, 0x5AB7, 0x5ADA, 0x5AFD, 0x5B21, 0x5B44,
	0x5B67, 0x5B8B, 0x5BAE, 0x5BD1, 0x5BF4, 0x5C17, 0x5C3A,
	0x5C5D, 0x5C7F, 0x5CA2, 0x5CC5, 0x5CE7, 0x5D0A, 0x5D2D,
	0x5D4F, 0x5D71, 0x5D94, 0x5DB6, 0x5DD8, 0x5DFA, 0x5E1D,
	0x5E3F, 0x5E61, 0x5E83, 0x5EA5, 0x5EC6, 0x5EE8, 0x5F0A,
	0x5F2C, 0x5F4D, 0x5F6F, 0x5F90, 0x5FB2, 0x5FD3, 0x5FF4,
	0x6016, 0x6037, 0x6058, 0x6079, 0x609A, 0x60BB, 0x60DC,
	0x60FD, 0x611E, 0x613E, 0x615F, 0x6180, 0x61A0, 0x61C1,
	0x61E1, 0x6202, 0x6222, 0x6242, 0x6263, 0x6283, 0x62A3,
	0x62C3, 0x62E3, 0x6303, 0x6323, 0x6342, 0x6362, 0x6382,
	0x63A1, 0x63C1, 0x63E0, 0x6400, 0x641F, 0x643F, 0x645E,
	0x647D, 0x649C, 0x64BB, 0x64DA, 0x64F9, 0x6518, 0x6537,
	0x6556, 0x6574, 0x6593, 0x65B2, 0x65D0, 0x65EF, 0x660D,
	0x662B, 0x664A, 0x6668, 0x6686, 0x66A4, 0x66C2, 0x66E0,
	0x66FE, 0x671C, 0x673A, 0x6757, 0x6775, 0x6792, 0x67B0,
	0x67CD, 0x67EB, 0x6808, 0x6825, 0x6843, 0x6860, 0x687D,
	0x689A, 0x68B7, 0x68D4, 0x68F1, 0x690D, 0x692A, 0x6947,
	0x6963, 0x6980, 0x699C, 0x69B9, 0x69D5, 0x69F1, 0x6A0E,
	0x6A2A, 0x6A46, 0x6A62, 0x6A7E, 0x6A9A, 0x6AB5, 0x6AD1,
	0x6AED, 0x6B08, 0x6B24, 0x6B40, 0x6B5B, 0x6B76, 0x6B92,
	0x6BAD, 0x6BC8, 0x6BE3, 0x6BFE, 0x6C19, 0x6C34, 0x6C4F,
	0x6C6A, 0x6C84, 0x6C9F, 0x6CBA, 0x6CD4, 0x6CEF, 0x6D09,
	0x6D23, 0x6D3E, 0x6D58, 0x6D72, 0x6D8C, 0x6DA6, 0x6DC0,
	0x6DDA, 0x6DF3, 0x6E0D, 0x6E27, 0x6E40, 0x6E5A, 0x6E73,
	0x6E8D, 0x6EA6, 0x6EBF, 0x6ED9, 0x6EF2, 0x6F0B, 0x6F24,
	0x6F3D, 0x6F55, 0x6F6E, 0x6F87, 0x6FA0, 0x6FB8, 0x6FD1,
	0x6FE9, 0x7002, 0x701A, 0x7032, 0x704A, 0x7062, 0x707A,
	0x7092, 0x70AA, 0x70C2, 0x70DA, 0x70F2, 0x7109, 0x7121,
	0x7138, 0x7150, 0x7167, 0x717E, 0x7196, 0x71AD, 0x71C4,
	0x71DB, 0x71F2, 0x7209, 0x7220, 0x7236, 0x724D, 0x7264,
	0x727A, 0x7291, 0x72A7, 0x72BD, 0x72D4, 0x72EA, 0x7300,
	0x7316, 0x732C, 0x7342, 0x7358, 0x736E, 0x7383, 0x7399,
	0x73AE, 0x73C4, 0x73D9, 0x73EF, 0x7404, 0x7419, 0x742E,
	0x7443, 0x7458, 0x746D, 0x7482, 0x7497, 0x74AC, 0x74C0,
	0x74D5, 0x74EA, 0x74FE, 0x7512, 0x7527, 0x753B, 0x754F,
	0x7563, 0x7577, 0x758B, 0x759F, 0x75B3, 0x75C7, 0x75DA,
	0x75EE, 0x7601, 0x7615, 0x7628, 0x763B, 0x764F, 0x7662,
	0x7675, 0x7688, 0x769B, 0x76AE, 0x76C1, 0x76D3, 0x76E6,
	0x76F9, 0x770B, 0x771E, 0x7730, 0x7742, 0x7754, 0x7767,
	0x7779, 0x778B, 0x779D, 0x77AF, 0x77C0, 0x77D2, 0x77E4,
	0x77F5, 0x7807, 0x7818, 0x782A, 0x783B, 0x784C, 0x785D,
	0x786E, 0x787F, 0x7890, 0x78A1, 0x78B2, 0x78C3, 0x78D3,
	0x78E4, 0x78F4, 0x7905, 0x7915, 0x7925, 0x7936, 0x7946,
	0x7956, 0x7966, 0x7976, 0x7985, 0x7995, 0x79A5, 0x79B5,
	0x79C4, 0x79D4, 0x79E3, 0x79F2, 0x7A02, 0x7A11, 0x7A20,
	0x7A2F, 0x7A3E, 0x7A4D, 0x7A5B, 0x7A6A, 0x7A79, 0x7A87,
	0x7A96, 0x7AA4, 0x7AB3, 0x7AC1, 0x7ACF, 0x7ADD, 0x7AEB,
	0x7AF9, 0x7B07, 0x7B15, 0x7B23, 0x7B31, 0x7B3E, 0x7B4C,
	0x7B59, 0x7B67, 0x7B74, 0x7B81, 0x7B8E, 0x7B9B, 0x7BA8,
	0x7BB5, 0x7BC2, 0x7BCF, 0x7BDC, 0x7BE8, 0x7BF5, 0x7C02,
	0x7C0E, 0x7C1A, 0x7C27, 0x7C33, 0x7C3F, 0x7C4B, 0x7C57,
	0x7C63, 0x7C6F, 0x7C7A, 0x7C86, 0x7C92, 0x7C9D, 0x7CA9,
	0x7CB4, 0x7CBF, 0x7CCB, 0x7CD6, 0x7CE1, 0x7CEC, 0x7CF7,
	0x7D02, 0x7D0C, 0x7D17, 0x7D22, 0x7D2C, 0x7D37, 0x7D41,
	0x7D4B, 0x7D56, 0x7D60, 0x7D6A, 0x7D74, 0x7D7E, 0x7D88,
	0x7D91, 0x7D9B, 0x7DA5, 0x7DAE, 0x7DB8, 0x7DC1, 0x7DCB,
	0x7DD4, 0x7DDD, 0x7DE6, 0x7DEF, 0x7DF8, 0x7E01, 0x7E0A,
	0x7E13, 0x7E1B, 0x7E24, 0x7E2C, 0x7E35, 0x7E3D, 0x7E45,
	0x7E4D, 0x7E56, 0x7E5E, 0x7E66, 0x7E6D, 0x7E75, 0x7E7D,
	0x7E85, 0x7E8C, 0x7E94, 0x7E9B, 0x7EA3, 0x7EAA, 0x7EB1,
	0x7EB8, 0x7EBF, 0x7EC6, 0x7ECD, 0x7ED4, 0x7EDB, 0x7EE1,
	0x7EE8, 0x7EEE, 0x7EF5, 0x7EFB, 0x7F01, 0x7F08, 0x7F0E,
	0x7F14, 0x7F1A, 0x7F20, 0x7F25, 0x7F2B, 0x7F31, 0x7F36,
	0x7F3C, 0x7F41, 0x7F47, 0x7F4C, 0x7F51, 0x7F56, 0x7F5B,
	0x7F60, 0x7F65, 0x7F6A, 0x7F6F, 0x7F74, 0x7F78, 0x7F7D,
	0x7F81, 0x7F85, 0x7F8A, 0x7F8E, 0x7F92, 0x7F96, 0x7F9A,
	0x7F9E, 0x7FA2, 0x7FA6, 0x7FA9, 0x7FAD, 0x7FB0, 0x7FB4,
	0x7FB7, 0x7FBA, 0x7FBE, 0x7FC1, 0x7FC4, 0x7FC7, 0x7FCA,
	0x7FCC, 0x7FCF, 0x7FD2, 0x7FD4, 0x7FD7, 0x7FD9, 0x7FDC,
	0x7FDE, 0x7FE0, 0x7FE2, 0x7FE4, 0x7FE6, 0x7FE8, 0x7FEA,
	0x7FEC, 0x7FED, 0x7FEF, 0x7FF1, 0x7FF2, 0x7FF3, 0x7FF5,
	0x7FF6, 0x7FF7, 0x7FF8, 0x7FF9, 0x7FFA, 0x7FFB, 0x7FFB,
	0x7FFC, 0x7FFD, 0x7FFD, 0x7FFE, 0x7FFE, 0x7FFE, 0x7FFE,
	0x7FFE, 0x7FFF,
};

static s16 sins(u16 x) {
	s16 value;
	
	x >>= 4;
	
	if (x & 0x400) {
		value = sintable[0x3FF - (x & 0x3FF)];
	} else {
		value = sintable[x & 0x3FF];
	}
	
	if (x & 0x800) {
		return -value;
	} else {
		return value;
	}
}

static s16 coss(u16 angle) {
	return sins(angle + 0x4000);
}

static void Scene_DrawConfigGanonCastleExterior(AnimOoT* this) {
	s8 sp83;
	
	if (1) {
	}                      // Necessary to match
	
	sp83 = coss(this->frame * 1500) >> 8;
	
	gSPSegment(POLY_XLU_DISP++, 0x09, Gfx_TexScroll( 0, (this->frame * 1) % 256, 64, 64));
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		Gfx_TwoTexScroll(
			0,
			0,
			255 - (this->frame * 1) % 256,
			64,
			64,
			1,
			0,
			(this->frame * 1) % 256,
			64,
			64
		)
	);
	
	gSPSegment(
		POLY_OPA_DISP++,
		0x0B,
		Gfx_TwoTexScroll(
			0,
			255 - (this->frame * 1) % 128,
			(this->frame * 1) % 128,
			32,
			32,
			1,
			(this->frame * 1) % 128,
			(this->frame * 1) % 128,
			32,
			32
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
	
	sp83 = (sp83 >> 1) + 192;
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, sp83, sp83, sp83, 128);
}

// Screen Shake for Ganon's Tower Collapse
static void func_8009BEEC(AnimOoT* this) {
}

static void Scene_DrawConfigGanonFinal(AnimOoT* this) {
	s8 sp7B;
	
	sp7B = coss((this->frame * 1500) & 0xFFFF) >> 8;
	
	gSPSegment(
		POLY_OPA_DISP++,
		0x08,
		Gfx_TwoTexScroll(
			0,
			0,
			(this->frame * 1) % 512,
			64,
			128,
			1,
			0,
			511 - (this->frame * 1) % 512,
			64,
			128
		)
	);
	gSPSegment(
		POLY_OPA_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			0,
			(this->frame * 1) % 256,
			32,
			64,
			1,
			0,
			255 - (this->frame * 1) % 256,
			32,
			64
		)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x0A,
		Gfx_TwoTexScroll(
			0,
			0,
			(this->frame * 20) % 2048,
			16,
			512,
			1,
			0,
			(this->frame * 30) % 2048,
			16,
			512
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
	
	sp7B = (sp7B >> 1) + 192;
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, sp7B, sp7B, sp7B, 128);
}

static void32 sIceCavernEntranceTextures[] = {
	0x0200FAC0,
	0x0200F8C0,
};

static void Scene_DrawConfigIceDoukuto(AnimOoT* this) {
	if (0) {
	}                      // Necessary to match
	
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		SEGMENTED_TO_VIRTUAL(sIceCavernEntranceTextures[this->nightFlag])
	);
	gSPSegment(
		POLY_OPA_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32
		)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x0A,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigHakaanaOuke(AnimOoT* this) {
	gSPSegment(POLY_XLU_DISP++, 0x08, Gfx_TexScroll( 0, (this->frame * 1) % 64, 256, 16));
	gSPSegment(
		POLY_XLU_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			0,
			(this->frame * 60) % 2048,
			8,
			512,
			1,
			0,
			(this->frame * 50) % 2048,
			8,
			512
		)
	);
	gSPSegment(
		POLY_OPA_DISP++,
		0x0A,
		Gfx_TwoTexScroll(
			0,
			127 - (this->frame * 1) % 128,
			0,
			32,
			32,
			1,
			(this->frame * 1) % 128,
			0,
			32,
			32
		)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x0B,
		Gfx_TwoTexScroll(
			0,
			0,
			1023 - (this->frame * 6) % 1024,
			16,
			256,
			1,
			0,
			1023 - (this->frame * 3) % 1024,
			16,
			256
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigHyliaLabo(AnimOoT* this) {
	gSPSegment(
		POLY_OPA_DISP++,
		0x08,
		Gfx_TwoTexScroll( 0, 0, 0, 32, 32, 1, 0, (this->frame * 1) % 128, 32, 32)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x0A,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32
		)
	);
	gSPSegment(POLY_XLU_DISP++, 0x09, Gfx_TexScroll( 0, 255 - (this->frame * 10) % 256, 32, 64));
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigCalmWater(AnimOoT* this) {
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void32 sGTGEntranceTextures[] = {
	0x0200F8C0,
	0x020100C0,
};

static void Scene_DrawConfigMen(AnimOoT* this) {
	if (0) {
	}                      // Necessary to match
	
	gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sGTGEntranceTextures[this->nightFlag]));
	gSPSegment(
		POLY_OPA_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32
		)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x0A,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigTuribori(AnimOoT* this) {
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		Gfx_TwoTexScrollPrimColor(
			0,
			127 - this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32,
			255,
			255,
			255,
			127
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigBowling(AnimOoT* this) {
	gSPSegment(POLY_XLU_DISP++, 0x08, Gfx_TexScroll( 127 - (this->frame * 4) % 128, 0, 32, 32));
	gSPSegment(POLY_OPA_DISP++, 0x09, Gfx_TexScroll( 0, (this->frame * 5) % 64, 16, 16));
	gSPSegment(POLY_OPA_DISP++, 0x0A, Gfx_TexScroll( 0, 63 - (this->frame * 2) % 64, 16, 16));
	gSPSegment(
		POLY_XLU_DISP++,
		0x0B,
		Gfx_TwoTexScroll( 0, 0, 127 - (this->frame * 3) % 128, 32, 32, 1, 0, 0, 32, 32)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void32 sLonLonHouseEntranceTextures[] = {
	0x02005210,
	0x02005010,
};

static void Scene_DrawConfigSouko(AnimOoT* this) {
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		SEGMENTED_TO_VIRTUAL(sLonLonHouseEntranceTextures[this->nightFlag])
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void32 sGuardHouseView2Textures[] = {
	0x02006550,
	0x02003550,
};
static void32 sGuardHouseView1Textures[] = {
	0x02002350,
	0x02001350,
};

static void Scene_DrawConfigMiharigoya(AnimOoT* this) {
	gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sGuardHouseView1Textures[this->nightFlag]));
	gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(sGuardHouseView2Textures[this->nightFlag]));
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigMahouya(AnimOoT* this) {
	gSPSegment(POLY_OPA_DISP++, 0x08, Gfx_TexScroll( 0, (this->frame * 3) % 128, 32, 32));
	gSPSegment(
		POLY_XLU_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			0,
			1023 - (this->frame * 3) % 1024,
			16,
			256,
			1,
			0,
			1023 - (this->frame * 6) % 1024,
			16,
			256
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void32 sForestTempleEntranceTextures[] = {
	0x02014D90,
	0x02014590,
};

static void Scene_DrawConfigBmori1(AnimOoT* this) {
	if (0) {
	}                      // Necessary to match
	
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		SEGMENTED_TO_VIRTUAL(sForestTempleEntranceTextures[this->nightFlag])
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32
		)
	);
	gSPSegment(
		POLY_OPA_DISP++,
		0x0A,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void32 sSpiritTempleEntranceTextures[] = {
	0x02018920,
	0x02018020,
};

static void Scene_DrawConfigJyasinzou(AnimOoT* this) {
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		SEGMENTED_TO_VIRTUAL(sSpiritTempleEntranceTextures[this->nightFlag])
	);
}

static void Scene_DrawConfigSpot00(AnimOoT* this) {
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 3) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 3) % 128,
			32,
			32
		)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 10) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 10) % 128,
			32,
			32
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void32 sKakarikoWindowTextures[] = {
	0x02015B50,
	0x02016B50,
};

static void Scene_DrawConfigSpot01(AnimOoT* this) {
	gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sKakarikoWindowTextures[this->nightFlag]));
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigSpot03(AnimOoT* this) {
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 6) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 6) % 128,
			32,
			32
		)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 3) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 3) % 128,
			32,
			32
		)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x0A,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigSpot04(AnimOoT* this) {
	u8 spA3;
	u16 spA0;
	Gfx* displayListHead;
	
	spA3 = 128;
	spA0 = 500;
	displayListHead = n64_graph_alloc(6 * sizeof(Gfx));
	
	if (1) {
	}
	if (1) {
	}
	
	gSPSegment(
		POLY_XLU_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32
		)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 10) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 10) % 128,
			32,
			32
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
	
	gSPSegment(POLY_OPA_DISP++, 0x0A, displayListHead);
	gDPPipeSync(displayListHead++);
	gDPSetEnvColor(displayListHead++, 128, 128, 128, spA3);
	gSPEndDisplayList(displayListHead++);
	
	gSPSegment(POLY_XLU_DISP++, 0x0B, displayListHead);
	gSPSegment(POLY_OPA_DISP++, 0x0B, displayListHead);
	gDPPipeSync(displayListHead++);
	gDPSetEnvColor(displayListHead++, 128, 128, 128, spA0 * 0.1f);
	gSPEndDisplayList(displayListHead);
}

static void Scene_DrawConfigSpot06(AnimOoT* this) {
	gSPSegment(
		POLY_OPA_DISP++,
		0x08,
		Gfx_TwoTexScrollEnvColor(
			0,
			this->frame,
			this->frame,
			32,
			32,
			1,
			0,
			0,
			32,
			32,
			0,
			0,
			0,
			168
		)
	);
	gSPSegment(
		POLY_OPA_DISP++,
		0x09,
		Gfx_TwoTexScrollEnvColor(
			0,
			-this->frame,
			-this->frame,
			32,
			32,
			1,
			0,
			0,
			16,
			64,
			0,
			0,
			0,
			168
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 128);
}

static void32 sZorasDomainEntranceTextures[] = {
	0x02008F98,
	0x02008FD8,
};

static void Scene_DrawConfigSpot07(AnimOoT* this) {
	u32 x;
	
	x = 127 - (this->frame * 1) % 128;
	
	gSPSegment(POLY_OPA_DISP++, 0x0C, Gfx_TwoTexScroll( 0, 0, 0, 64, 32, 1, 0, x, 64, 32));
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		SEGMENTED_TO_VIRTUAL(sZorasDomainEntranceTextures[this->nightFlag])
	);
}

static void Scene_DrawConfigSpot08(AnimOoT* this) {
	gSPSegment(
		POLY_OPA_DISP++,
		0x08,
		Gfx_TwoTexScroll( 0, (this->frame * 1) % 128, 0, 32, 32, 1, 0, 0, 32, 32)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			0,
			255 - (this->frame * 2) % 256,
			64,
			64,
			1,
			0,
			255 - (this->frame * 2) % 256,
			64,
			64
		)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x0A,
		Gfx_TwoTexScroll(
			0,
			0,
			(this->frame * 1) % 128,
			32,
			32,
			1,
			0,
			(this->frame * 1) % 128,
			32,
			32
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigSpot09(AnimOoT* this) {
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		Gfx_TwoTexScroll(
			0,
			0,
			(this->frame * 3) % 1024,
			32,
			256,
			1,
			0,
			(this->frame * 3) % 1024,
			32,
			256
		)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			0,
			(this->frame * 1) % 256,
			64,
			64,
			1,
			0,
			(this->frame * 1) % 256,
			64,
			64
		)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x0A,
		Gfx_TwoTexScroll(
			0,
			0,
			(this->frame * 2) % 128,
			32,
			32,
			1,
			0,
			(this->frame * 2) % 128,
			32,
			32
		)
	);
	gSPSegment(
		POLY_OPA_DISP++,
		0x0B,
		Gfx_TwoTexScroll( 0, 0, 0, 32, 32, 1, 0, 127 - (this->frame * 3) % 128, 32, 32)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x0C,
		Gfx_TwoTexScroll(
			0,
			0,
			(this->frame * 1) % 128,
			32,
			32,
			1,
			0,
			(this->frame * 1) % 128,
			32,
			32
		)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x0D,
		Gfx_TwoTexScroll(
			0,
			0,
			(this->frame * 1) % 64,
			16,
			16,
			1,
			0,
			(this->frame * 1) % 64,
			16,
			16
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigSpot10(AnimOoT* this) {
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		Gfx_TwoTexScroll( 0, this->frame % 128, 0, 32, 16, 1, this->frame % 128, 0, 32, 16)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			this->frame % 128,
			32,
			32,
			1,
			this->frame % 128,
			this->frame % 128,
			32,
			32
		)
	);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigSpot11(AnimOoT* this) {
	gSPSegment(
		POLY_OPA_DISP++,
		0x08,
		Gfx_TwoTexScroll( 0, 0, 0, 32, 32, 1, 0, 127 - this->frame % 128, 32, 32)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void32 D_8012A380[] = {
	0x02009678,
	0x0200DE78,
};

static void Scene_DrawConfigSpot12(AnimOoT* this) {
	gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(D_8012A380[this->nightFlag]));
}

static void Scene_DrawConfigSpot13(AnimOoT* this) {
	gSPSegment(
		POLY_OPA_DISP++,
		0x08,
		Gfx_TwoTexScroll( 0, 0, this->frame % 128, 32, 32, 1, 0, this->frame % 128, 32, 32)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x09,
		Gfx_TwoTexScroll( 0, 0, this->frame % 128, 32, 32, 1, 0, this->frame % 128, 32, 32)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigSpot15(AnimOoT* this) {
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 10) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 10) % 128,
			32,
			32
		)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 3) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 3) % 128,
			32,
			32
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigSpot16(AnimOoT* this) {
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigSpot17(AnimOoT* this) {
	s8 sp6F = coss((this->frame * 1500) & 0xFFFF) >> 8;
	s8 sp6E = coss((this->frame * 1500) & 0xFFFF) >> 8;
	
	sp6F = (sp6F >> 1) + 192;
	sp6E = (sp6E >> 1) + 192;
	
	gSPSegment(
		POLY_OPA_DISP++,
		0x08,
		Gfx_TwoTexScroll( 0, 0, this->frame % 128, 32, 32, 1, 0, this->frame % 128, 32, 32)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, sp6F, sp6E, 255, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void32 sGoronCityEntranceTextures[] = {
	0x02009808,
	0x02008FC8,
};

static void Scene_DrawConfigSpot18(AnimOoT* this) {
	gSPSegment(
		POLY_OPA_DISP++,
		0x08,
		Gfx_TwoTexScroll(
			0,
			0,
			127 - this->frame % 128,
			32,
			32,
			1,
			this->frame % 128,
			0,
			32,
			32
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
	
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		SEGMENTED_TO_VIRTUAL(sGoronCityEntranceTextures[this->nightFlag])
	);
}

static void32 sLonLonRanchWindowTextures[] = {
	0x020081E0,
	0x0200FBE0,
};

static void Scene_DrawConfigSpot20(AnimOoT* this) {
	gSPSegment(
		POLY_OPA_DISP++,
		0x08,
		SEGMENTED_TO_VIRTUAL(sLonLonRanchWindowTextures[this->nightFlag])
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigHidan(AnimOoT* this) {
	gSPSegment(
		POLY_OPA_DISP++,
		0x08,
		Gfx_TwoTexScroll(
			0,
			0,
			127 - this->frame % 128,
			32,
			32,
			1,
			127 - this->frame % 128,
			0,
			32,
			32
		)
	);
	gSPSegment(
		POLY_OPA_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			(this->frame * 3) % 128,
			127 - (this->frame * 6) % 128,
			32,
			32,
			1,
			(this->frame * 6) % 128,
			127 - (this->frame * 3) % 128,
			32,
			32
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 64);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 64);
}

static void Scene_DrawConfigBdan(AnimOoT* this) {
#if 0
	static s16 D_8012A39C = 538;
	static s16 D_8012A3A0 = 4272;
	
	f32 temp;
	
	if (play->sceneNum == SCENE_BDAN) {
		gSPSegment(
			POLY_OPA_DISP++,
			0x08,
			Gfx_TwoTexScroll(
				0,
				this->frame % 128,
				(this->frame * 2) % 128,
				32,
				32,
				1,
				127 - this->frame % 128,
				(this->frame * 2) % 128,
				32,
				32
			)
		);
		gSPSegment(
			POLY_OPA_DISP++,
			0x0B,
			Gfx_TwoTexScroll(
				0,
				0,
				255 - (this->frame * 4) % 256,
				32,
				64,
				1,
				0,
				255 - (this->frame * 4) % 256,
				32,
				64
			)
		);
	} else {
		gSPSegment(
			POLY_OPA_DISP++,
			0x08,
			Gfx_TexScroll( (127 - (this->frame * 1)) % 128, (this->frame * 1) % 128, 32, 32)
		);
	}
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
	
	if (FrameAdvance_IsEnabled(play) != true) {
		D_8012A39C += 1820;
		D_8012A3A0 += 1820;
		
		temp = 0.020000001f;
		View_SetDistortionOrientation(
			&play->view,
			((360.00018f / 65535.0f) * (M_PI / 180.0f)) * temp * Math_CosS(D_8012A39C),
			((360.00018f / 65535.0f) * (M_PI / 180.0f)) * temp * Math_SinS(D_8012A39C),
			((360.00018f / 65535.0f) * (M_PI / 180.0f)) * temp * Math_SinS(D_8012A3A0)
		);
		View_SetDistortionScale(
			&play->view,
			1.f + (0.79999995f * temp * Math_SinS(D_8012A3A0)),
			1.f + (0.39999998f * temp * Math_CosS(D_8012A3A0)),
			1.f + (1 * temp * Math_CosS(D_8012A39C))
		);
		View_SetDistortionSpeed(&play->view, 0.95f);
		
		switch (play->roomCtx.unk_74[0]) {
			case 0:
				break;
			case 1:
				if (play->roomCtx.unk_74[1] < 1200) {
					play->roomCtx.unk_74[1] += 200;
				} else {
					play->roomCtx.unk_74[0]++;
				}
				break;
			case 2:
				if (play->roomCtx.unk_74[1] > 0) {
					play->roomCtx.unk_74[1] -= 30;
				} else {
					play->roomCtx.unk_74[1] = 0;
					play->roomCtx.unk_74[0] = 0;
				}
				break;
		}
		
		D_8012A398 += 0.15f + (play->roomCtx.unk_74[1] * 0.001f);
	}
	
	if (play->roomCtx.curRoom.num == 2) {
		Matrix_Scale(1.0f, sinf(D_8012A398) * 0.8f, 1.0f, MTXMODE_NEW);
	} else {
		Matrix_Scale(1.005f, sinf(D_8012A398) * 0.8f, 1.005f, MTXMODE_NEW);
	}
	
	gSPSegment(POLY_OPA_DISP++, 0x0D, Matrix_NewMtx(play->state.gfxCtx, "../z_scene_table.c", 7809));
#endif
}

static void Scene_DrawConfigGanontika(AnimOoT* this) {
	gSPSegment(
		POLY_XLU_DISP++,
		0x08,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 1) % 512,
			32,
			128,
			1,
			this->frame % 128,
			(this->frame * 1) % 512,
			32,
			128
		)
	);
	gSPSegment(
		POLY_XLU_DISP++,
		0x09,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32
		)
	);
	gSPSegment(
		POLY_OPA_DISP++,
		0x0A,
		Gfx_TwoTexScroll(
			0,
			127 - this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32,
			1,
			this->frame % 128,
			(this->frame * 1) % 128,
			32,
			32
		)
	);
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

static void Scene_DrawConfigGanontikaSonogo(AnimOoT* this) {
}

static void Scene_DrawConfigGanonSonogo(AnimOoT* this) {
}

static void Scene_DrawConfigBesitu(AnimOoT* this) {
	gSPSegment(POLY_OPA_DISP++, 0x08, Gfx_TexScroll( 127 - (this->frame * 2) % 128, 0, 32, 64));
	gSPSegment(POLY_OPA_DISP++, 0x09, Gfx_TexScroll( 0, (this->frame * 2) % 512, 128, 128));
	
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, 128);
	
	gDPPipeSync(POLY_XLU_DISP++);
	gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 128);
}

void (*sSceneDrawConfigs[53])(AnimOoT* this) = {
	/* 0  */ NULL, // SDC_DEFAULT
	/* 1  */ Scene_DrawConfigSpot00,
	/* 2  */ Scene_DrawConfigSpot01,
	/* 3  */ Scene_DrawConfigSpot03,
	/* 4  */ Scene_DrawConfigSpot04,
	/* 5  */ Scene_DrawConfigSpot06,
	/* 6  */ Scene_DrawConfigSpot07,
	/* 7  */ Scene_DrawConfigSpot08,
	/* 8  */ Scene_DrawConfigSpot09,
	/* 9  */ Scene_DrawConfigSpot10,
	/* 10 */ Scene_DrawConfigSpot11,
	/* 11 */ Scene_DrawConfigSpot12,
	/* 12 */ Scene_DrawConfigSpot13,
	/* 13 */ Scene_DrawConfigSpot15,
	/* 14 */ Scene_DrawConfigSpot16,
	/* 15 */ Scene_DrawConfigSpot17,
	/* 16 */ Scene_DrawConfigSpot18,
	/* 17 */ Scene_DrawConfigSpot20,
	/* 18 */ Scene_DrawConfigHidan,
	/* 19 */ Scene_DrawConfigYdan,
	/* 20 */ Scene_DrawConfigDdan,
	/* 21 */ Scene_DrawConfigBdan,
	/* 22 */ Scene_DrawConfigBmori1,
	/* 23 */ Scene_DrawConfigMizusin,
	/* 24 */ Scene_DrawConfigHakadan,
	/* 25 */ Scene_DrawConfigJyasinzou,
	/* 26 */ Scene_DrawConfigGanontika,
	/* 27 */ Scene_DrawConfigMen,
	/* 28 */ Scene_DrawConfigYdanBoss,
	/* 29 */ Scene_DrawConfigMizusinBs,
	/* 30 */ Scene_DrawConfigTokinoma,
	/* 31 */ Scene_DrawConfigKakusiana,
	/* 32 */ Scene_DrawConfigKenjyanoma,
	/* 33 */ Scene_DrawConfigGreatFairyFountain,
	/* 34 */ Scene_DrawConfigSyatekijyou,
	/* 35 */ Scene_DrawConfigHairalNiwa,
	/* 36 */ Scene_DrawConfigGanonCastleExterior,
	/* 37 */ Scene_DrawConfigIceDoukuto,
	/* 38 */ Scene_DrawConfigGanonFinal,
	/* 39 */ Scene_DrawConfigFairyFountain,
	/* 40 */ Scene_DrawConfigGerudoway,
	/* 41 */ Scene_DrawConfigBowling,
	/* 42 */ Scene_DrawConfigHakaanaOuke,
	/* 43 */ Scene_DrawConfigHyliaLabo,
	/* 44 */ Scene_DrawConfigSouko,
	/* 45 */ Scene_DrawConfigMiharigoya,
	/* 46 */ Scene_DrawConfigMahouya,
	/* 47 */ Scene_DrawConfigCalmWater,
	/* 48 */ Scene_DrawConfigGraveExitLightShining,
	/* 49 */ Scene_DrawConfigBesitu,
	/* 50 */ Scene_DrawConfigTuribori,
	/* 51 */ Scene_DrawConfigGanonSonogo,
	/* 52 */ Scene_DrawConfigGanontikaSonogo,
};