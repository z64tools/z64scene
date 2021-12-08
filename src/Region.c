#include "Editor.h"

static
void Region_UpdateWorkRegion(EditorContext* editorCtx) {
	RegionContext* regCtx = &editorCtx->regionCtx;
	Vec2i* winDim = &editorCtx->appInfo.winDim;
	
	regCtx->workRegion = (Recti) {
		0,
		0 + regCtx->bar[REG_BAR_TOP].rect.h,
		winDim->x,
		winDim->y - regCtx->bar[REG_BAR_BOT].rect.h - regCtx->bar[REG_BAR_TOP].rect.h
	};
}

static
void Region_SetTopBarHeight(EditorContext* editorCtx, s32 h) {
	RegionContext* regCtx = &editorCtx->regionCtx;
	Vec2i* winDim = &editorCtx->appInfo.winDim;
	
	regCtx->bar[REG_BAR_TOP].rect.x = 0;
	regCtx->bar[REG_BAR_TOP].rect.y = 0;
	regCtx->bar[REG_BAR_TOP].rect.w = winDim->x;
	regCtx->bar[REG_BAR_TOP].rect.h = h;
	Region_UpdateWorkRegion(editorCtx);
}

static
void Region_SetBotBarHeight(EditorContext* editorCtx, s32 h) {
	RegionContext* regCtx = &editorCtx->regionCtx;
	Vec2i* winDim = &editorCtx->appInfo.winDim;
	
	regCtx->bar[REG_BAR_BOT].rect.x = 0;
	regCtx->bar[REG_BAR_BOT].rect.y = winDim->y - h;
	regCtx->bar[REG_BAR_BOT].rect.w = winDim->x;
	regCtx->bar[REG_BAR_BOT].rect.h = h;
	Region_UpdateWorkRegion(editorCtx);
}

