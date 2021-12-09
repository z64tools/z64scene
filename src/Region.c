#include "Editor.h"

/* / / / / / / / / / / / / / / / / / / / / / / / / / / */

static void Region_UpdateWorkRegion(EditorContext* editorCtx) {
	RegionContext* regCtx = &editorCtx->regionCtx;
	Vec2i* winDim = &editorCtx->appInfo.winDim;
	
	regCtx->workRegion = (Recti) {
		0,
		0 + regCtx->bar[REG_BAR_TOP].rect.h,
		winDim->x,
		winDim->y - regCtx->bar[REG_BAR_BOT].rect.h - regCtx->bar[REG_BAR_TOP].rect.h
	};
}

static void Region_SetTopBarHeight(EditorContext* editorCtx, s32 h) {
	RegionContext* regCtx = &editorCtx->regionCtx;
	Vec2i* winDim = &editorCtx->appInfo.winDim;
	
	regCtx->bar[REG_BAR_TOP].rect.x = 0;
	regCtx->bar[REG_BAR_TOP].rect.y = 0;
	regCtx->bar[REG_BAR_TOP].rect.w = winDim->x;
	regCtx->bar[REG_BAR_TOP].rect.h = h;
	Region_UpdateWorkRegion(editorCtx);
}

static void Region_SetBotBarHeight(EditorContext* editorCtx, s32 h) {
	RegionContext* regCtx = &editorCtx->regionCtx;
	Vec2i* winDim = &editorCtx->appInfo.winDim;
	
	regCtx->bar[REG_BAR_BOT].rect.x = 0;
	regCtx->bar[REG_BAR_BOT].rect.y = winDim->y - h;
	regCtx->bar[REG_BAR_BOT].rect.w = winDim->x;
	regCtx->bar[REG_BAR_BOT].rect.h = h;
	Region_UpdateWorkRegion(editorCtx);
}

/* / / / / / / / / / / / / / / / / / / / / / / / / / / */

static void Region_Draw_RegionBorders(NVGcontext* vg, Recti* rect) {
	nvgBeginPath(vg);
	nvgRect(
		vg,
		0,
		0,
		rect->w,
		rect->h
	);
	nvgRoundedRect(
		vg,
		0 + REG_SPLIT_W,
		0 + REG_SPLIT_W,
		rect->w - REG_SPLIT_W * 2,
		rect->h - REG_SPLIT_W * 2,
		REG_ROUND_R
	);
	nvgPathWinding(vg, NVG_HOLE);
	nvgFillColor(vg, Theme_GetColor(THEME_SPLITTER));
	nvgFill(vg);
}

static s32 Region_Cursor_DistTo(RegionPointFlag flag, Region* reg) {
	Vec2i mouse[] = {
		{ reg->mousePos.x, reg->mousePos.y }, /* REG_POINT_TL */
		{ reg->mousePos.x, reg->mousePos.y }, /* REG_POINT_TR */
		{ reg->mousePos.x, reg->mousePos.y }, /* REG_POINT_BL */
		{ reg->mousePos.x, reg->mousePos.y }, /* REG_POINT_BR */
		{ reg->mousePos.x, 0               }, /* REG_SIDE_L */
		{ reg->mousePos.x, 0               }, /* REG_SIDE_R */
		{ 0,               reg->mousePos.y }, /* REG_SIDE_T */
		{ 0,               reg->mousePos.y }, /* REG_SIDE_B */
	};
	Vec2i pos[] = {
		{ 0,           0,           }, /* REG_POINT_TL */
		{ reg->rect.w, 0,           }, /* REG_POINT_TR */
		{ 0,           reg->rect.h, }, /* REG_POINT_BL */
		{ reg->rect.w, reg->rect.h, }, /* REG_POINT_BR */
		{ 0,           0,           }, /* REG_SIDE_L */
		{ reg->rect.w, 0,           }, /* REG_SIDE_R */
		{ 0,           0,           }, /* REG_SIDE_T */
		{ 0,           reg->rect.h, }, /* REG_SIDE_B */
	};
	s32 i, f;
	
	for (i = 0, f = REG_POINT_TL; f <= REG_SIDE_B; i++, f = f << 1) {
		if (flag & f) {
			break;
		}
	}
	
	return Vec_Vec2i_DistXZ(&mouse[i], &pos[i]);
}

