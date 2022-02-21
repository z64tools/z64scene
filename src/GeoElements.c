#include "GeoGrid.h"

struct ElementCallInfo;

typedef void (* ElementFunc)(struct ElementCallInfo*);

typedef struct ElementCallInfo {
	void*  arg;
	Split* split;
	ElementFunc func;
	GeoGridContext* geoCtx;
} ElementCallInfo;

/* ───────────────────────────────────────────────────────────────────────── */

static ElementCallInfo pElementStack[1024 * 2];
static ElementCallInfo* sCurrentElement;
static u32 sElemNum;

static ElementCallInfo pPostStack[1024 * 2];
static ElementCallInfo* sCurrentPost;
static u32 sPostNum;

static ElTextbox* sCurTextbox;
static Split* sCurSplitTextbox;
static s32 sTextPos;
static s32 sSelectPos = -1;
static char* sStoreA;

static s32 sFlickTimer;
static s32 sFlickFlag = 1;

static s16 sBreatheYaw;
static f32 sBreathe;

/* ───────────────────────────────────────────────────────────────────────── */

static void Element_QueueElement(GeoGridContext* geoCtx, Split* split, ElementFunc func, void* arg) {
	sCurrentElement->geoCtx = geoCtx;
	sCurrentElement->split = split;
	sCurrentElement->func = func;
	sCurrentElement->arg = arg;
	sCurrentElement++;
	sElemNum++;
}

void Element_PushToPost() {
	sCurrentPost[0] = sCurrentElement[-1];
	
	sCurrentPost++;
	sPostNum++;
	
	sCurrentElement--;
	sElemNum--;
}

static void Element_Draw_RoundedOutline(void* vg, Rect* rect, NVGcolor color) {
	nvgBeginPath(vg);
	nvgFillColor(vg, color);
	nvgRoundedRect(
		vg,
		rect->x - 1,
		rect->y - 1,
		rect->w + 2,
		rect->h + 2,
		SPLIT_ROUND_R
	);
	nvgFill(vg);
}

static void Element_Draw_RoundedRect(void* vg, Rect* rect, NVGcolor color) {
	nvgBeginPath(vg);
	nvgFillColor(vg, color);
	nvgRoundedRect(
		vg,
		rect->x,
		rect->y,
		rect->w,
		rect->h,
		SPLIT_ROUND_R
	);
	nvgFill(vg);
}

static s32 Element_PressCondition(GeoGridContext* geoCtx, Split* split, Rect* rect) {
	return (geoCtx->ctxMenu.num == 0 &&
	       split->mouseInSplit &&
	       !split->blockMouse &&
	       GeoGrid_Cursor_InRect(split, rect));
}

static void Element_Draw_TextOutline(void* vg, f32 x, f32 y, char* txt) {
	nvgFillColor(vg, Theme_GetColor(THEME_LINE, 80));
	nvgFontBlur(vg, 0.55);
	
	nvgText(
		vg,
		x + 1,
		y + 1,
		txt,
		NULL
	);
	nvgText(
		vg,
		x + 1,
		y - 1,
		txt,
		NULL
	);
	nvgText(
		vg,
		x - 1,
		y + 1,
		txt,
		NULL
	);
	nvgText(
		vg,
		x - 1,
		y - 1,
		txt,
		NULL
	);
}

/* ───────────────────────────────────────────────────────────────────────── */

