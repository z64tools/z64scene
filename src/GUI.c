#include "Editor.h"

/* / / / / / / / / / / / / / / / / / / / / / / / / / / */

SplitVtx* Split_AddVertex(GuiContext* guiCtx, s16 x, s16 y) {
	SplitVtx* head = guiCtx->splitVtxHead;
	
	while (head) {
		if (head->pos.x == x && head->pos.y == y) {
			OsPrintfEx("VtxConnect %d %d", x, y);
			
			return head;
		}
		head = head->next;
	}
	
	SplitVtx* vtx = Lib_Calloc(0, sizeof(SplitVtx));
	
	vtx->pos.x = x;
	vtx->pos.y = y;
	Node_Add(SplitVtx, guiCtx->splitVtxHead, vtx);
	
	return vtx;
}

SplitEdge* Split_AddEdge(GuiContext* guiCtx, SplitVtx* v1, SplitVtx* v2) {
	SplitEdge* head = guiCtx->splitEdgeHead;
	
	if ((intptr_t)v1 > (intptr_t)v2) {
		SplitVtx* temp = v1;
		v1 = v2;
		v2 = temp;
	}
	
	while (head) {
		if (head->vtx[0] == v1 && head->vtx[1] == v2) {
			OsPrintfEx("EdgeConnect");
			
			return head;
		}
		head = head->next;
	}
	
	SplitEdge* edge = Lib_Calloc(0, sizeof(SplitEdge));
	
	edge->vtx[0] = v1;
	edge->vtx[1] = v2;
	Node_Add(SplitEdge, guiCtx->splitEdgeHead, edge);
	
	return edge;
}

/* / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Split_InitRect(GuiContext* guiCtx, Split* split, Rect* rect) {
	split->vtx[0] = Split_AddVertex(
		guiCtx,
		rect->x,
		rect->y + rect->h
	);
	split->vtx[1] = Split_AddVertex(
		guiCtx,
		rect->x,
		rect->y
	);
	split->vtx[2] = Split_AddVertex(
		guiCtx,
		rect->x + rect->w,
		rect->y
	);
	split->vtx[3] = Split_AddVertex(
		guiCtx,
		rect->x + rect->w,
		rect->y + rect->h
	);
	
	Split_AddEdge(guiCtx, split->vtx[0], split->vtx[1]);
	Split_AddEdge(guiCtx, split->vtx[1], split->vtx[2]);
	Split_AddEdge(guiCtx, split->vtx[2], split->vtx[3]);
	Split_AddEdge(guiCtx, split->vtx[3], split->vtx[0]);
}

void Split_SetRect(Split* split) {
	split->rect = (Rect) {
		split->vtx[1]->pos.x,
		split->vtx[1]->pos.y,
		
		split->vtx[3]->pos.x - split->vtx[1]->pos.x,
		split->vtx[3]->pos.y - split->vtx[1]->pos.y
	};
}

/* / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Split_Draw_RegionBorders(NVGcontext* vg, Rect* rect) {
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
		0 + SPLIT_SPLIT_W,
		0 + SPLIT_SPLIT_W,
		rect->w - SPLIT_SPLIT_W * 2,
		rect->h - SPLIT_SPLIT_W * 2,
		SPLIT_ROUND_R
	);
	nvgPathWinding(vg, NVG_HOLE);
	nvgFillColor(vg, Theme_GetColor(THEME_SPLITTER));
	nvgFill(vg);
}

/* / / / / / / / / / / / / / / / / / / / / / / / / / / */

s32 Split_Cursor_DistTo(SplitPointIndex flag, Split* split) {
	Vec2s mouse[] = {
		{ split->mousePos.x, split->mousePos.y }, /* SPLIT_POINT_TL */
		{ split->mousePos.x, split->mousePos.y }, /* SPLIT_POINT_TR */
		{ split->mousePos.x, split->mousePos.y }, /* SPLIT_POINT_BL */
		{ split->mousePos.x, split->mousePos.y }, /* SPLIT_POINT_BR */
		{ 0,                 split->mousePos.y }, /* SPLIT_SIDE_T */
		{ 0,                 split->mousePos.y }, /* SPLIT_SIDE_B */
		{ split->mousePos.x, 0               }, /* SPLIT_SIDE_L */
		{ split->mousePos.x, 0               }, /* SPLIT_SIDE_R */
	};
	Vec2s pos[] = {
		{ 0,             0,           }, /* SPLIT_POINT_TL */
		{ split->rect.w, 0,           }, /* SPLIT_POINT_TR */
		{ 0,             split->rect.h, }, /* SPLIT_POINT_BL */
		{ split->rect.w, split->rect.h, }, /* SPLIT_POINT_BR */
		{ 0,             0,           }, /* SPLIT_SIDE_T */
		{ 0,             split->rect.h, }, /* SPLIT_SIDE_B */
		{ 0,             0,           }, /* SPLIT_SIDE_L */
		{ split->rect.w, 0,           }, /* SPLIT_SIDE_R */
	};
	s32 i;
	
	for (i = 0; (1 << i) <= SPLIT_SIDE_R; i++) {
		if (flag & (1 << i)) {
			break;
		}
	}
	
	return Vec_Vec2s_DistXZ(&mouse[i], &pos[i]);
}

