#include "GeoGrid.h"

typedef enum {
	ELEM_ID_BUTTON,
	
	ELEM_ID_MAX,
} ElementIndex;

typedef struct {
	void*  arg;
	Split* split;
	ElementIndex    type;
	GeoGridContext* geoCtx;
} ElementCallInfo;

typedef void (* ElementFunc)(ElementCallInfo*);

/* ───────────────────────────────────────────────────────────────────────── */

static ElementCallInfo* sCurrentElement;
static ElementCallInfo* sElementStack;
static u32 sElemNum;

/* ───────────────────────────────────────────────────────────────────────── */

void Elements_QueueElement(GeoGridContext* geoCtx, Split* split, ElementIndex type, void* arg) {
	sCurrentElement->geoCtx = geoCtx;
	sCurrentElement->split = split;
	sCurrentElement->type = type;
	sCurrentElement->arg = arg;
	sCurrentElement++;
	sElemNum++;
}

/* ───────────────────────────────────────────────────────────────────────── */

s32 Element_Button(GeoGridContext* geoCtx, Split* split, ElButton* button, Rect* rect) {
	u32 set = 0;
	
	if (rect == NULL) {
		rect = &button->rect;
	} else {
		button->rect = *rect;
	}
	
	button->hover = 0;
	button->state = 0;
	if (split->mouseInSplit && !split->blockMouse && GeoGrid_Cursor_InRect(split, rect)) {
		if (geoCtx->mouse->clickL.press) {
			button->state++;
		}
		
		if (geoCtx->mouse->clickL.hold) {
			button->state++;
		}
		
		button->hover = 1;
	}
	
	Elements_QueueElement(
		geoCtx,
		split,
		ELEM_ID_BUTTON,
		button
	);
	
	return button->state;
}

void Element_Draw_Button(ElementCallInfo* info) {
	void* vg = info->geoCtx->vg;
	Split* split = info->split;
	ElButton* button = info->arg;
	
	nvgBeginPath(vg);
	nvgFillColor(vg, Theme_GetColor(THEME_LGHT, 175));
	nvgRoundedRect(
		vg,
		button->rect.x - 1.0f,
		button->rect.y - 1.0f,
		button->rect.w + 1.0f * 2,
		button->rect.h + 1.0f * 2,
		SPLIT_ROUND_R
	);
	nvgFill(vg);
	
	nvgBeginPath(vg);
	
	if (button->hover) {
		if (button->state) {
			nvgFillColor(vg, Theme_GetColor(THEME_BUTP, 175));
		} else {
			nvgFillColor(vg, Theme_GetColor(THEME_BUTH, 175));
		}
	} else {
		nvgFillColor(vg, Theme_GetColor(THEME_BUTI, 175));
	}
	nvgRoundedRect(vg, button->rect.x, button->rect.y, button->rect.w, button->rect.h, SPLIT_ROUND_R);
	nvgFill(vg);
	
	if (button->txt) {
		nvgFillColor(vg, Theme_GetColor(THEME_TEXT, 255));
		nvgFontFace(vg, "sans");
		nvgFontSize(vg, SPLIT_TEXT_SCALE);
		nvgFontBlur(vg, 0.0);
		nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
		nvgText(
			vg,
			button->rect.x + SPLIT_TEXT_SPLIT,
			button->rect.y + SPLIT_TEXT_SPLIT,
			button->txt,
			NULL
		);
	}
}

/* ───────────────────────────────────────────────────────────────────────── */

ElementFunc sFuncTable[] = {
	Element_Draw_Button
};

void Elements_Init(GeoGridContext* geoCtx) {
	sElementStack = sCurrentElement = Malloc(0, sizeof(ElementCallInfo) * 512);
}

void Elements_Update(GeoGridContext* geoCtx) {
	sCurrentElement = sElementStack;
	sElemNum = 0;
}

void Elements_Draw(GeoGridContext* geoCtx, Split* split) {
	for (s32 i = 0; i < sElemNum; i++) {
		if (sElementStack[i].split == split)
			sFuncTable[sElementStack[i].type](&sElementStack[i]);
	}
}