static void Element_Draw_Button(ElementCallInfo* info) {
	void* vg = info->geoCtx->vg;
	Split* split = info->split;
	ElButton* this = info->arg;
	
	Rect rect = this->rect;
	s32 alpha = this->hover ? 300 : 255;
	ThemeColor colID = this->hover ? THEME_PRIM : THEME_ACCENT;
	
	rect.h = SPLIT_TEXT_MED * 1.5;
	
	if (rect.w < 16) {
		return;
	}
	
	if (this->toggle == 2)
		Theme_SmoothStepToCol(&this->colorOL, Theme_GetColor(colID, 355), 0.16, 0.1, 0.0);
	else
		Theme_SmoothStepToCol(&this->colorOL, Theme_GetColor(THEME_LINE, 355), 0.16, 0.1, 0.0);
	
	Element_Draw_RoundedOutline(vg, &rect, this->colorOL);
	
	nvgBeginPath(vg);
	if (this->toggle == 2) {
		Theme_SmoothStepToCol(&this->colorIL, Theme_GetColor(colID, alpha), 0.16, 0.1, 0.0);
	} else {
		if (this->state) {
			if (this->toggle)
				Theme_SmoothStepToCol(&this->colorIL, Theme_GetColor(THEME_BUTTON_PRESS, alpha), 0.16, 0.1, 0.0);
			else
				Theme_SmoothStepToCol(&this->colorIL, Theme_GetColor(colID, alpha), 0.66, 0.4, 0.0);
		} else {
			Theme_SmoothStepToCol(&this->colorIL, Theme_GetColor(THEME_BUTTON_HOVER, alpha), 0.16, 0.1, 0.0);
		}
	}
	
	nvgFillColor(vg, this->colorIL);
	nvgRoundedRect(vg, rect.x, rect.y, rect.w, rect.h, SPLIT_ROUND_R);
	nvgFill(vg);
	
	if (this->txt) {
		nvgFontFace(vg, "font-basic");
		nvgFontSize(vg, SPLIT_TEXT);
		nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		nvgTextLetterSpacing(vg, 0.0);
		
		if (colID == THEME_PRIM) {
			Element_Draw_TextOutline(
				vg,
				rect.x + SPLIT_TEXT_PADDING,
				rect.y + rect.h * 0.5,
				this->txt
			);
		}
		
		nvgFillColor(vg, Theme_GetColor(THEME_TEXT, alpha));
		nvgFontBlur(vg, 0.0);
		nvgText(
			vg,
			rect.x + SPLIT_TEXT_PADDING,
			rect.y + rect.h * 0.5,
			this->txt,
			NULL
		);
	}
}

