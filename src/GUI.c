#include "Editor.h"

/* / / / / / / / / / / / / / / / / / / / / / / / / / / */

char* Debug_Split_GetStateStrings(Split* split) {
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

SplitVtx* Split_AddVertex(GuiContext* guiCtx, s16 x, s16 y) {
	SplitVtx* head = guiCtx->vtxHead;
	
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
	Node_Add(SplitVtx, guiCtx->vtxHead, vtx);
	
	return vtx;
}

SplitEdge* Split_AddEdge(GuiContext* guiCtx, SplitVtx* v1, SplitVtx* v2) {
	SplitEdge* head = guiCtx->edgeHead;
	SplitEdge* edge = NULL;
	
	OsAssert(v1 != NULL && v2 != NULL);
	
	if ((intptr_t)v1 > (intptr_t)v2) {
		SplitVtx* temp = v1;
		v1 = v2;
		v2 = temp;
	}
	
	while (head) {
		if (head->vtx[0] == v1 && head->vtx[1] == v2) {
			OsPrintfEx("EdgeConnect");
			
			edge = head;
			break;
		}
		head = head->next;
	}
	
	if (edge == NULL) {
		edge = Lib_Calloc(0, sizeof(SplitEdge));
		
		edge->vtx[0] = v1;
		edge->vtx[1] = v2;
		Node_Add(SplitEdge, guiCtx->edgeHead, edge);
	}
	
	if (edge->vtx[0]->pos.y == edge->vtx[1]->pos.y) {
		edge->state |= EDGE_VERTICAL;
	} else {
		edge->state |= EDGE_HORIZONTAL;
	}
	
	return edge;
}

/* / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Split_InitRect(GuiContext* guiCtx, Split* split, Rect* rect) {
	split->vtx[VTX_BOT_L] = Split_AddVertex(
		guiCtx,
		rect->x,
		rect->y + rect->h
	);
	split->vtx[VTX_TOP_L] = Split_AddVertex(
		guiCtx,
		rect->x,
		rect->y
	);
	split->vtx[VTX_TOP_R] = Split_AddVertex(
		guiCtx,
		rect->x + rect->w,
		rect->y
	);
	split->vtx[VTX_BOT_R] = Split_AddVertex(
		guiCtx,
		rect->x + rect->w,
		rect->y + rect->h
	);
	
	Split_AddEdge(guiCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_TOP_L]);
	Split_AddEdge(guiCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
	Split_AddEdge(guiCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	Split_AddEdge(guiCtx, split->vtx[VTX_BOT_R], split->vtx[VTX_BOT_L]);
	
	OsPrintfEx("Init OK");
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
		{ split->mousePos.x, split->mousePos.y },
		{ split->mousePos.x, split->mousePos.y },
		{ split->mousePos.x, split->mousePos.y },
		{ split->mousePos.x, split->mousePos.y },
		{ 0,                 split->mousePos.y },
		{ 0,                 split->mousePos.y },
		{ split->mousePos.x, 0               },
		{ split->mousePos.x, 0               },
	};
	Vec2s pos[] = {
		{ 0,             0,           },
		{ split->rect.w, 0,           },
		{ 0,             split->rect.h, },
		{ split->rect.w, split->rect.h, },
		{ 0,             0,           },
		{ 0,             split->rect.h, },
		{ 0,             0,           },
		{ split->rect.w, 0,           },
	};
	s32 i;
	
	for (i = 0; (1 << i) <= SPLIT_SIDE_R; i++) {
		if (flag & (1 << i)) {
			break;
		}
	}
	
	return Vec_Vec2s_DistXZ(&mouse[i], &pos[i]);
}

s32 Split_Cursor_InSplit(Split* split) {
	s32 resX = (split->mousePos.x < split->rect.w && split->mousePos.x >= 0);
	s32 resY = (split->mousePos.y < split->rect.h && split->mousePos.y >= 0);
	
	return (resX && resY);
}

u32 Split_Cursor_InRangeOfStatePoint(Split* split, s32 range) {
	for (s32 i = 0; (1 << i) <= SPLIT_SIDE_R; i++) {
		if (Split_Cursor_DistTo((1 << i), split) <= range) {
			return (1 << i);
		}
	}
	
	return false;
}
/*
 * +─────────+─────────+
 * |         |         |
 * |    Y    Z    X    |
 * |         |         |
 * +─────────+─────────+
 * Get Edge [Z] using Split [Y] and Split [X]
 */
SplitEdge* Split_GetEdge_SharedBySplits(GuiContext* guiCtx, Split* a, Split* b) {
	SplitVtx* vtx[2] = { NULL };
	SplitVtx* temp;
	SplitEdge* edge = guiCtx->edgeHead;
	s32 k = 0;
	
	if (b == NULL) {
		return NULL;
	}
	OsAssert(guiCtx->edgeHead != NULL);
	
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
/*
 * +─────────+─────────+
 * |         |         |
 * |         |         |
 * |         |         |
 * +────Z────X────Y────+
 * Get Edge [Z] using Edge [Y] and Vtx [X]
 */
SplitEdge* Split_GetEdge_Sibling(GuiContext* guiCtx, SplitEdge* edge, SplitVtx* vtx) {
	SplitEdge* ret = guiCtx->edgeHead;
	
	while (ret) {
		if (ret == edge) {
			ret = ret->next;
			continue;
		}
		
		if ((ret->state & EDGE_ALIGN) & (edge->state & EDGE_ALIGN)) {
			if (ret->vtx[0] == vtx || ret->vtx[1] == vtx) {
				return ret;
			}
		}
		
		ret = ret->next;
	}
	
	return NULL;
}
/*
 * +─────────X─────────+
 * |         |         |
 * |         Z         |
 * |         |         |
 * +─────────Y─────────+
 * Get Edge [Z] using Vtx [X] and Vtx [Y]
 */
SplitEdge* Split_GetEdge_VtxVtx(GuiContext* guiCtx, SplitVtx* v1, SplitVtx* v2) {
	SplitEdge* edge = guiCtx->edgeHead;
	
	if ((intptr_t)v1 > (intptr_t)v2) {
		SplitVtx* temp = v1;
		v1 = v2;
		v2 = temp;
	}
	
	while (edge) {
		if (edge->vtx[0] == v1 && edge->vtx[1] == v2) {
			return edge;
		}
		
		edge = edge->next;
	}
	
	return NULL;
}

Split* Split_GetSplit_Neighbour(GuiContext* guiCtx, Split* split, SplitDir dir) {
	Split* ret = guiCtx->splitHead;
	SplitVtx* a;
	SplitVtx* b;
	SVtx compA;
	SVtx compB;
	
	OsAssert(dir >= DIR_T && dir <= DIR_R);
	
	switch (dir) {
	    case DIR_T:
		    a = split->vtx[VTX_TOP_L];
		    b = split->vtx[VTX_TOP_R];
		    compA = VTX_BOT_L;
		    compB = VTX_BOT_R;
		    break;
	    case DIR_B:
		    a = split->vtx[VTX_BOT_L];
		    b = split->vtx[VTX_BOT_R];
		    compA = VTX_TOP_L;
		    compB = VTX_TOP_R;
		    break;
	    case DIR_L:
		    a = split->vtx[VTX_TOP_L];
		    b = split->vtx[VTX_BOT_L];
		    compA = VTX_TOP_R;
		    compB = VTX_BOT_R;
		    break;
	    case DIR_R:
		    a = split->vtx[VTX_TOP_R];
		    b = split->vtx[VTX_BOT_R];
		    compA = VTX_TOP_L;
		    compB = VTX_BOT_L;
		    break;
	}
	
	while (ret) {
		if (ret->vtx[compA] == a && ret->vtx[compB] == b) {
			return ret;
		}
		
		ret = ret->next;
	}
	
	return NULL;
}

Split* Split_GetSplit_NeighbourByState(GuiContext* guiCtx, Split* split) {
	Split* nei = guiCtx->splitHead;
	
	OsAssert(split != NULL);
	
	if (split->stateFlag & SPLIT_SIDES) {
		for (s32 i = 1; i <= 4; i++) {
			if (split->stateFlag & (1 << (3 + i))) {
				return Split_GetSplit_Neighbour(guiCtx, split, i);
			}
		}
	} else if (split->stateFlag & SPLIT_SIDES) {
	}
	
	return NULL;
}

SplitVtx* Split_GetVtx_Neighbour(GuiContext* guiCtx, SplitVtx* vtx, SplitDir dir) {
	SplitEdge* edge = guiCtx->edgeHead;
	
	switch (dir) {
	    case DIR_T:
		    while (edge) {
			    if (edge->state & EDGE_VERTICAL) {
				    for (s32 i = 0; i <= 1; i++) {
					    s32 j = 1 - i;
					    if (edge->vtx[i] == vtx) {
						    if (edge->vtx[j]->pos.y < vtx->pos.y) {
							    return edge->vtx[j];
						    }
					    }
				    }
			    }
			    
			    edge = edge->next;
		    }
		    break;
	    case DIR_B:
		    if (edge->state & EDGE_VERTICAL) {
			    for (s32 i = 0; i <= 1; i++) {
				    s32 j = 1 - i;
				    if (edge->vtx[i] == vtx) {
					    if (edge->vtx[j]->pos.y > vtx->pos.y) {
						    return edge->vtx[j];
					    }
				    }
			    }
		    }
		    break;
	    case DIR_L:
		    if (edge->state & EDGE_HORIZONTAL) {
			    for (s32 i = 0; i <= 1; i++) {
				    s32 j = 1 - i;
				    if (edge->vtx[i] == vtx) {
					    if (edge->vtx[j]->pos.x < vtx->pos.x) {
						    return edge->vtx[j];
					    }
				    }
			    }
		    }
		    break;
	    case DIR_R:
		    if (edge->state & EDGE_HORIZONTAL) {
			    for (s32 i = 0; i <= 1; i++) {
				    s32 j = 1 - i;
				    if (edge->vtx[i] == vtx) {
					    if (edge->vtx[j]->pos.x > vtx->pos.x) {
						    return edge->vtx[j];
					    }
				    }
			    }
		    }
		    break;
	}
	
	return NULL;
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
		split->stateFlag |= Split_Cursor_InRangeOfStatePoint(split, 20);
		Split* nei = Split_GetSplit_NeighbourByState(guiCtx, split);
		guiCtx->resizeEdge = Split_GetEdge_SharedBySplits(guiCtx, split, nei);
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
		
		split->mouseInRegion = Split_Cursor_InSplit(split);
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
			if (Split_Cursor_InRangeOfStatePoint(split, 20) & SPLIT_POINTS) {
				editorCtx->inputCtx.mouse.cursorIcon = CURSOR_CROSSHAIR;
			} else if (Split_Cursor_InRangeOfStatePoint(split, 10) & SPLIT_SIDE_H) {
				editorCtx->inputCtx.mouse.cursorIcon = CURSOR_HRESIZE;
			} else if (Split_Cursor_InRangeOfStatePoint(split, 10) & SPLIT_SIDE_V) {
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
	SplitVtx* vtx = guiCtx->vtxHead;
	Rect* rect = &guiCtx->workRect;
	f64 x = (f64)guiCtx->workRect.w / (f64)guiCtx->prevWorkRect.w;
	f64 y = (f64)guiCtx->workRect.h / (f64)guiCtx->prevWorkRect.h;
	
	while (vtx) {
		vtx->pos.y -= guiCtx->bar[GUI_BAR_TOP].rect.h;
		vtx->pos.x *= x;
		vtx->pos.y *= y;
		vtx->pos.y += guiCtx->bar[GUI_BAR_TOP].rect.h;
		
		if (vtx->state & VTX_STICK_L) {
			vtx->pos.x = guiCtx->workRect.x;
		}
		if (vtx->state & VTX_STICK_R) {
			vtx->pos.x = guiCtx->workRect.x + guiCtx->workRect.w;
		}
		if (vtx->state & VTX_STICK_T) {
			vtx->pos.y = guiCtx->workRect.y;
		}
		if (vtx->state & VTX_STICK_B) {
			vtx->pos.y = guiCtx->workRect.y + guiCtx->workRect.h;
		}
		
		if (guiCtx->resizeEdge || __appInfo->isResizeCallback) {
			f64 clampXmin = 100.0f;
			f64 clampYmin = 100.0f;
			f64 clampXmax = guiCtx->workRect.x + guiCtx->workRect.w - 100.0f;
			f64 clampYmax = guiCtx->workRect.y + guiCtx->workRect.h - 100.0f;
			
			if (!(vtx->state & (VTX_STICK_L | VTX_STICK_R))) {
				vtx->pos.x = CLAMP(vtx->pos.x, clampXmin, clampXmax);
			}
			if (!(vtx->state & (VTX_STICK_T | VTX_STICK_B))) {
				vtx->pos.y = CLAMP(vtx->pos.y, clampYmin, clampYmax);
			}
		}
		
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
	
	guiCtx->splitHead->vtx[0]->state |= (VTX_STICK_L | VTX_STICK_B);
	guiCtx->splitHead->vtx[1]->state |= (VTX_STICK_L | VTX_STICK_T);
	guiCtx->splitHead->vtx[2]->state |= (VTX_STICK_T);
	guiCtx->splitHead->vtx[3]->state |= (VTX_STICK_B);
	
	guiCtx->splitHead->next->vtx[0]->state |= (VTX_STICK_B);
	guiCtx->splitHead->next->vtx[1]->state |= (VTX_STICK_T);
	guiCtx->splitHead->next->vtx[2]->state |= (VTX_STICK_R | VTX_STICK_T);
	guiCtx->splitHead->next->vtx[3]->state |= (VTX_STICK_R | VTX_STICK_B);
	
	guiCtx->prevWorkRect = guiCtx->workRect;
}

void Gui_Update(EditorContext* editorCtx) {
	GuiContext* guiCtx = &editorCtx->guiCtx;
	
	if (guiCtx->resizeEdge) {
		SplitVtx* a = guiCtx->resizeEdge->vtx[0];
		SplitVtx* b = guiCtx->resizeEdge->vtx[1];
		
		a->pos.x = b->pos.x = (s32)((f64)__inputCtx->mouse.pos.x * 0.25) * 4.0 + 2;
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
				static Split* lastActive;
				
				if (lastActive == NULL) {
					lastActive = guiCtx->splitHead;
				}
				
				if (guiCtx->actionSplit) {
					lastActive = guiCtx->actionSplit;
				}
				
				nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_BASE_WHITE));
				nvgFontSize(editorCtx->vg, 30 - 16);
				nvgFontFace(editorCtx->vg, "sans");
				nvgTextAlign(editorCtx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
				nvgText(
					editorCtx->vg,
					8,
					8,
					Debug_Split_GetStateStrings(lastActive),
					NULL
				);
			}
		} nvgEndFrame(editorCtx->vg);
	}
	
	Gui_Split_DrawAll(editorCtx);
}