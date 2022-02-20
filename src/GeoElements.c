#include "GeoGrid.h"

typedef enum {
	ELEM_ID_BUTTON,
	ELEM_ID_TEXTBOX,
	
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

static ElementCallInfo pElementStack[1024 * 2];
static ElementCallInfo* sCurrentElement;
static u32 sElemNum;

static ElTextbox* sCurTextbox;
static Split* sCurSplitTextbox;
static s32 sTextPos;
static s32 sSelectPos = -1;
static char* sStoreA;

/* ───────────────────────────────────────────────────────────────────────── */

static void Elements_QueueElement(GeoGridContext* geoCtx, Split* split, ElementIndex type, void* arg) {
	sCurrentElement->geoCtx = geoCtx;
	sCurrentElement->split = split;
	sCurrentElement->type = type;
	sCurrentElement->arg = arg;
	sCurrentElement++;
	sElemNum++;
}

static void Element_Draw_Button(ElementCallInfo* info) {
	void* vg = info->geoCtx->vg;
	Split* split = info->split;
	ElButton* button = info->arg;
	f32 w = button->rect.w;
	
	button->rect.h = SPLIT_TEXT_MED * 1.5;
	
	if (button->txt) {
		Rectf32 txtb = { 0 };
		
		nvgFontFace(vg, "sans");
		nvgFontSize(vg, SPLIT_TEXT_SMALL);
		nvgFontBlur(vg, 0.0);
		nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		nvgTextLetterSpacing(vg, 1.05);
		nvgTextBounds(vg, button->rect.x, button->rect.y, button->txt, NULL, (f32*)&txtb);
		
		w = fmax(w, txtb.w);
	}
	
	nvgBeginPath(vg);
	nvgFillColor(vg, Theme_GetColor(THEME_LGHT, 175));
	nvgRoundedRect(
		vg,
		button->rect.x - 1.0f,
		button->rect.y - 1.0f,
		w + 1.0f * 2,
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
	nvgRoundedRect(vg, button->rect.x, button->rect.y, w, button->rect.h, SPLIT_ROUND_R);
	nvgFill(vg);
	
	if (button->txt) {
		nvgFillColor(vg, Theme_GetColor(THEME_TEXT, 175));
		nvgText(
			vg,
			button->rect.x + SPLIT_TEXT_PADDING,
			button->rect.y + button->rect.h * 0.5,
			button->txt,
			NULL
		);
	}
}

static void Element_Draw_Textbox(ElementCallInfo* info) {
	void* vg = info->geoCtx->vg;
	Split* split = info->split;
	ElTextbox* txtbox = info->arg;
	Rectf32 bound = { 0 };
	Rectf32 sel = { 0 };
	static char buffer[512]; strcpy(buffer, txtbox->txt);
	char* txtA = buffer;
	char* txtB = &buffer[strlen(buffer)];
	
	nvgFontFace(vg, "sans");
	nvgFontSize(vg, SPLIT_TEXT_SMALL);
	nvgFontBlur(vg, 0.0);
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
	nvgTextLetterSpacing(vg, 0.95);
	nvgTextBounds(vg, txtbox->rect.x, txtbox->rect.y, txtA, txtB, (f32*)&bound);
	
	if (sStoreA != NULL && txtbox == sCurTextbox) {
		if (bound.w < txtbox->rect.w) {
			sStoreA = buffer;
		} else {
			txtA = sStoreA;
			nvgTextBounds(vg, txtbox->rect.x, txtbox->rect.y, txtA, txtB, (f32*)&bound);
			while (bound.w > txtbox->rect.w) {
				txtB--;
				nvgTextBounds(vg, txtbox->rect.x, txtbox->rect.y, txtA, txtB, (f32*)&bound);
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
					
					nvgTextBounds(vg, txtbox->rect.x, txtbox->rect.y, txtA, txtB, (f32*)&bound);
					while (bound.w > txtbox->rect.w) {
						txtB--;
						nvgTextBounds(vg, txtbox->rect.x, txtbox->rect.y, txtA, txtB, (f32*)&bound);
					}
				}
			}
		}
	} else {
		while (bound.w > txtbox->rect.w) {
			txtB--;
			nvgTextBounds(vg, txtbox->rect.x, txtbox->rect.y, txtA, txtB, (f32*)&bound);
		}
		
		if (txtbox == sCurTextbox)
			sStoreA = txtA;
	}
	
	InputContext* inputCtx = info->geoCtx->input;
	s32 posA = (uPtr)txtA - (uPtr)buffer;
	s32 posB = (uPtr)txtB - (uPtr)buffer;
	
	if (GeoGrid_Cursor_InRect(split, &txtbox->rect) && inputCtx->mouse.clickL.hold) {
		if (inputCtx->mouse.clickL.press) {
			f32 dist = 400;
			for (char* tempB = txtA; tempB <= txtB; tempB++) {
				Vec2s glyphPos;
				f32 res;
				nvgTextBounds(vg, txtbox->rect.x, txtbox->rect.y, txtA, tempB, (f32*)&bound);
				glyphPos.x = bound.w + SPLIT_TEXT_PADDING - 2;
				glyphPos.y = bound.h - 2 + SPLIT_TEXT_SMALL * 0.5;
				
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
				nvgTextBounds(vg, txtbox->rect.x, txtbox->rect.y, txtA, tempB, (f32*)&bound);
				glyphPos.x = bound.w + SPLIT_TEXT_PADDING - 2;
				glyphPos.y = bound.h - 2 + SPLIT_TEXT_SMALL * 0.5;
				
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
	
	txtbox->rect.h = SPLIT_TEXT_MED * 1.5;
	
	nvgBeginPath(vg);
	if (txtbox->bgCl.r == 0 && txtbox->bgCl.g == 0 && txtbox->bgCl.b == 0) {
		txtbox->bgCl = Theme_GetColor(THEME_LGHT, 175);
	}
	if (txtbox != sCurTextbox) {
		Theme_SmoothStepToCol(&txtbox->bgCl, Theme_GetColor(THEME_LGHT, 175), 0.25, 0.15, 0.0);
	} else {
		Theme_SmoothStepToCol(&txtbox->bgCl, Theme_GetColor(THEME_SELC, 175), 0.25, 0.05, 0.0);
	}
	nvgFillColor(vg, txtbox->bgCl);
	nvgRoundedRect(
		vg,
		txtbox->rect.x - 1.0f,
		txtbox->rect.y - 1.0f,
		txtbox->rect.w + 1.0f * 2,
		txtbox->rect.h + 1.0f * 2,
		SPLIT_ROUND_R
	);
	nvgFill(vg);
	
	nvgBeginPath(vg);
	nvgFillColor(vg, Theme_GetColor(THEME_LINE, 200));
	nvgRoundedRect(vg, txtbox->rect.x, txtbox->rect.y, txtbox->rect.w, txtbox->rect.h, SPLIT_ROUND_R);
	nvgFill(vg);
	
	if (txtbox == sCurTextbox) {
		u32 swap = 0;
		
		if (buffer[sTextPos - 1] == ' ') {
			swap = true;
			buffer[sTextPos - 1] = '_';
		}
		
		nvgBeginPath(vg);
		nvgTextBounds(vg, txtbox->rect.x, txtbox->rect.y, txtA, &buffer[sTextPos], (f32*)&bound);
		nvgBeginPath(vg);
		nvgFillColor(vg, Theme_GetColor(THEME_SELC, 255));
		nvgRect(vg, bound.w + SPLIT_TEXT_PADDING - 2, bound.h - 2, 2, SPLIT_TEXT_SMALL);
		nvgFill(vg);
		
		if (sSelectPos != -1) {
			s32 min = fmin(sSelectPos, sTextPos);
			s32 max = fmax(sSelectPos, sTextPos);
			f32 x, xmax;
			
			if (txtA < &buffer[min]) {
				nvgTextBounds(vg, txtbox->rect.x, txtbox->rect.y, txtA, &buffer[min], (f32*)&bound);
				x = bound.w + SPLIT_TEXT_PADDING - 2;
			} else {
				x = txtbox->rect.x + SPLIT_TEXT_PADDING;
			}
			
			nvgTextBounds(vg, txtbox->rect.x, txtbox->rect.y, txtA, &buffer[max], (f32*)&bound);
			xmax = bound.w + SPLIT_TEXT_PADDING - 2 - x;
			
			nvgBeginPath(vg);
			nvgFillColor(vg, Theme_GetColor(THEME_SELC, 255));
			nvgRect(vg, x, bound.h - 2, CLAMP_MAX(xmax, txtbox->rect.w - SPLIT_TEXT_PADDING - 2), SPLIT_TEXT_SMALL);
			nvgFill(vg);
		}
		
		if (swap) {
			buffer[sTextPos - 1] = ' ';
		}
	} else {
		if (&buffer[strlen(buffer)] != txtB)
			for (s32 i = 0; i < 3; i++)
				txtB[-i] = '.';
	}
	nvgFillColor(vg, Theme_GetColor(THEME_TEXT, 255));
	nvgText(
		vg,
		txtbox->rect.x + SPLIT_TEXT_PADDING,
		txtbox->rect.y + txtbox->rect.h * 0.5,
		txtA,
		txtB + 1
	);
}

ElementFunc sFuncTable[] = {
	Element_Draw_Button,
	Element_Draw_Textbox
};

/* ───────────────────────────────────────────────────────────────────────── */

bool Element_Button(GeoGridContext* geoCtx, Split* split, ElButton* button) {
	u32 set = 0;
	
	button->hover = 0;
	button->state = 0;
	if (split->mouseInSplit && !split->blockMouse && GeoGrid_Cursor_InRect(split, &button->rect)) {
		if (geoCtx->input->mouse.clickL.press) {
			button->state++;
		}
		
		if (geoCtx->input->mouse.clickL.hold) {
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
	
	return button->state ? true : false;
}

void Element_Textbox(GeoGridContext* geoCtx, Split* split, ElTextbox* txtbox) {
	u32 set = 0;
	
	txtbox->hover = 0;
	if (split->mouseInSplit && !split->blockMouse && GeoGrid_Cursor_InRect(split, &txtbox->rect)) {
		txtbox->hover = 1;
		if (geoCtx->input->mouse.clickL.press) {
			sCurTextbox = txtbox;
			sCurSplitTextbox = split;
			sTextPos = strlen(txtbox->txt);
		}
	}
	
	Elements_QueueElement(
		geoCtx,
		split,
		ELEM_ID_TEXTBOX,
		txtbox
	);
}

/* ───────────────────────────────────────────────────────────────────────── */

void Elements_Init(GeoGridContext* geoCtx) {
	sCurrentElement = pElementStack;
}

void Elements_Update(GeoGridContext* geoCtx) {
	sCurrentElement = pElementStack;
	sElemNum = 0;
	static s32 timer = 0;
	
	if (sCurTextbox) {
		char* txt = geoCtx->input->buffer;
		s32 prevTextPos = sTextPos;
		s32 press = 0;
		
		if (geoCtx->input->mouse.click.press || geoCtx->input->key[KEY_ENTER].press) {
			sSelectPos = -1;
			if (!GeoGrid_Cursor_InRect(sCurSplitTextbox, &sCurTextbox->rect) || geoCtx->input->key[KEY_ENTER].press) {
				sCurTextbox = NULL;
				sCurTextbox = NULL;
				sStoreA = NULL;
				
				return;
			}
		}
		
		if (geoCtx->input->key[KEY_LEFT_CONTROL].hold) {
			if (geoCtx->input->key[KEY_A].press) {
				prevTextPos = strlen(sCurTextbox->txt);
				sTextPos = 0;
			}
			if (geoCtx->input->key[KEY_V].press) {
				txt = (char*)Input_GetClipboardStr();
			}
			if (geoCtx->input->key[KEY_C].press) {
				s32 max = fmax(sSelectPos, sTextPos);
				s32 min = fmin(sSelectPos, sTextPos);
				char* copy = Graph_Alloc(max - min + 1);
				
				memcpy(copy, &sCurTextbox->txt[min], max - min);
				Input_SetClipboardStr(copy);
			}
		} else {
			if (geoCtx->input->key[KEY_LEFT].press) {
				sTextPos--;
				press++;
			}
			
			if (geoCtx->input->key[KEY_RIGHT].press) {
				sTextPos++;
				press++;
			}
			
			if (geoCtx->input->key[KEY_HOME].press) {
				sTextPos = 0;
			}
			
			if (geoCtx->input->key[KEY_END].press) {
				sTextPos = strlen(sCurTextbox->txt);
			}
			
			if (geoCtx->input->key[KEY_LEFT].hold || geoCtx->input->key[KEY_RIGHT].hold) {
				timer++;
			} else {
				timer = 0;
			}
			
			if (timer >= 30 && timer % 2 == 0) {
				if (geoCtx->input->key[KEY_LEFT].hold) {
					sTextPos--;
				}
				
				if (geoCtx->input->key[KEY_RIGHT].hold) {
					sTextPos++;
				}
			}
		}
		
		sTextPos = CLAMP(sTextPos, 0, strlen(sCurTextbox->txt));
		
		if (sTextPos != prevTextPos) {
			if (geoCtx->input->key[KEY_LEFT_SHIFT].hold || geoCtx->input->key[KEY_LEFT_CONTROL].hold)
				sSelectPos = prevTextPos;
			else
				sSelectPos = -1;
		} else if (press) {
			sSelectPos = -1;
		}
		
		if (geoCtx->input->key[KEY_BACKSPACE].press) {
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

void Elements_Draw(GeoGridContext* geoCtx, Split* split) {
	for (s32 i = 0; i < sElemNum; i++) {
		if (pElementStack[i].split == split)
			sFuncTable[pElementStack[i].type](&pElementStack[i]);
	}
}