static void Element_Draw_Textbox(ElementCallInfo* info) {
	void* vg = info->geoCtx->vg;
	Split* split = info->split;
	ElTextbox* this = info->arg;
	
	Rectf32 bound = { 0 };
	Rectf32 sel = { 0 };
	static char buffer[512]; strcpy(buffer, this->txt);
	char* txtA = buffer;
	char* txtB = &buffer[strlen(buffer)];
	
	if (this->rect.w < 16) {
		return;
	}
	
	nvgFontFace(vg, "font-basic");
	nvgFontSize(vg, SPLIT_TEXT);
	nvgFontBlur(vg, 0.0);
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
	nvgTextLetterSpacing(vg, 0.1);
	nvgTextBounds(vg, 0, 0, txtA, txtB, (f32*)&bound);
	
	if (sStoreA != NULL && this == sCurTextbox) {
		if (bound.w < this->rect.w) {
			sStoreA = buffer;
		} else {
			txtA = sStoreA;
			nvgTextBounds(vg, 0, 0, txtA, txtB, (f32*)&bound);
			while (bound.w > this->rect.w - SPLIT_TEXT_PADDING * 2) {
				txtB--;
				nvgTextBounds(vg, 0, 0, txtA, txtB, (f32*)&bound);
			}
			
			if (strlen(buffer) > 4) {
				s32 posA = (uPtr)txtA - (uPtr)buffer;
				s32 posB = (uPtr)txtB - (uPtr)buffer;
				
				if (sTextPos < posA || sTextPos > posB + 1 || posB - posA <= 4) {
					if (sTextPos < posA) {
						txtA -= posA - sTextPos;
					}
					if (sTextPos > posB + 1) {
						txtA += sTextPos - posB + 1;
					}
					if (posB - posA <= 4)
						txtA += (posB - posA) - 4;
					
					txtB = &buffer[strlen(buffer)];
					sStoreA = txtA;
					
					nvgTextBounds(vg, 0, 0, txtA, txtB, (f32*)&bound);
					while (bound.w > this->rect.w - SPLIT_TEXT_PADDING * 2) {
						txtB--;
						nvgTextBounds(vg, 0, 0, txtA, txtB, (f32*)&bound);
					}
				}
			}
		}
	} else {
		while (bound.w > this->rect.w - SPLIT_TEXT_PADDING * 2) {
			txtB--;
			nvgTextBounds(vg, 0, 0, txtA, txtB, (f32*)&bound);
		}
		
		if (this == sCurTextbox)
			sStoreA = txtA;
	}
	
	InputContext* inputCtx = info->geoCtx->input;
	s32 posA = (uPtr)txtA - (uPtr)buffer;
	s32 posB = (uPtr)txtB - (uPtr)buffer;
	
	if (GeoGrid_Cursor_InRect(split, &this->rect) && inputCtx->mouse.clickL.hold) {
		if (inputCtx->mouse.clickL.press) {
			f32 dist = 400;
			for (char* tempB = txtA; tempB <= txtB; tempB++) {
				Vec2s glyphPos;
				f32 res;
				nvgTextBounds(vg, 0, 0, txtA, tempB, (f32*)&bound);
				glyphPos.x = this->rect.x + bound.w + SPLIT_TEXT_PADDING - 1;
				glyphPos.y = this->rect.y + bound.h - 1 + SPLIT_TEXT * 0.5;
				
				res = Vec_Vec2s_DistXZ(&split->mousePos, &glyphPos);
				
				if (res < dist) {
					dist = res;
					
					sTextPos = (uPtr)tempB - (uPtr)buffer;
				}
			}
		} else {
			f32 dist = 400;
			uPtr wow;
			for (char* tempB = txtA; tempB <= txtB; tempB++) {
				Vec2s glyphPos;
				f32 res;
				nvgTextBounds(vg, 0, 0, txtA, tempB, (f32*)&bound);
				glyphPos.x = this->rect.x + bound.w + SPLIT_TEXT_PADDING - 1;
				glyphPos.y = this->rect.y + bound.h - 1 + SPLIT_TEXT * 0.5;
				
				res = Vec_Vec2s_DistXZ(&split->mousePos, &glyphPos);
				
				if (res < dist) {
					dist = res;
					wow = (uPtr)tempB - (uPtr)buffer;
				}
			}
			
			if (wow != sTextPos) {
				sSelectPos = wow;
			}
		}
	}
	
	this->rect.h = SPLIT_TEXT_MED * 1.5;
	
	if (this != sCurTextbox)
		Theme_SmoothStepToCol(&this->bgCl, Theme_GetColor(THEME_LIGHT, 255), 0.25, 0.15, 0.0);
	else
		Theme_SmoothStepToCol(&this->bgCl, Theme_GetColor(THEME_PRIM, 255), 0.25, 0.05, 0.0);
	Element_Draw_RoundedOutline(vg, &this->rect, this->bgCl);
	
	nvgBeginPath(vg);
	nvgFillColor(vg, Theme_GetColor(THEME_LINE, 255));
	nvgRoundedRect(vg, this->rect.x, this->rect.y, this->rect.w, this->rect.h, SPLIT_ROUND_R);
	nvgFill(vg);
	
	if (this == sCurTextbox) {
		if (sSelectPos != -1) {
			s32 min = fmin(sSelectPos, sTextPos);
			s32 max = fmax(sSelectPos, sTextPos);
			f32 x, xmax;
			
			if (txtA < &buffer[min]) {
				nvgTextBounds(vg, 0, 0, txtA, &buffer[min], (f32*)&bound);
				x = this->rect.x + bound.w + SPLIT_TEXT_PADDING - 1;
			} else {
				x = this->rect.x + SPLIT_TEXT_PADDING;
			}
			
			nvgTextBounds(vg, 0, 0, txtA, &buffer[max], (f32*)&bound);
			xmax = this->rect.x + bound.w + SPLIT_TEXT_PADDING - 1 - x;
			
			nvgBeginPath(vg);
			nvgFillColor(vg, Theme_GetColor(THEME_PRIM, 255));
			nvgRoundedRect(vg, x, this->rect.y + 2, CLAMP_MAX(xmax, this->rect.w - SPLIT_TEXT_PADDING - 2), this->rect.h - 4, SPLIT_ROUND_R);
			nvgFill(vg);
		} else {
			sFlickTimer++;
			
			if (sFlickTimer % 30 == 0)
				sFlickFlag ^= 1;
			
			if (sFlickFlag) {
				nvgBeginPath(vg);
				nvgTextBounds(vg, 0, 0, txtA, &buffer[sTextPos], (f32*)&bound);
				nvgBeginPath(vg);
				nvgFillColor(vg, Theme_GetColor(THEME_PRIM, 355));
				nvgRoundedRect(vg, this->rect.x + bound.w + SPLIT_TEXT_PADDING - 1, this->rect.y + bound.h - 3, 2, SPLIT_TEXT, SPLIT_ROUND_R);
				nvgFill(vg);
			}
		}
	} else {
		if (&buffer[strlen(buffer)] != txtB)
			for (s32 i = 0; i < 3; i++)
				txtB[-i] = '.';
	}
	if (this == sCurTextbox)
		nvgFillColor(vg, Theme_GetColor(THEME_TEXT_HIGHLIGHT, 255));
	else
		nvgFillColor(vg, Theme_GetColor(THEME_TEXT, 255));
	nvgText(
		vg,
		this->rect.x + SPLIT_TEXT_PADDING,
		this->rect.y + this->rect.h * 0.5,
		txtA,
		txtB
	);
}