static
void Region_DrawRegionBorders(NVGcontext* vg, Recti* rect) {
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

static
s32 Region_GetDistTo(RegionPointFlag flag, Region* reg) {
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

static
bool Region_IsCursorInRegion(Region* region) {
	f32 resX = (f32)region->mousePos.x / region->rect.w;
	f32 resY = (f32)region->mousePos.y / region->rect.h;
	
	if (resX >= 0 && resX <= 1.0f && resY >= 0 && resY <= 1.0f) {
		return true;
	} else {
		return false;
	}
}

static
void Region_UnsetActionRegion(RegionContext* regCtx) {
	if (regCtx->actionLockedRegion == NULL)
		return;
	regCtx->actionLockedRegion->stateFlag &= ~(
		REG_STATE_SPLIT_TL |
		REG_STATE_SPLIT_TR |
		REG_STATE_SPLIT_BL |
		REG_STATE_SPLIT_BR
	);
	
	regCtx->actionLockedRegion = NULL;
}

static
void Region_ActionReg_Update(RegionContext* regCtx) {
	Region* reg = regCtx->actionLockedRegion;
	RegionState splitState = (
		REG_STATE_SPLIT_TL |
		REG_STATE_SPLIT_TR |
		REG_STATE_SPLIT_BL |
		REG_STATE_SPLIT_BR
	);
	
	if (reg->mousePress) {
		for (s32 i = REG_POINT_TL; i <= REG_POINT_BR; i = i << 1) {
			if (Region_GetDistTo(i, reg) < 20) {
				reg->stateFlag |= i;
				break;
			}
		}
	}
	
	if (reg->stateFlag & splitState) {
		if (Region_GetDistTo(reg->stateFlag & splitState, reg) > 20) {
			Region_UnsetActionRegion(regCtx);
		}
	}
}

/* / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Region_Init(EditorContext* editorCtx) {
	RegionContext* regCtx = &editorCtx->regionCtx;
	Vec2i* winDim = &editorCtx->appInfo.winDim;
	
	regCtx->nodeHead = Lib_Calloc(0, sizeof(Region));
	Region_SetTopBarHeight(editorCtx, 30);
	Region_SetBotBarHeight(editorCtx, 30);
}

void Region_Update(EditorContext* editorCtx) {
	RegionContext* regCtx = &editorCtx->regionCtx;
	Region* reg = regCtx->nodeHead;
	Region* killReg;
	MouseInput* mouse = &editorCtx->inputCtx.mouse;
	Vec2i* winDim = &editorCtx->appInfo.winDim;
	
	Region_SetTopBarHeight(editorCtx, regCtx->bar[REG_BAR_BOT].rect.h);
	Region_SetBotBarHeight(editorCtx, regCtx->bar[REG_BAR_BOT].rect.h);
	
	regCtx->nodeHead->rect = (Recti) {
		regCtx->workRegion.x,
		regCtx->workRegion.y,
		regCtx->workRegion.w,
		regCtx->workRegion.h
	};
	
	if (regCtx->actionLockedRegion && mouse->cursorAction == false) {
		Region_UnsetActionRegion(regCtx);
	}
	
	while (reg) {
		if (reg->update || reg == regCtx->nodeHead) {
			// Update Region Instance
			Vec2i rectPos = {
				reg->rect.x,
				reg->rect.y
			};
			Vec_Vec2i_Substract(
				&reg->mousePos,
				&mouse->pos,
				(Vec2i*)&rectPos
			);
			
			if (regCtx->actionLockedRegion == NULL &&
			    Region_IsCursorInRegion(reg) &&
			    mouse->cursorAction) {
				if (mouse->click.press) {
					reg->mousePressPos = reg->mousePos;
					reg->mousePress = true;
				}
				regCtx->actionLockedRegion = reg;
			}
			
			if (regCtx->actionLockedRegion)
				Region_ActionReg_Update(regCtx);
			
			if (reg != regCtx->nodeHead &&
			    ((regCtx->actionLockedRegion && regCtx->actionLockedRegion == reg) ||
			    regCtx->actionLockedRegion == NULL)) {
				reg->update(editorCtx, reg);
			}
			
			reg->mousePress = false;
		} else {
			// Kill Region Instance
			if (reg->next) {
				reg->next->prev = reg->prev;
			}
			
			if (reg->prev) {
				reg->prev->next = reg->next;
			} else {
				regCtx->nodeHead = reg->next;
			}
			
			killReg = reg;
			
			free(killReg);
		}
		
		reg = reg->next;
	}
}

void Region_Draw(EditorContext* editorCtx) {
	RegionContext* regCtx = &editorCtx->regionCtx;
	Region* reg = regCtx->nodeHead->next;
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
			Region_DrawRegionBorders(editorCtx->vg, &reg->rect);
		} nvgEndFrame(editorCtx->vg);
		
		reg = reg->next;
	}
	// Draw NodeHead Borders
	if ((reg = regCtx->nodeHead) != NULL) {
		glViewport(
			reg->rect.x,
			winDim->y - reg->rect.y - reg->rect.h,
			reg->rect.w,
			reg->rect.h
		);
		nvgBeginFrame(editorCtx->vg, reg->rect.w, reg->rect.h, 1.0f); {
			Region_DrawRegionBorders(editorCtx->vg, &reg->rect);
		} nvgEndFrame(editorCtx->vg);
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
				char* txt[] = {
					"REG_STATE_NONE",
					"REG_STATE_SPLIT_TL",
					"REG_STATE_SPLIT_TR",
					"REG_STATE_SPLIT_BL",
					"REG_STATE_SPLIT_BR"
				};
				s32 i = 0;
				
				if (reg->stateFlag & REG_STATE_SPLIT_TL) {
					i = 1;
				} else if (reg->stateFlag & REG_STATE_SPLIT_TR) {
					i = 2;
				} else if (reg->stateFlag & REG_STATE_SPLIT_BL) {
					i = 3;
				} else if (reg->stateFlag & REG_STATE_SPLIT_BR) {
					i = 4;
				}
				
				nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_BASE_WHITE));
				nvgFontSize(editorCtx->vg, 30 - 14);
				nvgFontFace(editorCtx->vg, "sans");
				nvgTextAlign(editorCtx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
				nvgText(
					editorCtx->vg,
					7,
					7,
					txt[i],
					NULL
				);
			}
		} nvgEndFrame(editorCtx->vg);
	}
}