s32 Split_Cursor_InRegion(Split* region) {
	s32 resX = (region->mousePos.x < region->rect.w && region->mousePos.x >= 0);
	s32 resY = (region->mousePos.y < region->rect.h && region->mousePos.y >= 0);
	
	return (resX && resY);
}

u32 Split_Cursor_GetStateInRange(Split* split, s32 range) {
	for (s32 i = 0; (1 << i) <= SPLIT_SIDE_R; i++) {
		if (Split_Cursor_DistTo((1 << i), split) <= range) {
			return (1 << i);
		}
	}
	
	return false;
}

SplitEdge* Split_GetSharedEdge(GuiContext* guiCtx, Split* a, Split* b) {
	SplitVtx* vtx[2] = { NULL };
	SplitVtx* temp;
	SplitEdge* edge = guiCtx->splitEdgeHead;
	s32 k = 0;
	
	OsAssert(a != NULL && b != NULL);
	OsAssert(guiCtx->splitEdgeHead != NULL);
	
	// Find Vertesies
	for (s32 i = 0; i < 4; i++) {
		for (s32 j = 0; j < 4; j++) {
			if (a->vtx[i] == b->vtx[j]) {
				vtx[k++] = a->vtx[i];
				OsPrintfEx("%08X", a->vtx[i]);
				#ifndef NDEBUG
				if (k > 2)
					OsPrintfEx("\ak > 2");
				#endif
			}
		}
	}
	
	OsAssert(vtx[0] != NULL && vtx[1] != NULL);
	
	if ((intptr_t)vtx[0] > (intptr_t)vtx[1]) {
		SplitVtx* temp = vtx[0];
		vtx[0] = vtx[1];
		vtx[1] = temp;
	}
	
	while (edge) {
		OsPrintfEx("%08X : %08X - %08X", edge, edge->vtx[0], edge->vtx[1]);
		if (edge->vtx[0] == vtx[0] && edge->vtx[1] == vtx[1]) {
			return edge;
		}
		
		edge = edge->next;
	}
	
	return NULL;
}

Split* Split_GetNeighbourByState(GuiContext* guiCtx, Split* split) {
	Split* nei = guiCtx->splitHead;
	
	OsAssert(split != NULL);
	
	if (split->stateFlag & SPLIT_STATE_DRAG_R) {
		while (nei) {
			if (nei->vtx[1] == split->vtx[2] && nei->vtx[0] == split->vtx[3]) {
				return nei;
			}
			nei = nei->next;
		}
		
		return NULL;
	}
	
	return NULL;
}

/* / / / / / / / / / / / / / / / / / / / / / / / / / / */

char* Split_Debug_GetStates(Split* split) {
	static char buffer[1024];
	s32 t = 0;
	char* states[] = {
		"NONE",
		"DRAG_TL",
		"DRAG_TR",
		"DRAG_BL",
		"DRAG_BR",
		"DRAG_T",
		"DRAG_B",
		"DRAG_L",
		"DRAG_R",
		"SPLIT_T",
		"SPLIT_B",
		"SPLIT_L",
		"SPLIT_R",
	};
	
	String_Copy(buffer, states[0]);
	
	for (s32 i = 0; (1 << i) <= 0xFFFF; i++) {
		if (split->stateFlag & (1 << i)) {
			if (t++ == 0) {
				String_Copy(buffer, states[i + 1]);
			} else {
				String_Merge(buffer, "|");
				String_Merge(buffer, states[i + 1]);
			}
		}
	}
	
	return buffer;
}

/* / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Split_Action_Reset(GuiContext* guiCtx) {
	if (guiCtx->actionSplit == NULL)
		return;
	guiCtx->actionSplit->stateFlag &= ~(
		SPLIT_STATE_DRAG_CORNER | SPLIT_STATE_DRAG_SIDE
	);
	
	guiCtx->actionSplit = NULL;
	guiCtx->resizeEdge = NULL;
}

void Split_Action_Update(GuiContext* guiCtx) {
	Split* split = guiCtx->actionSplit;
	SplitState splitState = (
		SPLIT_STATE_DRAG_TL |
		SPLIT_STATE_DRAG_TR |
		SPLIT_STATE_DRAG_BL |
		SPLIT_STATE_DRAG_BR
	);
	
	if (split->mousePress) {
		split->stateFlag |= Split_Cursor_GetStateInRange(split, 20);
		Split* nei = Split_GetNeighbourByState(guiCtx, split);
		guiCtx->resizeEdge = Split_GetSharedEdge(guiCtx, split, nei);
	}
}

/* / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Gui_Split_UpdateAll(EditorContext* editorCtx) {
	GuiContext* guiCtx = &editorCtx->guiCtx;
	Split* split = guiCtx->splitHead;
	Split* killReg;
	MouseInput* mouse = &editorCtx->inputCtx.mouse;
	Vec2s* winDim = &editorCtx->appInfo.winDim;
	
	// guiCtx->splitHead->rect = (Rect) {
	// 	guiCtx->workRect.x,
	// 	guiCtx->workRect.y,
	// 	guiCtx->workRect.w,
	// 	guiCtx->workRect.h
	// };
	
	if (guiCtx->actionSplit && mouse->cursorAction == false) {
		Split_Action_Reset(guiCtx);
	}
	
	while (split) {
		Split_SetRect(split);
		Vec2s rectPos = {
			split->rect.x,
			split->rect.y
		};
		Vec_Vec2s_Substract(
			&split->mousePos,
			&mouse->pos,
			(Vec2s*)&rectPos
		);
		
		split->mouseInRegion = Split_Cursor_InRegion(split);
		split->center.x = split->rect.x + split->rect.w * 0.5f;
		split->center.y = split->rect.y + split->rect.h * 0.5f;
		
		if (guiCtx->actionSplit == NULL &&
		    split->mouseInRegion &&
		    mouse->cursorAction) {
			if (mouse->click.press) {
				split->mousePressPos = split->mousePos;
				split->mousePress = true;
			}
			guiCtx->actionSplit = split;
		}
		
		if (guiCtx->actionSplit) {
			Split_Action_Update(guiCtx);
		} else {
			if (Split_Cursor_GetStateInRange(split, 20) & SPLIT_POINTS) {
				editorCtx->inputCtx.mouse.cursorIcon = CURSOR_CROSSHAIR;
			} else if (Split_Cursor_GetStateInRange(split, 10) & SPLIT_SIDE_H) {
				editorCtx->inputCtx.mouse.cursorIcon = CURSOR_HRESIZE;
			} else if (Split_Cursor_GetStateInRange(split, 10) & SPLIT_SIDE_V) {
				editorCtx->inputCtx.mouse.cursorIcon = CURSOR_VRESIZE;
			}
		}
		
		if (split->update) {
			if (split != guiCtx->splitHead &&
			    ((guiCtx->actionSplit && guiCtx->actionSplit == split) ||
			    guiCtx->actionSplit == NULL)) {
				split->update(editorCtx, split);
			}
		}
		
		split->mousePress = false;
		split = split->next;
	}
}

void Gui_Split_DrawAll(EditorContext* editorCtx) {
	GuiContext* guiCtx = &editorCtx->guiCtx;
	Split* split = guiCtx->splitHead;
	MouseInput* mouse = &editorCtx->inputCtx.mouse;
	Vec2s* winDim = &editorCtx->appInfo.winDim;
	
	// Draw All Regions Except NodeHead
	while (split != NULL) {
		glViewport(
			split->rect.x,
			winDim->y - split->rect.y - split->rect.h,
			split->rect.w,
			split->rect.h
		);
		nvgBeginFrame(editorCtx->vg, split->rect.w, split->rect.h, 1.0f); {
			if (split->draw)
				split->draw(editorCtx, split);
			
			Split_Draw_RegionBorders(editorCtx->vg, &split->rect);
		} nvgEndFrame(editorCtx->vg);
		
		split = split->next;
	}
}

void Gui_SplitVtx_UpdateAll(GuiContext* guiCtx) {
	SplitVtx* vtx = guiCtx->splitVtxHead;
	Rect* rect = &guiCtx->workRect;
	f64 x = (f64)guiCtx->workRect.w / (f64)guiCtx->prevWorkRect.w;
	f64 y = (f64)guiCtx->workRect.h / (f64)guiCtx->prevWorkRect.h;
	
	while (vtx) {
		vtx->pos.y -= guiCtx->bar[GUI_BAR_TOP].rect.h;
		vtx->pos.x *= x;
		vtx->pos.y *= y;
		vtx->pos.y += guiCtx->bar[GUI_BAR_TOP].rect.h;
		vtx = vtx->next;
	}
}

/* / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Gui_UpdateWorkRegion(EditorContext* editorCtx) {
	GuiContext* guiCtx = &editorCtx->guiCtx;
	Vec2s* winDim = &editorCtx->appInfo.winDim;
	
	guiCtx->workRect = (Rect) {
		0,
		0 + guiCtx->bar[GUI_BAR_TOP].rect.h,
		winDim->x,
		winDim->y - guiCtx->bar[GUI_BAR_BOT].rect.h - guiCtx->bar[GUI_BAR_TOP].rect.h
	};
}

void Gui_SetTopBarHeight(EditorContext* editorCtx, s32 h) {
	GuiContext* guiCtx = &editorCtx->guiCtx;
	Vec2s* winDim = &editorCtx->appInfo.winDim;
	
	guiCtx->bar[GUI_BAR_TOP].rect.x = 0;
	guiCtx->bar[GUI_BAR_TOP].rect.y = 0;
	guiCtx->bar[GUI_BAR_TOP].rect.w = winDim->x;
	guiCtx->bar[GUI_BAR_TOP].rect.h = h;
	Gui_UpdateWorkRegion(editorCtx);
}

void Gui_SetBotBarHeight(EditorContext* editorCtx, s32 h) {
	GuiContext* guiCtx = &editorCtx->guiCtx;
	Vec2s* winDim = &editorCtx->appInfo.winDim;
	
	guiCtx->bar[GUI_BAR_BOT].rect.x = 0;
	guiCtx->bar[GUI_BAR_BOT].rect.y = winDim->y - h;
	guiCtx->bar[GUI_BAR_BOT].rect.w = winDim->x;
	guiCtx->bar[GUI_BAR_BOT].rect.h = h;
	Gui_UpdateWorkRegion(editorCtx);
}

/* / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Gui_Init(EditorContext* editorCtx) {
	GuiContext* guiCtx = &editorCtx->guiCtx;
	Vec2s* winDim = &editorCtx->appInfo.winDim;
	
	guiCtx->splitHead = Lib_Calloc(0, sizeof(Split));
	guiCtx->splitHead->next = Lib_Calloc(0, sizeof(Split));
	Gui_SetTopBarHeight(editorCtx, 30);
	Gui_SetBotBarHeight(editorCtx, 30);
	
	Rect lHalf = {
		guiCtx->workRect.x, guiCtx->workRect.y,
		guiCtx->workRect.w / 2, guiCtx->workRect.h
	};
	Rect rHalf = {
		guiCtx->workRect.x + guiCtx->workRect.w / 2, guiCtx->workRect.y,
		guiCtx->workRect.w / 2, guiCtx->workRect.h
	};
	
	Split_InitRect(guiCtx, guiCtx->splitHead, &lHalf);
	Split_InitRect(guiCtx, guiCtx->splitHead->next, &rHalf);
	
	guiCtx->prevWorkRect = guiCtx->workRect;
}

void Gui_Update(EditorContext* editorCtx) {
	GuiContext* guiCtx = &editorCtx->guiCtx;
	
	if (guiCtx->resizeEdge) {
		SplitVtx* a = guiCtx->resizeEdge->vtx[0];
		SplitVtx* b = guiCtx->resizeEdge->vtx[1];
		
		a->pos.x = b->pos.x = __inputCtx->mouse.pos.x;
	}
	
	Gui_SetTopBarHeight(editorCtx, guiCtx->bar[GUI_BAR_TOP].rect.h);
	Gui_SetBotBarHeight(editorCtx, guiCtx->bar[GUI_BAR_BOT].rect.h);
	Gui_SplitVtx_UpdateAll(guiCtx);
	Gui_Split_UpdateAll(editorCtx);
	
	guiCtx->prevWorkRect = guiCtx->workRect;
}

void Gui_Draw(EditorContext* editorCtx) {
	GuiContext* guiCtx = &editorCtx->guiCtx;
	Vec2s* winDim = &editorCtx->appInfo.winDim;
	
	// Draw Bars
	for (s32 i = 0; i < 2; i++) {
		glViewport(
			guiCtx->bar[i].rect.x,
			winDim->y - guiCtx->bar[i].rect.y - guiCtx->bar[i].rect.h,
			guiCtx->bar[i].rect.w,
			guiCtx->bar[i].rect.h
		);
		nvgBeginFrame(editorCtx->vg, guiCtx->bar[i].rect.w, guiCtx->bar[i].rect.h, 1.0f); {
			nvgBeginPath(editorCtx->vg);
			nvgRect(
				editorCtx->vg,
				0,
				0,
				guiCtx->bar[i].rect.w,
				guiCtx->bar[i].rect.h
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
					Split_Debug_GetStates(guiCtx->splitHead),
					NULL
				);
			}
		} nvgEndFrame(editorCtx->vg);
	}
	
	Gui_Split_DrawAll(editorCtx);
}