static void Element_Draw_Text(ElementCallInfo* info) {
	void* vg = info->geoCtx->vg;
	Split* split = info->split;
	ElText* this = info->arg;
	
	nvgFontFace(vg, "font-basic");
	nvgFontSize(vg, SPLIT_TEXT);
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
	
	nvgFontBlur(vg, 1.5);
	nvgFillColor(vg, Theme_GetColor(THEME_SHADOW, 255));
	nvgText(
		vg,
		this->rect.x,
		this->rect.y + this->rect.h * 0.5,
		this->txt,
		NULL
	);
	
	nvgFontBlur(vg, 0.0);
	nvgFillColor(vg, Theme_GetColor(THEME_TEXT, 255));
	nvgText(
		vg,
		this->rect.x,
		this->rect.y + this->rect.h * 0.5,
		this->txt,
		NULL
	);
}

static void Element_Draw_Checkbox(ElementCallInfo* info) {
	void* vg = info->geoCtx->vg;
	Split* split = info->split;
	ElCheckbox* this = info->arg;
	Vec2f center;
	ThemeColor colID = this->hover ? THEME_PRIM : THEME_ACCENT;
	const Vec2f sVector_Cross[] = {
		{ .x = -10, .y =  10 }, { .x =  -7, .y =  10 },
		{ .x =   0, .y =   3 }, { .x =   7, .y =  10 },
		{ .x =  10, .y =  10 }, { .x =  10, .y =   7 },
		{ .x =   3, .y =   0 }, { .x =  10, .y =  -7 },
		{ .x =  10, .y = -10 }, { .x =   7, .y = -10 },
		{ .x =   0, .y =  -3 }, { .x =  -7, .y = -10 },
		{ .x = -10, .y = -10 }, { .x = -10, .y =  -7 },
		{ .x =  -3, .y =   0 }, { .x = -10, .y =   7 },
	};
	
	if (this->toggle) {
		Math_SmoothStepToF(&this->lerp, 0.8f - sBreathe * 0.08, 0.178f, 0.1f, 0.0f);
		Theme_SmoothStepToCol(&this->color, Theme_GetColor(colID, 255), 0.16, 0.13, 0.0);
	} else {
		Math_SmoothStepToF(&this->lerp, 0.0f, 0.268f, 0.1f, 0.0f);
		Theme_SmoothStepToCol(&this->color, Theme_GetColor(THEME_LIGHT, 255), 0.16, 0.13, 0.0);
	}
	
	Element_Draw_RoundedOutline(vg, &this->rect, this->color);
	Element_Draw_RoundedRect(vg, &this->rect, Theme_GetColor(THEME_LINE, 255));
	
	NVGcolor col = this->color;
	f32 flipLerp = 1.0f - this->lerp;
	
	flipLerp = (1.0f - powf(flipLerp, 1.6));
	center.x = this->rect.x + this->rect.w * 0.5;
	center.y = this->rect.y + this->rect.h * 0.5;
	
	col.a = flipLerp * 1.67;
	col.a = CLAMP_MIN(col.a, 0.80);
	
	nvgBeginPath(vg);
	nvgFillColor(vg, col);
	
	for (s32 i = 0; i < ArrayCount(sVector_Cross); i++) {
		s32 wi = Wrap(i, 0, ArrayCount(sVector_Cross) - 1);
		Vec2f zero = { 0 };
		Vec2f pos = {
			sVector_Cross[wi].x * 0.75,
			sVector_Cross[wi].y * 0.75,
		};
		f32 dist = Vec_Vec2f_DistXZ(&zero, &pos);
		s16 yaw = Vec_Vec2f_Yaw(&zero, &pos);
		
		dist = Lerp(flipLerp, 4, dist);
		dist = Lerp((this->lerp > 0.5 ? 1.0 - this->lerp : this->lerp), dist, powf((dist * 0.1), 0.15) * 3);
		
		pos.x = center.x + Math_SinS(yaw) * dist;
		pos.y = center.y + Math_CosS(yaw) * dist;
		
		if ( i == 0 )
			nvgMoveTo(vg, pos.x, pos.y);
		else
			nvgLineTo(vg, pos.x, pos.y);
	}
	nvgFill(vg);
}

