#include "GeoGrid.h"

typedef enum {
	ELEM_ID_BUTTON,
	
	ELEM_ID_MAX,
} ElementIndex;

typedef struct {
	f64   f64[2];
	s32   s32[2];
	s16   s16[2];
	s8    s8[4];
	Rect  rect;
	void* ptr[2];
} ElementArgs;

typedef struct {
	ElementArgs     args;
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

void Elements_QueueElement(GeoGridContext* geoCtx, Split* split, ElementIndex type, ElementArgs args) {
	sCurrentElement->geoCtx = geoCtx;
	sCurrentElement->split = split;
	sCurrentElement->type = type;
	sCurrentElement->args = args;
	sCurrentElement++;
	sElemNum++;
}

/* ───────────────────────────────────────────────────────────────────────── */

s32 Element_Button(GeoGridContext* geoCtx, Split* split, ElemButton* button, Rect* rect) {
	u32 set = 0;
	u32 hoverOver = 0;
	
	button->state = 0;
	if (split->mouseInSplit && !split->blockMouse && GeoGrid_Cursor_InRect(split, rect)) {
		if (geoCtx->mouse->clickL.press) {
			button->state++;
		}
		
		if (geoCtx->mouse->clickL.hold) {
			button->state++;
		}
		
		hoverOver = 1;
	}
	
	void* args[] = {
		button, rect, &hoverOver
	};
	
	Elements_QueueElement(
		geoCtx,
		split,
		ELEM_ID_BUTTON,
		(ElementArgs) {
		.rect = *rect,
		.s8[0] = hoverOver,
		.s8[1] = button->state,
		.ptr[0] = button->txt
	}
	);
	
	return button->state;
}

void Element_Draw_Button(ElementCallInfo* info) {
	void* vg = info->geoCtx->vg;
	Split* split = info->split;
	
	char* txt = info->args.ptr[0];
	u32 hover = info->args.s8[0];
	s8 buttonState = info->args.s8[1];
	Rect* rect = &info->args.rect;
	
	nvgBeginPath(vg);
	nvgFillColor(vg, Theme_GetColor(THEME_LGHT, 175));
	nvgRoundedRect(
		vg,
		rect->x - 1.0f,
		rect->y - 1.0f,
		rect->w + 1.0f * 2,
		rect->h + 1.0f * 2,
		SPLIT_ROUND_R
	);
	nvgFill(vg);
	
	nvgBeginPath(vg);
	
	if (hover) {
		if (buttonState) {
			nvgFillColor(vg, Theme_GetColor(THEME_BUTP, 175));
		} else {
			nvgFillColor(vg, Theme_GetColor(THEME_BUTH, 175));
		}
	} else {
		nvgFillColor(vg, Theme_GetColor(THEME_BUTI, 175));
	}
	nvgRoundedRect(vg, rect->x, rect->y, rect->w, rect->h, SPLIT_ROUND_R);
	nvgFill(vg);
}

/* ───────────────────────────────────────────────────────────────────────── */

ElementFunc sFuncTable[] = {
	Element_Draw_Button
};

void Elements_Init(GeoGridContext* geoCtx) {
	sElementStack = sCurrentElement = Lib_Malloc(0, sizeof(ElementCallInfo) * 512);
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