static s32 Region_Cursor_InRegion(Region* region) {
	s32 resX = (region->mousePos.x < region->rect.w && region->mousePos.x >= 0);
	s32 resY = (region->mousePos.y < region->rect.h && region->mousePos.y >= 0);
	
	return (resX && resY);
}

static u32 Region_Cursor_GetStateInRange(Region* reg, s32 range) {
	for (s32 i = REG_POINT_TL; i <= REG_POINT_BR; i = i << 1) {
		if (Region_Cursor_DistTo(i, reg) <= range) {
			return i;
		}
	}
	
	return false;
}

static char* Region_Debug_GetStates(Region* reg) {
	static char buffer[1024];
	char* states[] = {
		"NONE",
		"SPLIT_TL",
		"SPLIT_TR",
		"SPLIT_BL",
		"SPLIT_BR",
		"RESIZE_L",
		"RESIZE_R",
		"RESIZE_T",
		"RESIZE_B",
		"BLOCK_L",
		"BLOCK_R",
		"BLOCK_T",
		"BLOCK_B",
	};
	
	String_Copy(buffer, states[0]);
	
	for (s32 s = 1, t = 0; (1 << s) <= REG_STATE_BLOCK_B; s++) {
		if (reg->stateFlag & (1 << s)) {
			if (t == 0) {
				String_Copy(buffer, states[s + 1]);
				t++;
			} else {
				String_Merge(buffer, "|");
				String_Merge(buffer, states[s + 1]);
			}
		}
	}
	
	return buffer;
}

static void Region_ActionReg_Reset(RegionContext* regCtx) {
	if (regCtx->actionRegion == NULL)
		return;
	regCtx->actionRegion->stateFlag &= ~(
		REG_STATE_SPLIT_TL |
		REG_STATE_SPLIT_TR |
		REG_STATE_SPLIT_BL |
		REG_STATE_SPLIT_BR
	);
	
	regCtx->actionRegion = NULL;
}

static void Region_ActionReg_Update(RegionContext* regCtx) {
	Region* reg = regCtx->actionRegion;
	RegStateFlag splitState = (
		REG_STATE_SPLIT_TL |
		REG_STATE_SPLIT_TR |
		REG_STATE_SPLIT_BL |
		REG_STATE_SPLIT_BR
	);
	
	if (reg->mousePress) {
		reg->stateFlag |= Region_Cursor_GetStateInRange(reg, 20);
	}
}

/* / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Region_KillRegion(RegionContext* regCtx, Region** regg) {
	Region* reg = *regg;
	
	if (reg->next) {
		reg->next->prev = reg->prev;
	}
	
	if (reg->prev) {
		reg->prev->next = reg->next;
	} else {
		regCtx->nodeHead = reg->next;
	}
	
	free(reg);
	
	regg = NULL;
}

void Region_Init(EditorContext* editorCtx) {
	RegionContext* regCtx = &editorCtx->regionCtx;
	Vec2i* winDim = &editorCtx->appInfo.winDim;
	
	regCtx->nodeHead = Lib_Calloc(0, sizeof(Region));
	Region_SetTopBarHeight(editorCtx, 30);
	Region_SetBotBarHeight(editorCtx, 30);
	
	regCtx->nodeHead->stateFlag = (REG_STATE_BLOCK_ALL);
}

void Region_Update(EditorContext* editorCtx) {
	RegionContext* regCtx = &editorCtx->regionCtx;
	Region* reg = regCtx->nodeHead;
	Region* killReg;
	MouseInput* mouse = &editorCtx->inputCtx.mouse;
	Vec2i* winDim = &editorCtx->appInfo.winDim;
	
	Region_SetTopBarHeight(editorCtx, regCtx->bar[REG_BAR_TOP].rect.h);
	Region_SetBotBarHeight(editorCtx, regCtx->bar[REG_BAR_BOT].rect.h);
	
	regCtx->nodeHead->rect = (Recti) {
		regCtx->workRegion.x,
		regCtx->workRegion.y,
		regCtx->workRegion.w,
		regCtx->workRegion.h
	};
	
	if (regCtx->actionRegion && mouse->cursorAction == false) {
		Region_ActionReg_Reset(regCtx);
	}
	
	while (reg) {
		Vec2i rectPos = {
			reg->rect.x,
			reg->rect.y
		};
		Vec_Vec2i_Substract(
			&reg->mousePos,
			&mouse->pos,
			(Vec2i*)&rectPos
		);
		
		reg->mouseInRegion = Region_Cursor_InRegion(reg);
		reg->center.x = reg->rect.x + reg->rect.w * 0.5f;
		reg->center.y = reg->rect.y + reg->rect.h * 0.5f;
		
		if (regCtx->actionRegion == NULL &&
		    reg->mouseInRegion &&
		    mouse->cursorAction) {
			if (mouse->click.press) {
				reg->mousePressPos = reg->mousePos;
				reg->mousePress = true;
			}
			regCtx->actionRegion = reg;
		}
		
		if (regCtx->actionRegion) {
			Region_ActionReg_Update(regCtx);
		} else {
			if (Region_Cursor_GetStateInRange(reg, 20)) {
				editorCtx->inputCtx.mouse.cursorIcon = CURSOR_CROSSHAIRR;
			}
		}
		
		if (reg->update) {
			if (reg != regCtx->nodeHead &&
			    ((regCtx->actionRegion && regCtx->actionRegion == reg) ||
			    regCtx->actionRegion == NULL)) {
				reg->update(editorCtx, reg);
			}
		}
		
		reg->mousePress = false;
		reg = reg->next;
	}
}

void Region_Draw(EditorContext* editorCtx) {
	RegionContext* regCtx = &editorCtx->regionCtx;
	Region* reg = regCtx->nodeHead;
	Region* killReg;
	MouseInput* mouse = &editorCtx->inputCtx.mouse;
	Vec2i* winDim = &editorCtx->appInfo.winDim;
	
	// Draw All Regions Except NodeHead
	while (reg) {
		glViewport(
			reg->rect.x,
			winDim->y - reg->rect.y - reg->rect.h,
			reg->rect.w,
			reg->rect.h
		);
		nvgBeginFrame(editorCtx->vg, reg->rect.w, reg->rect.h, 1.0f); {
			if (reg->draw)
				reg->draw(editorCtx, reg);
			
			if (reg->mouseInRegion) {
				nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_BASE_WHITE));
				nvgFontSize(editorCtx->vg, 30 - 16);
				nvgFontFace(editorCtx->vg, "sans");
				nvgTextAlign(editorCtx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
				nvgText(
					editorCtx->vg,
					8,
					8,
					":hello mouse:",
					NULL
				);
			}
			
			Region_Draw_RegionBorders(editorCtx->vg, &reg->rect);
		} nvgEndFrame(editorCtx->vg);
		
		reg = reg->next;
	}
	// Draw Bars
	for (s32 i = 0; i < 2; i++) {
		glViewport(
			regCtx->bar[i].rect.x,
			winDim->y - regCtx->bar[i].rect.y - regCtx->bar[i].rect.h,
			regCtx->bar[i].rect.w,
			regCtx->bar[i].rect.h
		);
		nvgBeginFrame(editorCtx->vg, regCtx->bar[i].rect.w, regCtx->bar[i].rect.h, 1.0f); {
			nvgBeginPath(editorCtx->vg);
			nvgRect(
				editorCtx->vg,
				0,
				0,
				regCtx->bar[i].rect.w,
				regCtx->bar[i].rect.h
			);
			nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_SPLITTER_DARKER));
			nvgFill(editorCtx->vg);
			
			if (i == 1) {
				nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_BASE_WHITE));
				nvgFontSize(editorCtx->vg, 30 - 16);
				nvgFontFace(editorCtx->vg, "sans");
				nvgTextAlign(editorCtx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
				nvgText(
					editorCtx->vg,
					8,
					8,
					Region_Debug_GetStates(regCtx->nodeHead),
					NULL
				);
			}
		} nvgEndFrame(editorCtx->vg);
	}
}