static void Element_Draw_Slider(ElementCallInfo* info) {
	void* vg = info->geoCtx->vg;
	Split* split = info->split;
	ElSlider* this = info->arg;
	Rectf32 rect;
	
	Math_SmoothStepToF(&this->value, this->target, 0.25f, 0.75f, 0.0f);
	rect.x = this->rect.x;
	rect.y = this->rect.y;
	rect.w = this->rect.w;
	rect.h = this->rect.h;
	rect.w = CLAMP_MIN(rect.w * this->value, 0);
	
	if (this->holdState) {
		Theme_SmoothStepToCol(&this->color, Theme_GetColor(THEME_PRIM, 325), 0.25f, 0.5f, 0.0f);
	} else {
		Theme_SmoothStepToCol(&this->color, Theme_GetColor(THEME_ACCENT, 255), 0.25f, 0.5f, 0.0f);
	}
	
	Element_Draw_RoundedOutline(vg, &this->rect, Theme_GetColor(THEME_LIGHT, 255));
	Element_Draw_RoundedRect(vg, &this->rect, Theme_GetColor(THEME_SHADOW, 255));
	
	if (this->value <= 0.02) {
		this->color.a = Lerp(CLAMP_MIN((this->value - 0.01f) * 100.0f, 0.0f), 0.0, this->color.a);
	}
	
	nvgBeginPath(vg);
	nvgFillColor(vg, this->color);
	nvgRoundedRect(
		vg,
		rect.x + 1,
		rect.y + 1,
		rect.w - 2,
		rect.h - 2,
		SPLIT_ROUND_R
	);
	nvgFill(vg);
	
	snprintf(this->txt, 31, "%.3f", Lerp(this->target, this->min, this->max));
	
	nvgFontFace(vg, "font-basic");
	nvgFontSize(vg, SPLIT_TEXT);
	nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	
	if (this->holdState) {
		Element_Draw_TextOutline(
			vg,
			this->rect.x + this->rect.w * 0.5,
			this->rect.y + this->rect.h * 0.5,
			this->txt
		);
	}
	nvgFillColor(vg, Theme_GetColor(THEME_TEXT, 255));
	nvgFontBlur(vg, 0.0);
	nvgText(
		vg,
		this->rect.x + this->rect.w * 0.5,
		this->rect.y + this->rect.h * 0.5,
		this->txt,
		NULL
	);
}

