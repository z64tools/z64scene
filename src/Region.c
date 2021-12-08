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
	Vec2i a;
	Vec2i b;
	
	if (flag & REG_CORNER_TL) {
		a = (Vec2i) {
			reg->mousePos.x,
			reg->mousePos.y
		};
		b = (Vec2i) {
			0,
			0
		};
	} else if (flag & REG_CORNER_TR) {
		a = (Vec2i) {
			reg->mousePos.x,
			reg->mousePos.y
		};
		b = (Vec2i) {
			reg->rect.w,
			0
		};
	} else if (flag & REG_CORNER_BL) {
		a = (Vec2i) {
			reg->mousePos.x,
			reg->mousePos.y
		};
		b = (Vec2i) {
			0,
			reg->rect.h
		};
	} else if (flag & REG_CORNER_BR) {
		a = (Vec2i) {
			reg->mousePos.x,
			reg->mousePos.y
		};
		b = (Vec2i) {
			reg->rect.w,
			reg->rect.h
		};
	} else if (flag & REG_SIDE_T) {
		a = (Vec2i) {
			0,
			reg->mousePos.y
		};
		b = (Vec2i) {
			0,
			0
		};
	} else if (flag & REG_SIDE_L) {
		a = (Vec2i) {
			reg->mousePos.x,
			0
		};
		b = (Vec2i) {
			0,
			0
		};
	} else if (flag & REG_SIDE_B) {
		a = (Vec2i) {
			0,
			reg->mousePos.y
		};
		b = (Vec2i) {
			0,
			reg->rect.h
		};
	} else if (flag & REG_SIDE_R) {
		a = (Vec2i) {
			reg->mousePos.x,
			0
		};
		b = (Vec2i) {
			reg->rect.w,
			0
		};
	}
	
	return Vec_Vec2i_DistXZ(&a, &b);
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
	
	Region_SetBotBarHeight(editorCtx, regCtx->bar[REG_BAR_BOT].rect.h);
	
	regCtx->nodeHead->rect = (Recti) {
		regCtx->workRegion.x,
		regCtx->workRegion.y,
		regCtx->workRegion.w,
		regCtx->workRegion.h
	};
	
	while (reg) {
		if (reg->update || reg == regCtx->nodeHead) {
			Vec2i rectPos = {
				reg->rect.x,
				reg->rect.y
			};
			Vec_Vec2i_Substract(
				&reg->mousePos,
				&mouse->pos,
				(Vec2i*)&rectPos
			);
			
			if (reg != regCtx->nodeHead)
				reg->update(editorCtx, reg);
		} else {
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
				char txt[128];
				sprintf(
					txt,
					"%04d %04d - %04d %04d - %4d",
					mouse->pos.x,
					mouse->pos.y,
					regCtx->nodeHead->mousePos.x,
					regCtx->nodeHead->mousePos.y,
					Region_GetDistTo(REG_CORNER_TL, regCtx->nodeHead)
				);
				nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_BASE_WHITE));
				nvgFontSize(editorCtx->vg, 30 - 14);
				nvgFontFace(editorCtx->vg, "sans");
				nvgTextAlign(editorCtx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
				nvgText(
					editorCtx->vg,
					7,
					7,
					txt,
					NULL
				);
			}
		} nvgEndFrame(editorCtx->vg);
	}
}