/* ───────────────────────────────────────────────────────────────────────── */

// Returns button state, 0bXY, X == toggle, Y == pressed
s32 Element_Button(GeoGridContext* geoCtx, Split* split, ElButton* this) {
	s32 set = 0;
	void* vg = geoCtx->vg;
	
	if (this->txt) {
		Rectf32 txtb = { 0 };
		
		nvgFontFace(vg, "font-basic");
		nvgFontSize(vg, SPLIT_TEXT);
		nvgFontBlur(vg, 0.0);
		nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		nvgTextLetterSpacing(vg, 0.0);
		nvgTextBounds(vg, 0, 0, this->txt, NULL, (f32*)&txtb);
		
		this->rect.w = fmax(this->rect.w, txtb.w + SPLIT_TEXT_PADDING * 2);
	}
	
	this->hover = 0;
	this->state = 0;
	if (Element_PressCondition(geoCtx, split, &this->rect)) {
		if (Input_GetMouse(MOUSE_L)->press) {
			this->state++;
			
			if (this->toggle) {
				u8 t = (this->toggle - 1) == 0; // Invert
				
				this->toggle = t + 1;
			}
		}
		
		if (Input_GetMouse(MOUSE_L)->hold) {
			this->state++;
		}
		
		this->hover = 1;
	}
	
	Element_QueueElement(
		geoCtx,
		split,
		Element_Draw_Button,
		this
	);
	
	return (this->state == 2) | (CLAMP_MIN(this->toggle - 1, 0)) << 4;
}

void Element_Textbox(GeoGridContext* geoCtx, Split* split, ElTextbox* this) {
	u32 set = 0;
	
	this->hover = 0;
	if (Element_PressCondition(geoCtx, split, &this->rect)) {
		this->hover = 1;
		if (Input_GetMouse(MOUSE_L)->press) {
			sCurTextbox = this;
			sCurSplitTextbox = split;
			sTextPos = strlen(this->txt);
		}
	}
	
	Element_QueueElement(
		geoCtx,
		split,
		Element_Draw_Textbox,
		this
	);
}

// Returns text width
f32 Element_Text(GeoGridContext* geoCtx, Split* split, ElText* this) {
	f32 bounds[4] = { 0 };
	void* vg = geoCtx->vg;
	
	nvgFontFace(vg, "font-basic");
	nvgFontSize(vg, SPLIT_TEXT);
	nvgFontBlur(vg, 0.0);
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
	nvgTextLetterSpacing(vg, 0.1);
	nvgFillColor(vg, Theme_GetColor(THEME_TEXT, 255));
	nvgTextBounds(vg, 0, 0, this->txt, NULL, bounds);
	
	Element_QueueElement(
		geoCtx,
		split,
		Element_Draw_Text,
		this
	);
	
	return bounds[2];
}

s32 Element_Checkbox(GeoGridContext* geoCtx, Split* split, ElCheckbox* this) {
	this->rect.w = this->rect.h = SPLIT_TEXT_H - SPLIT_TEXT_PADDING * 0.5;
	
	this->hover = 0;
	if (Element_PressCondition(geoCtx, split, &this->rect)) {
		this->hover = 1;
		if (Input_GetMouse(MOUSE_L)->press) {
			this->toggle ^= 1;
		}
	}
	
	Element_QueueElement(
		geoCtx,
		split,
		Element_Draw_Checkbox,
		this
	);
	
	return this->toggle;
}

f32 Element_Slider(GeoGridContext* geoCtx, Split* split, ElSlider* this) {
	if (this->min == 0.0f && this->max == 0.0f)
		this->max = 1.0f;
	
	if (Element_PressCondition(geoCtx, split, &this->rect) || this->holdState) {
		u32 pos = false;
		this->holdState = false;
		
		if (Input_GetMouse(MOUSE_L)->hold) {
			if (Input_GetKey(KEY_LEFT_SHIFT)->hold) {
				this->target += (f32)geoCtx->input->mouse.vel.x * 0.001;
				pos = true;
			} else {
				this->target = (f32)(split->mousePos.x - this->rect.x) / (this->rect.w);
			}
			Cursor_SetCursor(CURSOR_EMPTY);
			this->holdState = true;
		}
		
		if (geoCtx->input->mouse.scrollY) {
			if (Input_GetKey(KEY_LEFT_SHIFT)->hold) {
				this->target += geoCtx->input->mouse.scrollY * 0.1;
			} else if (Input_GetKey(KEY_LEFT_ALT)->hold) {
				this->target += geoCtx->input->mouse.scrollY * 0.001;
			} else {
				this->target += geoCtx->input->mouse.scrollY * 0.01;
			}
		}
		
		if (Input_GetMouse(MOUSE_L)->release || pos) {
			f32 x = split->rect.x + this->rect.x + this->rect.w * this->target;
			f32 y = split->rect.y + this->rect.y + this->rect.h * 0.5;
			
			Input_SetMousePos(x, y);
		}
	}
	
	this->target = CLAMP(this->target, 0.0, 1.0f);
	
	Element_QueueElement(
		geoCtx,
		split,
		Element_Draw_Slider,
		this
	);
	
	return Lerp(this->target, this->min, this->max);
}

/* ───────────────────────────────────────────────────────────────────────── */

void Element_SetRect_Text(Rect* rect, f32 x, f32 y, f32 w) {
	rect->x = x;
	rect->y = y;
	rect->w = w;
	rect->h = SPLIT_TEXT_H;
}

void Element_Init(GeoGridContext* geoCtx) {
	sCurrentElement = pElementStack;
	sCurrentPost = pPostStack;
}

void Element_Update(GeoGridContext* geoCtx) {
	static s32 timer = 0;
	
	sCurrentElement = pElementStack;
	sElemNum = 0;
	
	sCurrentPost = pPostStack;
	sPostNum = 0;
	
	sBreatheYaw += DegToBin(3);
	sBreathe = (Math_SinS(sBreatheYaw) + 1.0f) * 0.5;
	
	if (sCurTextbox) {
		char* txt = geoCtx->input->buffer;
		s32 prevTextPos = sTextPos;
		s32 press = 0;
		
		if (Input_GetMouse(MOUSE_ANY)->press || Input_GetKey(KEY_ENTER)->press) {
			sSelectPos = -1;
			if (!GeoGrid_Cursor_InRect(sCurSplitTextbox, &sCurTextbox->rect) || Input_GetKey(KEY_ENTER)->press) {
				sCurTextbox = NULL;
				sCurTextbox = NULL;
				sStoreA = NULL;
				sFlickFlag = 1;
				sFlickTimer = 0;
				
				return;
			}
		}
		
		if (Input_GetKey(KEY_LEFT_CONTROL)->hold) {
			if (Input_GetKey(KEY_A)->press) {
				prevTextPos = strlen(sCurTextbox->txt);
				sTextPos = 0;
			}
			if (Input_GetKey(KEY_V)->press) {
				txt = (char*)Input_GetClipboardStr();
				printf_debugExt("Paste [%s]", txt);
			}
			if (Input_GetKey(KEY_C)->press) {
				s32 max = fmax(sSelectPos, sTextPos);
				s32 min = fmin(sSelectPos, sTextPos);
				char* copy = Tmp_Alloc(512);
				
				memcpy(copy, &sCurTextbox->txt[min], max - min);
				Input_SetClipboardStr(copy);
				printf_debugExt("Copy [%s]", copy);
			}
			if (Input_GetKey(KEY_X)->press) {
				s32 max = fmax(sSelectPos, sTextPos);
				s32 min = fmin(sSelectPos, sTextPos);
				char* copy = Tmp_Alloc(512);
				
				memcpy(copy, &sCurTextbox->txt[min], max - min);
				Input_SetClipboardStr(copy);
				printf_debugExt("Copy [%s]", copy);
			}
		} else {
			if (Input_GetKey(KEY_LEFT)->press) {
				sTextPos--;
				press++;
				sFlickFlag = 1;
				sFlickTimer = 0;
			}
			
			if (Input_GetKey(KEY_RIGHT)->press) {
				sTextPos++;
				press++;
				sFlickFlag = 1;
				sFlickTimer = 0;
			}
			
			if (Input_GetKey(KEY_HOME)->press) {
				sTextPos = 0;
			}
			
			if (Input_GetKey(KEY_END)->press) {
				sTextPos = strlen(sCurTextbox->txt);
			}
			
			if (Input_GetKey(KEY_LEFT)->hold || Input_GetKey(KEY_RIGHT)->hold) {
				timer++;
			} else {
				timer = 0;
			}
			
			if (timer >= 30 && timer % 2 == 0) {
				if (Input_GetKey(KEY_LEFT)->hold) {
					sTextPos--;
				}
				
				if (Input_GetKey(KEY_RIGHT)->hold) {
					sTextPos++;
				}
			}
		}
		
		sTextPos = CLAMP(sTextPos, 0, strlen(sCurTextbox->txt));
		
		if (sTextPos != prevTextPos) {
			if (Input_GetKey(KEY_LEFT_SHIFT)->hold || Input_GetKey(KEY_LEFT_CONTROL)->hold) {
				if (sSelectPos == -1)
					sSelectPos = prevTextPos;
			} else
				sSelectPos = -1;
		} else if (press) {
			sSelectPos = -1;
		}
		
		if (Input_GetKey(KEY_BACKSPACE)->press || Input_GetShortcut(KEY_LEFT_CONTROL, KEY_X)) {
			if (sSelectPos != -1) {
				s32 max = fmax(sTextPos, sSelectPos);
				s32 min = fmin(sTextPos, sSelectPos);
				
				String_Remove(&sCurTextbox->txt[min], max - min);
				sTextPos = min;
				sSelectPos = -1;
			} else if (sTextPos != 0) {
				String_Remove(&sCurTextbox->txt[sTextPos - 1], 1);
				sTextPos--;
			}
		}
		
		if (txt[0] != '\0') {
			if (sSelectPos != -1) {
				s32 max = fmax(sTextPos, sSelectPos);
				s32 min = fmin(sTextPos, sSelectPos);
				
				String_Remove(&sCurTextbox->txt[min], max - min);
				sTextPos = min;
				sSelectPos = -1;
			}
			
			if (strlen(sCurTextbox->txt) == 0)
				strcpy(sCurTextbox->txt, txt);
			else
				String_Insert(&sCurTextbox->txt[sTextPos], txt);
			
			sTextPos += strlen(txt);
		}
		
		sTextPos = CLAMP(sTextPos, 0, strlen(sCurTextbox->txt));
	}
}

void Element_Draw(GeoGridContext* geoCtx, Split* split) {
	for (s32 i = 0; i < sElemNum; i++) {
		if (pElementStack[i].split == split)
			pElementStack[i].func(&pElementStack[i]);
	}
}

void Element_PostDraw(GeoGridContext* geoCtx, Split* split) {
	for (s32 i = 0; i < sPostNum; i++) {
		if (pPostStack[i].split == split)
			pPostStack[i].func(&pPostStack[i]);
	}
}