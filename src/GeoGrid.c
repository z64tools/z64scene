#include "Editor.h"

void GeoGrid_RemoveDublicates(GeoGridContext* geoCtx);
void GeoGrid_Update_SplitRect(Split* split);

/* ───────────────────────────────────────────────────────────────────────── */

SplitDir GeoGrid_GetDir_Opposite(SplitDir dir) {
	return Wrap(dir + 2, DIR_L, DIR_B);
}

SplitDir GeoGrid_GerDir_MouseToPressPos(Split* split) {
	Vec2s pos;
	
	Vec2_Substract(pos, split->mousePos, split->mousePressPos);
	
	if (ABS(pos.x) > ABS(pos.y)) {
		if (pos.x < 0) {
			return DIR_L;
		}
		
		return DIR_R;
	} else {
		if (pos.y < 0) {
			return DIR_T;
		}
		
		return DIR_B;
	}
}

SplitVtx* GeoGrid_AddVtx(GeoGridContext* geoCtx, f64 x, f64 y) {
	SplitVtx* head = geoCtx->vtxHead;
	
	while (head) {
		if (fabs(head->pos.x - x) < 0.25 && fabs(head->pos.y - y) < 0.25) {
			OsPrintfEx("VtxConnect %.2f %.2f", x, y);
			
			return head;
		}
		head = head->next;
	}
	
	SplitVtx* vtx = Lib_Calloc(0, sizeof(SplitVtx));
	
	vtx->pos.x = x;
	vtx->pos.y = y;
	Node_Add(geoCtx->vtxHead, vtx);
	
	return vtx;
}

SplitEdge* GeoGrid_AddEdge(GeoGridContext* geoCtx, SplitVtx* v1, SplitVtx* v2) {
	SplitEdge* head = geoCtx->edgeHead;
	SplitEdge* edge = NULL;
	
	OsAssert(v1 != NULL && v2 != NULL);
	
	if (v1->pos.y == v2->pos.y) {
		if (v1->pos.x > v2->pos.x) {
			Swap(v1, v2);
		}
	} else {
		if (v1->pos.y > v2->pos.y) {
			Swap(v1, v2);
		}
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
		Node_Add(geoCtx->edgeHead, edge);
	}
	
	if (edge->vtx[0]->pos.y == edge->vtx[1]->pos.y) {
		edge->state |= EDGE_HORIZONTAL;
		edge->pos = edge->vtx[0]->pos.y;
		if (edge->pos < geoCtx->workRect.y + 1) {
			edge->state |= EDGE_STICK_T;
		}
		if (edge->pos > geoCtx->workRect.y + geoCtx->workRect.h - 1) {
			edge->state |= EDGE_STICK_B;
		}
	} else {
		edge->state |= EDGE_VERTICAL;
		edge->pos = edge->vtx[0]->pos.x;
		if (edge->pos < geoCtx->workRect.x + 1) {
			edge->state |= EDGE_STICK_L;
		}
		if (edge->pos > geoCtx->workRect.x + geoCtx->workRect.w - 1) {
			edge->state |= EDGE_STICK_R;
		}
	}
	
	return edge;
}

s32 GeoGrid_Cursor_GetDistTo(SplitState flag, Split* split) {
	Vec2s mouse[] = {
		{ split->mousePos.x, split->mousePos.y },
		{ split->mousePos.x, split->mousePos.y },
		{ split->mousePos.x, split->mousePos.y },
		{ split->mousePos.x, split->mousePos.y },
		{ split->mousePos.x, 0                 },
		{ 0,                 split->mousePos.y },
		{ split->mousePos.x, 0                 },
		{ 0,                 split->mousePos.y }
	};
	Vec2s pos[] = {
		{ 0,             split->rect.h,},
		{ 0,             0,            },
		{ split->rect.w, 0,            },
		{ split->rect.w, split->rect.h,},
		{ 0,             0,            },
		{ 0,             0,            },
		{ split->rect.w, 0,            },
		{ 0,             split->rect.h,},
	};
	s32 i;
	
	for (i = 0; (1 << i) <= SPLIT_SIDE_B; i++) {
		if (flag & (1 << i)) {
			break;
		}
	}
	
	return Vec_Vec2s_DistXZ(&mouse[i], &pos[i]);
}

SplitState GeoGrid_GetState_CursorPos(Split* split, s32 range) {
	for (s32 i = 0; (1 << i) <= SPLIT_SIDE_B; i++) {
		if (GeoGrid_Cursor_GetDistTo((1 << i), split) <= range) {
			return (1 << i);
		}
	}
	
	return SPLIT_POINT_NONE;
}

bool GeoGrid_Cursor_InRect(Split* split, Rect* rect) {
	s32 resX = (split->mousePos.x < rect->x + rect->w && split->mousePos.x >= rect->x);
	s32 resY = (split->mousePos.y < rect->y + rect->h && split->mousePos.y >= rect->y);
	
	return (resX && resY);
}

bool GeoGrid_Cursor_InSplit(Split* split) {
	s32 resX = (split->mousePos.x < split->rect.w && split->mousePos.x >= 0);
	s32 resY = (split->mousePos.y < split->rect.h && split->mousePos.y >= 0);
	
	return (resX && resY);
}

Split* GeoGrid_AddSplit(GeoGridContext* geoCtx, Rectf32* rect) {
	Split* split = Lib_Calloc(0, sizeof(Split));
	
	split->vtx[VTX_BOT_L] = GeoGrid_AddVtx(geoCtx, rect->x, rect->y + rect->h);
	split->vtx[VTX_TOP_L] = GeoGrid_AddVtx(geoCtx, rect->x, rect->y);
	split->vtx[VTX_TOP_R] = GeoGrid_AddVtx(geoCtx, rect->x + rect->w, rect->y);
	split->vtx[VTX_BOT_R] = GeoGrid_AddVtx(geoCtx, rect->x + rect->w, rect->y + rect->h);
	
	split->edge[EDGE_L] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_TOP_L]);
	split->edge[EDGE_T] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
	split->edge[EDGE_R] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	split->edge[EDGE_B] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_BOT_R], split->vtx[VTX_BOT_L]);
	
	Node_Add(geoCtx->splitHead, split);
	
	return split;
}

void GeoGrid_Edge_SetSlideClamp(GeoGridContext* geoCtx) {
	SplitEdge* tempEdge = geoCtx->edgeHead;
	SplitEdge* setEdge = geoCtx->actionEdge;
	SplitEdge* actionEdge = geoCtx->actionEdge;
	SplitVtx* vtxList[128] = { NULL };
	u32 vtxListCount = 0;
	u32 align = ((actionEdge->state & EDGE_VERTICAL) != 0);
	f64 posMin = actionEdge->vtx[0]->pos.s[align];
	f64 posMax = actionEdge->vtx[1]->pos.s[align];
	
	setEdge->state |= EDGE_EDIT;
	
	// Get edge with vtx closest to TOPLEFT
	while (tempEdge) {
		if ((tempEdge->state & EDGE_ALIGN) == (setEdge->state & EDGE_ALIGN)) {
			if (tempEdge->vtx[1] == setEdge->vtx[0]) {
				setEdge = tempEdge;
				tempEdge->state |= EDGE_EDIT;
				tempEdge = geoCtx->edgeHead;
				posMin = setEdge->vtx[0]->pos.s[align];
				continue;
			}
		}
		
		tempEdge = tempEdge->next;
	}
	
	tempEdge = geoCtx->edgeHead;
	
	// Set all below setEdgeA
	while (tempEdge) {
		if ((tempEdge->state & EDGE_ALIGN) == (setEdge->state & EDGE_ALIGN)) {
			if (tempEdge->vtx[0] == setEdge->vtx[1]) {
				tempEdge->state |= EDGE_EDIT;
				setEdge = tempEdge;
				tempEdge = geoCtx->edgeHead;
				posMax = setEdge->vtx[1]->pos.s[align];
				continue;
			}
		}
		
		tempEdge = tempEdge->next;
	}
	
	tempEdge = geoCtx->edgeHead;
	
	while (tempEdge) {
		if ((tempEdge->state & EDGE_ALIGN) == (actionEdge->state & EDGE_ALIGN)) {
			if (tempEdge->pos == actionEdge->pos) {
				if (tempEdge->vtx[1]->pos.s[align] <= posMax && tempEdge->vtx[0]->pos.s[align] >= posMin) {
					tempEdge->state |= EDGE_EDIT;
				}
			}
		}
		tempEdge = tempEdge->next;
	}
	
	OsPrintfEx("Align %d Min %.2f Max %.2f", align, posMin, posMax);
	
	if (geoCtx->actionEdge->state & EDGE_VERTICAL) {
		geoCtx->slide.clampMin = geoCtx->workRect.x;
		geoCtx->slide.clampMax = geoCtx->workRect.x + geoCtx->workRect.w;
	} else {
		geoCtx->slide.clampMin = geoCtx->workRect.y;
		geoCtx->slide.clampMax = geoCtx->workRect.y + geoCtx->workRect.h;
	}
}

void GeoGrid_Reset(GeoGridContext* geoCtx) {
	OsAssert(geoCtx->actionSplit);
	geoCtx->actionSplit->stateFlag &= ~(SPLIT_POINTS | SPLIT_SIDES);
	
	geoCtx->actionSplit = NULL;
}

void GeoGrid_Split(GeoGridContext* geoCtx, Split* split, SplitDir dir) {
	Split* partner;
	Split* newSplit;
	SplitDir mirDir = GeoGrid_GetDir_Opposite(dir);
	f64 splitPos = (dir == DIR_L || dir == DIR_R) ? geoCtx->mouse->pos.x : geoCtx->mouse->pos.y;
	
	newSplit = Lib_Calloc(0, sizeof(Split));
	
	Node_Add(geoCtx->splitHead, newSplit);
	
	OsPrintfEx("SplitDir: %d", dir);
	
	if (dir == DIR_L) {
		newSplit->vtx[0] = GeoGrid_AddVtx(geoCtx, splitPos, split->vtx[0]->pos.y);
		newSplit->vtx[1] = GeoGrid_AddVtx(geoCtx, splitPos, split->vtx[1]->pos.y);
		newSplit->vtx[2] =  GeoGrid_AddVtx(geoCtx, split->vtx[2]->pos.x, split->vtx[2]->pos.y);
		newSplit->vtx[3] = GeoGrid_AddVtx(geoCtx, split->vtx[3]->pos.x, split->vtx[3]->pos.y);
		split->vtx[2] = GeoGrid_AddVtx(geoCtx, splitPos, split->vtx[2]->pos.y);
		split->vtx[3] = GeoGrid_AddVtx(geoCtx, splitPos, split->vtx[3]->pos.y);
	}
	
	if (dir == DIR_R) {
		newSplit->vtx[0] = GeoGrid_AddVtx(geoCtx, split->vtx[0]->pos.x, split->vtx[0]->pos.y);
		newSplit->vtx[1] = GeoGrid_AddVtx(geoCtx, split->vtx[1]->pos.x, split->vtx[1]->pos.y);
		newSplit->vtx[2] = GeoGrid_AddVtx(geoCtx, splitPos, split->vtx[2]->pos.y);
		newSplit->vtx[3] = GeoGrid_AddVtx(geoCtx, splitPos, split->vtx[3]->pos.y);
		split->vtx[0] = GeoGrid_AddVtx(geoCtx, splitPos, split->vtx[0]->pos.y);
		split->vtx[1] = GeoGrid_AddVtx(geoCtx, splitPos, split->vtx[1]->pos.y);
	}
	
	if (dir == DIR_T) {
		newSplit->vtx[0] = GeoGrid_AddVtx(geoCtx, split->vtx[0]->pos.x, split->vtx[0]->pos.y);
		newSplit->vtx[1] = GeoGrid_AddVtx(geoCtx, split->vtx[1]->pos.x, splitPos);
		newSplit->vtx[2] = GeoGrid_AddVtx(geoCtx, split->vtx[2]->pos.x, splitPos);
		newSplit->vtx[3] = GeoGrid_AddVtx(geoCtx, split->vtx[3]->pos.x, split->vtx[3]->pos.y);
		split->vtx[3] = GeoGrid_AddVtx(geoCtx, split->vtx[3]->pos.x, splitPos);
		split->vtx[0] = GeoGrid_AddVtx(geoCtx, split->vtx[0]->pos.x, splitPos);
	}
	
	if (dir == DIR_B) {
		newSplit->vtx[0] = GeoGrid_AddVtx(geoCtx, split->vtx[0]->pos.x, splitPos);
		newSplit->vtx[1] = GeoGrid_AddVtx(geoCtx, split->vtx[1]->pos.x, split->vtx[1]->pos.y);
		newSplit->vtx[2] = GeoGrid_AddVtx(geoCtx, split->vtx[2]->pos.x, split->vtx[2]->pos.y);
		newSplit->vtx[3] = GeoGrid_AddVtx(geoCtx, split->vtx[3]->pos.x, splitPos);
		split->vtx[1] = GeoGrid_AddVtx(geoCtx, split->vtx[1]->pos.x, splitPos);
		split->vtx[2] = GeoGrid_AddVtx(geoCtx, split->vtx[2]->pos.x, splitPos);
	}
	
	split->edge[EDGE_L] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_TOP_L]);
	split->edge[EDGE_T] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
	split->edge[EDGE_R] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	split->edge[EDGE_B] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_BOT_R], split->vtx[VTX_BOT_L]);
	
	newSplit->edge[EDGE_L] = GeoGrid_AddEdge(geoCtx, newSplit->vtx[VTX_BOT_L], newSplit->vtx[VTX_TOP_L]);
	newSplit->edge[EDGE_T] = GeoGrid_AddEdge(geoCtx, newSplit->vtx[VTX_TOP_L], newSplit->vtx[VTX_TOP_R]);
	newSplit->edge[EDGE_R] = GeoGrid_AddEdge(geoCtx, newSplit->vtx[VTX_TOP_R], newSplit->vtx[VTX_BOT_R]);
	newSplit->edge[EDGE_B] = GeoGrid_AddEdge(geoCtx, newSplit->vtx[VTX_BOT_R], newSplit->vtx[VTX_BOT_L]);
	
	geoCtx->actionEdge = newSplit->edge[dir];
	GeoGrid_RemoveDublicates(geoCtx);
	GeoGrid_Edge_SetSlideClamp(geoCtx);
	GeoGrid_Update_SplitRect(split);
	GeoGrid_Update_SplitRect(newSplit);
}

void GeoGrid_KillSplit(GeoGridContext* geoCtx, Split* split, SplitDir dir) {
	SplitEdge* sharedEdge = split->edge[dir];
	Split* killSplit = geoCtx->splitHead;
	SplitDir oppositeDir = GeoGrid_GetDir_Opposite(dir);
	
	while (killSplit) {
		if (killSplit->edge[oppositeDir] == sharedEdge) {
			break;
		}
		
		killSplit = killSplit->next;
	}
	
	if (killSplit == NULL) {
		return;
	}
	
	split->edge[dir]->vtx[0]->killFlag = true;
	split->edge[dir]->vtx[1]->killFlag = true;
	
	if (dir == DIR_L) {
		split->vtx[VTX_BOT_L] = killSplit->vtx[VTX_BOT_L];
		split->vtx[VTX_TOP_L] = killSplit->vtx[VTX_TOP_L];
		split->edge[EDGE_T] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
		split->edge[EDGE_B] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_BOT_R]);
	}
	
	if (dir == DIR_T) {
		split->vtx[VTX_TOP_L] = killSplit->vtx[VTX_TOP_L];
		split->vtx[VTX_TOP_R] = killSplit->vtx[VTX_TOP_R];
		split->edge[EDGE_L] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_BOT_L]);
		split->edge[EDGE_R] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	}
	
	if (dir == DIR_R) {
		split->vtx[VTX_BOT_R] = killSplit->vtx[VTX_BOT_R];
		split->vtx[VTX_TOP_R] = killSplit->vtx[VTX_TOP_R];
		split->edge[EDGE_T] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
		split->edge[EDGE_B] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_BOT_R]);
	}
	
	if (dir == DIR_B) {
		split->vtx[VTX_BOT_L] = killSplit->vtx[VTX_BOT_L];
		split->vtx[VTX_BOT_R] = killSplit->vtx[VTX_BOT_R];
		split->edge[EDGE_L] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_BOT_L]);
		split->edge[EDGE_R] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	}
	
	split->edge[dir] = killSplit->edge[dir];
	
	if (killSplit->id > 0) {
		geoCtx->taskTable[killSplit->id].destroy(geoCtx->passArg, killSplit->instance, killSplit);
		free(killSplit->instance);
	}
	Node_Kill(geoCtx->splitHead, killSplit);
	GeoGrid_RemoveDublicates(geoCtx);
	GeoGrid_Update_SplitRect(split);
}

/* ───────────────────────────────────────────────────────────────────────── */

void GeoGrid_Update_Vtx_RemoveDublicates(GeoGridContext* geoCtx, SplitVtx* vtx) {
	SplitVtx* vtx2 = geoCtx->vtxHead;
	
	while (vtx2) {
		if (vtx2 == vtx) {
			vtx2 = vtx2->next;
			continue;
		}
		
		if (Vec2_Equal(&vtx->pos, &vtx2->pos)) {
			SplitVtx* kill = vtx2;
			Split* s = geoCtx->splitHead;
			SplitEdge* e = geoCtx->edgeHead;
			
			while (s) {
				for (s32 i = 0; i < 4; i++) {
					if (s->vtx[i] == vtx2) {
						s->vtx[i] = vtx;
					}
				}
				s = s->next;
			}
			
			while (e) {
				for (s32 i = 0; i < 2; i++) {
					if (e->vtx[i] == vtx2) {
						e->vtx[i] = vtx;
					}
				}
				e = e->next;
			}
			
			vtx2 = vtx2->next;
			Node_Kill(geoCtx->vtxHead, kill);
			OsPrintfEx("" PRNT_YELW "Kill Dublicate Vtx!");
			continue;
		}
		
		vtx2 = vtx2->next;
	}
}

void GeoGrid_Update_Vtx(GeoGridContext* geoCtx) {
	SplitVtx* vtx = geoCtx->vtxHead;
	static s32 clean;
	
	if (geoCtx->actionEdge != NULL) {
		clean = true;
	}
	
	while (vtx) {
		if (clean == true && geoCtx->actionEdge == NULL) {
			GeoGrid_Update_Vtx_RemoveDublicates(geoCtx, vtx);
		}
		
		if (vtx->killFlag == true) {
			Split* s = geoCtx->splitHead;
			
			while (s) {
				for (s32 i = 0; i < 4; i++) {
					if (s->vtx[i] == vtx) {
						vtx->killFlag = false;
					}
				}
				s = s->next;
			}
			
			if (vtx->killFlag == true) {
				SplitVtx* killVtx = vtx;
				vtx = vtx->prev;
				OsPrintfEx("" PRNT_YELW "Kill Tagged Vtx!");
				Node_Kill(geoCtx->vtxHead, killVtx);
				continue;
			}
		}
		
		vtx = vtx->next;
		if (vtx == NULL && geoCtx->actionEdge == NULL) {
			clean = false;
		}
	}
}

/* ───────────────────────────────────────────────────────────────────────── */

void GeoGrid_Update_Edge_RemoveDublicates(GeoGridContext* geoCtx, SplitEdge* edge) {
	SplitEdge* edge2 = geoCtx->edgeHead;
	
	while (edge2) {
		if (edge2 == edge) {
			edge2 = edge2->next;
			continue;
		}
		
		if (edge2->vtx[0] == edge->vtx[0] && edge2->vtx[1] == edge->vtx[1]) {
			SplitEdge* kill = edge2;
			Split* s = geoCtx->splitHead;
			
			if (geoCtx->actionEdge == edge2) {
				geoCtx->actionEdge = edge;
			}
			
			while (s) {
				for (s32 i = 0; i < 4; i++) {
					if (s->edge[i] == edge2) {
						s->edge[i] = edge;
					}
				}
				s = s->next;
			}
			
			edge2 = edge2->next;
			Node_Kill(geoCtx->edgeHead, kill);
			OsPrintfEx("" PRNT_YELW "Kill Dublicate Edge!");
			continue;
		}
		
		edge2 = edge2->next;
	}
}

void GeoGrid_Update_Edge_SetSlide(GeoGridContext* geoCtx) {
	SplitEdge* edge = geoCtx->edgeHead;
	f64 diffCentX = (f64)geoCtx->workRect.w / geoCtx->prevWorkRect.w;
	f64 diffCentY = (f64)geoCtx->workRect.h / geoCtx->prevWorkRect.h;
	
	while (edge) {
		bool clampFail = false;
		bool isEditEdge = (edge == geoCtx->actionEdge || edge->state & EDGE_EDIT);
		bool isCornerEdge = ((edge->state & EDGE_STICK) != 0);
		bool isHor = ((edge->state & EDGE_VERTICAL) == 0);
		bool matchesActionAlign = (geoCtx->actionEdge && ((edge->state & EDGE_ALIGN) == (geoCtx->actionEdge->state & EDGE_ALIGN)));
		
		OsAssert(!(edge->state & EDGE_HORIZONTAL && edge->state & EDGE_VERTICAL));
		
		if (isCornerEdge) {
			if (edge->state & EDGE_STICK_L) {
				edge->pos = geoCtx->workRect.x;
			}
			if (edge->state & EDGE_STICK_T) {
				edge->pos = geoCtx->workRect.y;
			}
			if (edge->state & EDGE_STICK_R) {
				edge->pos = geoCtx->workRect.x + geoCtx->workRect.w;
			}
			if (edge->state & EDGE_STICK_B) {
				edge->pos = geoCtx->workRect.y + geoCtx->workRect.h;
			}
		} else {
			if (edge->state & EDGE_HORIZONTAL) {
				edge->pos -= geoCtx->workRect.y;
				edge->pos *= diffCentY;
				edge->pos += geoCtx->workRect.y;
			}
			if (edge->state & EDGE_VERTICAL) {
				edge->pos *= diffCentX;
			}
		}
		
		if (isEditEdge && isCornerEdge == false) {
			SplitEdge* temp = geoCtx->edgeHead;
			s32 align = Wrap(edge->state & EDGE_ALIGN, 0, 1);
			
			while (temp) {
				for (s32 i = 0; i < 2; i++) {
					if (((temp->state & EDGE_ALIGN) != (edge->state & EDGE_ALIGN)) && temp->vtx[1] == edge->vtx[i]) {
						if (temp->vtx[0]->pos.s[align] > geoCtx->slide.clampMin) {
							geoCtx->slide.clampMin = temp->vtx[0]->pos.s[align];
							OsPrintfEx("foundMin %.2f", temp->vtx[0]->pos.s[align]);
						}
					}
				}
				
				for (s32 i = 0; i < 2; i++) {
					if (((temp->state & EDGE_ALIGN) != (edge->state & EDGE_ALIGN)) && temp->vtx[0] == edge->vtx[i]) {
						if (temp->vtx[1]->pos.s[align] < geoCtx->slide.clampMax) {
							geoCtx->slide.clampMax = temp->vtx[1]->pos.s[align];
							OsPrintfEx("foundMax %.2f", temp->vtx[1]->pos.s[align]);
						}
					}
				}
				
				temp = temp->next;
			}
			
			if (geoCtx->slide.clampMax - SPLIT_CLAMP > geoCtx->slide.clampMin + SPLIT_CLAMP) {
				if (edge->state & EDGE_HORIZONTAL) {
					edge->pos = __inputCtx->mouse.pos.y;
				}
				if (edge->state & EDGE_VERTICAL) {
					edge->pos = __inputCtx->mouse.pos.x;
				}
				edge->pos = CLAMP_MIN(edge->pos, geoCtx->slide.clampMin + SPLIT_CLAMP);
				edge->pos = CLAMP_MAX(edge->pos, geoCtx->slide.clampMax - SPLIT_CLAMP);
			} else {
				clampFail = true;
			}
		}
		
		if (isCornerEdge) {
			s32 revAlign = Wrap(isHor + 1, 0, 1);
			edge->vtx[0]->pos.s[isHor] = edge->pos;
			edge->vtx[1]->pos.s[isHor] = edge->pos;
		} else {
			if (isEditEdge && isCornerEdge == false && clampFail == false) {
				// edge->vtx[0]->pos.s[isHor] = floor(edge->pos * 0.125f) * 8.0f;
				// edge->vtx[1]->pos.s[isHor] = floor(edge->pos * 0.125f) * 8.0f;
				// edge->pos = floor(edge->pos * 0.125f) * 8.0f;
				edge->vtx[0]->pos.s[isHor] = edge->pos;
				edge->vtx[1]->pos.s[isHor] = edge->pos;
			} else {
				edge->vtx[0]->pos.s[isHor] = edge->pos;
				edge->vtx[1]->pos.s[isHor] = edge->pos;
			}
		}
		
		edge = edge->next;
	}
}

void GeoGrid_Update_Edges(GeoGridContext* geoCtx) {
	SplitEdge* edge = geoCtx->edgeHead;
	f64 diffCentX = (f64)geoCtx->workRect.w / geoCtx->prevWorkRect.w;
	f64 diffCentY = (f64)geoCtx->workRect.h / geoCtx->prevWorkRect.h;
	
	if (!geoCtx->mouse->click.hold) {
		geoCtx->actionEdge = NULL;
	}
	
	while (edge) {
		if (edge->killFlag == true) {
			SplitEdge* temp = edge->next;
			OsPrintfEx("" PRNT_YELW "Kill Tagged Edge!");
			Node_Kill(geoCtx->edgeHead, edge);
			edge = temp;
			continue;
		}
		edge->killFlag = true;
		
		if (geoCtx->actionEdge == NULL) {
			edge->state &= ~EDGE_EDIT;
		}
		
		GeoGrid_Update_Edge_RemoveDublicates(geoCtx, edge);
		edge = edge->next;
	}
	
	GeoGrid_Update_Edge_SetSlide(geoCtx);
}

/* ───────────────────────────────────────────────────────────────────────── */

void GeoGrid_Update_ActionSplit(GeoGridContext* geoCtx) {
	const char* stringDirection[] = {
		"EDGE_L",
		"EDGE_T",
		"EDGE_R",
		"EDGE_B",
	};
	const char* stringEdgeState[] = {
		"EDGE_HORIZONTAL", "EDGE_VERTICAL",   "EDGE_STICK_L",
		"EDGE_STICK_T",    "EDGE_STICK_R",    "EDGE_STICK_B",
	};
	Split* split = geoCtx->actionSplit;
	
	if (geoCtx->mouse->click.press) {
		SplitState tempStateA = GeoGrid_GetState_CursorPos(split, SPLIT_GRAB_DIST);
		SplitState tempStateB = GeoGrid_GetState_CursorPos(split, SPLIT_GRAB_DIST * 3);
		if (tempStateB & SPLIT_POINTS) {
			split->stateFlag |= tempStateB;
		} else if (tempStateA & SPLIT_SIDES) {
			split->stateFlag |= tempStateA;
		}
		
		if (split->stateFlag & SPLIT_SIDES) {
			s32 i;
			
			for (i = 0; i < 4; i++) {
				if (split->stateFlag & (1 << (4 + i))) {
					break;
				}
			}
			
			#ifndef NDEBUG
			char buffer[1024] = "None";
			s32 t = 0;
			OsPrintfEx("Split: %s", stringDirection[i]);
			
			for (s32 j = 0; (1 << j) <= EDGE_STICK_B; j++) {
				if (split->edge[i]->state & (1 << j)) {
					if (t++ == 0) {
						String_Copy(buffer, stringEdgeState[j]);
					} else {
						String_Merge(buffer, "|");
						String_Merge(buffer, stringEdgeState[j]);
					}
				}
			}
			
			OsPrintf("Edge: [%08X] [%s]", split->edge[i], buffer);
			
			#endif
			
			OsAssert(split->edge[i] != NULL);
			
			geoCtx->actionEdge = split->edge[i];
			GeoGrid_Edge_SetSlideClamp(geoCtx);
			GeoGrid_Reset(geoCtx);
		}
	}
	
	if (geoCtx->mouse->click.hold) {
		if (split->stateFlag & SPLIT_POINTS) {
			s32 splitDist =
			    GeoGrid_Cursor_GetDistTo(split->stateFlag & SPLIT_POINTS, split);
			s32 dist = Vec_Vec2s_DistXZ(&split->mousePos, &split->mousePressPos);
			
			if (dist > 1) {
				CursorIndex cid = GeoGrid_GerDir_MouseToPressPos(split) + 1;
				Cursor_SetCursor(cid);
			}
			if (dist > SPLIT_CLAMP * 1.05) {
				GeoGrid_Reset(geoCtx);
				if (split->mouseInSplit) {
					GeoGrid_Split(geoCtx, split, GeoGrid_GerDir_MouseToPressPos(split));
				} else {
					GeoGrid_KillSplit(geoCtx, split, GeoGrid_GerDir_MouseToPressPos(split));
				}
			}
		}
		if (split->stateFlag & SPLIT_SIDE_H) {
			Cursor_SetCursor(CURSOR_ARROW_H);
		}
		if (split->stateFlag & SPLIT_SIDE_V) {
			Cursor_SetCursor(CURSOR_ARROW_V);
		}
	}
}

void GeoGrid_Update_SplitRect(Split* split) {
	split->rect = (Rect) {
		floor(split->vtx[1]->pos.x),
		floor(split->vtx[1]->pos.y),
		floor(split->vtx[3]->pos.x) - floor(split->vtx[1]->pos.x),
		floor(split->vtx[3]->pos.y) - floor(split->vtx[1]->pos.y)
	};
}

void GeoGrid_Update_Split(GeoGridContext* geoCtx) {
	Split* split = geoCtx->splitHead;
	MouseInput* mouse = geoCtx->mouse;
	
	Cursor_SetCursor(CURSOR_DEFAULT);
	
	if (geoCtx->actionSplit != NULL && mouse->cursorAction == false) {
		GeoGrid_Reset(geoCtx);
	}
	
	while (split) {
		GeoGrid_Update_SplitRect(split);
		Vec2s rectPos = { split->rect.x, split->rect.y };
		Rect headerRect = {
			0, split->rect.h - SPLIT_BAR_HEIGHT,
			split->rect.w, SPLIT_BAR_HEIGHT
		};
		Vec2_Substract(split->mousePos, mouse->pos, rectPos);
		split->mouseInSplit = GeoGrid_Cursor_InSplit(split);
		split->mouseInHeader = GeoGrid_Cursor_InRect(split, &headerRect);
		split->center.x = split->rect.w * 0.5f;
		split->center.y = (split->rect.h - SPLIT_BAR_HEIGHT) * 0.5f;
		
		split->blockMouse = false;
		if (geoCtx->ctxMenu.num == 0) {
			if (GeoGrid_GetState_CursorPos(split, SPLIT_GRAB_DIST * 3) & SPLIT_POINTS &&
			    split->mouseInSplit) {
				Cursor_SetCursor(CURSOR_CROSSHAIR);
				split->blockMouse = true;
			} else if (GeoGrid_GetState_CursorPos(split, SPLIT_GRAB_DIST) & SPLIT_SIDE_H &&
			    split->mouseInSplit) {
				Cursor_SetCursor(CURSOR_ARROW_H);
				split->blockMouse = true;
			} else if (GeoGrid_GetState_CursorPos(split, SPLIT_GRAB_DIST) & SPLIT_SIDE_V &&
			    split->mouseInSplit) {
				Cursor_SetCursor(CURSOR_ARROW_V);
				split->blockMouse = true;
			}
			
			if (geoCtx->actionSplit == NULL && split->mouseInSplit && mouse->cursorAction) {
				if (mouse->click.press) {
					split->mousePressPos = split->mousePos;
					geoCtx->actionSplit = split;
				}
			}
			
			if (geoCtx->actionSplit != NULL && geoCtx->actionSplit == split) {
				GeoGrid_Update_ActionSplit(geoCtx);
			}
		}
		
		if (split->stateFlag != 0) {
			split->blockMouse = true;
		}
		
		u32 id = split->id;
		SplitTask* table = geoCtx->taskTable;
		if (split->id > 0) {
			if (split->id != split->prevId) {
				if (split->prevId != 0) {
					table[id].destroy(geoCtx->passArg, split->instance, split);
					free(split->instance);
				}
				
				split->instance = Lib_Calloc(0, table[id].size);
				OsPrintfEx("%08X", split->instance);
				table[id].init(geoCtx->passArg, split->instance, split);
				split->prevId = split->id;
			}
			table[id].update(geoCtx->passArg, split->instance, split);
		} else {
			if (split->prevId != 0) {
				id = split->prevId;
				table[id].destroy(geoCtx->passArg, split->instance, split);
				free(split->instance);
				split->prevId = 0;
			}
		}
		
		for (s32 i = 0; i < 4; i++) {
			OsAssert(split->edge[i] != NULL);
			split->edge[i]->killFlag = false;
		}
		split = split->next;
	}
}

/* ───────────────────────────────────────────────────────────────────────── */

bool GeoGrid_Button(Split* split, void* vg, const char* txt, Rect rect) {
	bool ret = false;
	
	nvgBeginPath(vg);
	nvgFillColor(vg, Theme_GetColor(THEME_LGHT, 175));
	nvgRoundedRect(
		vg,
		rect.x - 1.0f,
		rect.y - 1.0f,
		rect.w + 1.0f * 2,
		rect.h + 1.0f * 2,
		SPLIT_ROUND_R
	);
	nvgFill(vg);
	
	nvgBeginPath(vg);
	
	if (GeoGrid_Cursor_InRect(split, &rect)) {
		if (__inputCtx->mouse.clickL.press) {
			ret = true;
		}
		if (__inputCtx->mouse.clickL.hold) {
			nvgFillColor(vg, Theme_GetColor(THEME_BUTP, 175));
		} else {
			nvgFillColor(vg, Theme_GetColor(THEME_BUTH, 175));
		}
	} else {
		nvgFillColor(vg, Theme_GetColor(THEME_BUTI, 175));
	}
	nvgRoundedRect(vg, rect.x, rect.y, rect.w, rect.h, SPLIT_ROUND_R);
	nvgFill(vg);
	
	return ret;
}

void GeoGrid_SetContextMenu(GeoGridContext* geoCtx, Split* split, char** optionList, s32 num) {
	GeoCtxMenu* ctxMenu = &geoCtx->ctxMenu;
	
	ctxMenu->optionList = optionList;
	ctxMenu->split = split;
	ctxMenu->num = num;
	ctxMenu->pos = split->mousePos;
	ctxMenu->pos.x += split->vtx[1]->pos.x;
	ctxMenu->pos.y += split->vtx[1]->pos.y;
	
	OsPrintfEx("Set %d", num);
}

s32 GeoGrid_GetContextMenuResult(GeoGridContext* geoCtx, Split* split) {
	if (geoCtx->ctxMenu.split == split && geoCtx->ctxMenu.num < 0) {
		s32 ret = geoCtx->ctxMenu.num;
		
		geoCtx->ctxMenu.num = 0;
		geoCtx->ctxMenu.split = NULL;
		
		OsPrintfEx("Get");
		
		return -ret;
	}
	
	return 0;
}

/* ───────────────────────────────────────────────────────────────────────── */

void GeoGrid_Draw_Debug(GeoGridContext* geoCtx) {
	SplitVtx* vtx = geoCtx->vtxHead;
	Split* split = geoCtx->splitHead;
	s32 num = 0;
	Vec2s* winDim = geoCtx->winDim;
	
	glViewport(0, 0, winDim->x, winDim->y);
	nvgBeginFrame(geoCtx->vg, winDim->x, winDim->y, 1.0f); {
		while (split) {
			nvgBeginPath(geoCtx->vg);
			nvgLineCap(geoCtx->vg, NVG_ROUND);
			nvgStrokeWidth(geoCtx->vg, 0.8f);
			nvgMoveTo(
				geoCtx->vg,
				split->vtx[0]->pos.x + 2,
				split->vtx[0]->pos.y - 2
			);
			nvgLineTo(
				geoCtx->vg,
				split->vtx[1]->pos.x + 2,
				split->vtx[1]->pos.y + 2
			);
			nvgLineTo(
				geoCtx->vg,
				split->vtx[2]->pos.x - 2,
				split->vtx[2]->pos.y + 2
			);
			nvgLineTo(
				geoCtx->vg,
				split->vtx[3]->pos.x - 2,
				split->vtx[3]->pos.y - 2
			);
			nvgLineTo(
				geoCtx->vg,
				split->vtx[0]->pos.x + 2,
				split->vtx[0]->pos.y - 2
			);
			nvgStrokeColor(geoCtx->vg, nvgHSLA(0.111 * num, 1.0f, 0.4f, 255));
			nvgStroke(geoCtx->vg);
			
			split = split->next;
			num++;
		}
		
		num = 0;
		
		while (vtx) {
			char buf[128];
			Vec2f pos = {
				vtx->pos.x + CLAMP(
					winDim->x * 0.5 - vtx->pos.x,
					-150.0f,
					150.0f
				) *
				0.1f,
				vtx->pos.y + CLAMP(
					winDim->y * 0.5 - vtx->pos.y,
					-150.0f,
					150.0f
				) *
				0.1f
			};
			
			sprintf(buf, "%d", num);
			nvgFontSize(geoCtx->vg, SPLIT_TEXT_SCALE);
			nvgFontFace(geoCtx->vg, "sans");
			nvgTextAlign(geoCtx->vg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
			nvgFillColor(geoCtx->vg, Theme_GetColor(THEME_HEDR, 255));
			nvgFontBlur(geoCtx->vg, 1.5f);
			nvgText(geoCtx->vg, pos.x, pos.y, buf, 0);
			nvgFontBlur(geoCtx->vg, 0);
			nvgFillColor(geoCtx->vg, Theme_GetColor(THEME_TEXT, 255));
			nvgText(geoCtx->vg, pos.x, pos.y, buf, 0);
			
			vtx = vtx->next;
			num++;
		}
		
	} nvgEndFrame(geoCtx->vg);
}

void GeoGrid_Draw_SplitHeader(GeoGridContext* geoCtx, Split* split) {
	Rect rect = split->rect;
	s32 menuSel;
	
	rect.x = 0;
	rect.y = CLAMP_MIN(rect.h - SPLIT_BAR_HEIGHT, 0);
	
	nvgBeginPath(geoCtx->vg);
	nvgRect(geoCtx->vg, 0, CLAMP_MIN(rect.h - SPLIT_BAR_HEIGHT, 0), rect.w, rect.h);
	nvgPathWinding(geoCtx->vg, NVG_HOLE);
	nvgFillColor(geoCtx->vg, Theme_GetColor(THEME_BASE, 255));
	nvgFill(geoCtx->vg);
	
	rect.x += 8 + 4;
	rect.y += 4;
	rect.w = SPLIT_BAR_HEIGHT - SPLIT_SPLIT_W - 8 + 12;
	rect.h = SPLIT_BAR_HEIGHT - SPLIT_SPLIT_W - 8;
	
	if (GeoGrid_Button(split, geoCtx->vg, "", rect)) {
		static char* optionList[64] = { NULL };
		
		for (s32 i = 0; i < geoCtx->taskTableNum; i++) {
			optionList[i] = geoCtx->taskTable[i].taskName;
		}
		
		GeoGrid_SetContextMenu(geoCtx, split, optionList, geoCtx->taskTableNum);
	}
	
	if ((menuSel = GeoGrid_GetContextMenuResult(geoCtx, split))) {
		split->id = menuSel - 1;
	}
}

void GeoGrid_Draw_SplitBorder(GeoGridContext* geoCtx, Split* split) {
	void* vg = geoCtx->vg;
	Rect* rect = &split->rect;
	Rectf32 adjRect = {
		0 + SPLIT_SPLIT_W,
		0 + SPLIT_SPLIT_W,
		rect->w - SPLIT_SPLIT_W * 2,
		rect->h - SPLIT_SPLIT_W * 2
	};
	
	if (split->id > 0) {
		u32 id = split->id;
		SplitTask* table = geoCtx->taskTable;
		
		nvgBeginPath(vg);
		nvgRect(vg, 0, 0, rect->w, rect->h);
		nvgPathWinding(vg, NVG_HOLE);
		nvgFillColor(vg, Theme_GetColor(THEME_LINE, 195));
		nvgFill(vg);
		
		nvgEndFrame(geoCtx->vg);
		nvgBeginFrame(geoCtx->vg, split->rect.w, split->rect.h, 1.0f); {
			table[id].draw(geoCtx->passArg, split->instance, split);
			Elements_Draw(geoCtx, split);
		} nvgEndFrame(geoCtx->vg);
		nvgBeginFrame(geoCtx->vg, split->rect.w, split->rect.h, 1.0f);
	} else {
		nvgBeginPath(vg);
		nvgRect(vg, 0, 0, rect->w, rect->h);
		nvgPathWinding(vg, NVG_HOLE);
		nvgFillColor(vg, Theme_GetColor(THEME_SPBG, 255));
		nvgFill(vg);
		
		#if 0
		void* vg = geoCtx->vg;
		f64 thing = 25.0f;
		Vec2f p[] = {
			{ 0.0, -4.5 },
			{ 5.0, 4.5 },
			{ -5.0, 4.5 }
		};
		
		nvgBeginPath(vg);
		nvgFillPaint(
			vg,
			nvgRadialGradient(
				vg,
				split->center.x,
				split->center.y,
				0,
				800,
				Theme_GetColor(THEME_HEDR, 255),
				Theme_GetColor(THEME_SHDW, 255)
			)
		);
		nvgMoveTo(vg, split->center.x + p[0].x * thing, split->center.y + p[0].y * thing);
		
		for (s32 i = 1; i <= ArrayCount(p); i++) {
			s32 j = Wrap(i, 0, ArrayCount(p) - 1);
			
			nvgLineTo(vg, split->center.x + p[j].x * thing, split->center.y + p[j].y * thing);
		}
		nvgFill(vg);
		#endif
	}
	
	GeoGrid_Draw_SplitHeader(geoCtx, split);
	
	nvgBeginPath(geoCtx->vg);
	nvgRect(geoCtx->vg, 0, 0, rect->w, rect->h);
	nvgRoundedRect(
		geoCtx->vg,
		adjRect.x,
		adjRect.y,
		adjRect.w,
		adjRect.h,
		SPLIT_ROUND_R
	);
	nvgPathWinding(geoCtx->vg, NVG_HOLE);
	nvgFillColor(geoCtx->vg, Theme_GetColor(THEME_LINE, 255));
	nvgFill(geoCtx->vg);
}

void GeoGrid_Draw_Splits(GeoGridContext* geoCtx) {
	Split* split = geoCtx->splitHead;
	Vec2s* winDim = geoCtx->winDim;
	
	while (split != NULL) {
		GeoGrid_Update_SplitRect(split);
		glViewport(
			split->rect.x,
			winDim->y - split->rect.y - split->rect.h,
			split->rect.w,
			split->rect.h
		);
		nvgBeginFrame(geoCtx->vg, split->rect.w, split->rect.h, 1.0f); {
			GeoGrid_Draw_SplitBorder(geoCtx, split);
		} nvgEndFrame(geoCtx->vg);
		
		split = split->next;
	}
}

/* ───────────────────────────────────────────────────────────────────────── */

void GeoGrid_Update_ContextMenu(GeoGridContext* geoCtx) {
	GeoCtxMenu* ctxMenu = &geoCtx->ctxMenu;
	void* vg = geoCtx->vg;
	Rect menuRect = {
		ctxMenu->pos.x,
		ctxMenu->pos.y,
		SPLIT_TEXT_SCALE* 16,
		0
	};
	
	if (ctxMenu->num == 0)
		return;
	
	ctxMenu->hoverNum = -1;
	
	for (s32 i = 0; i < ctxMenu->num; i++) {
		if (ctxMenu->optionList[i] != NULL) {
			menuRect.h += SPLIT_TEXT_SPLIT + SPLIT_TEXT_SPLIT + SPLIT_TEXT_SCALE;
		} else {
			menuRect.h += SPLIT_TEXT_SPLIT;
		}
	}
	
	if (menuRect.y > geoCtx->winDim->y * 0.5) {
		menuRect.y = ctxMenu->pos.y - menuRect.h;
	}
	
	menuRect.h = 0;
	
	for (s32 i = 0; i < ctxMenu->num; i++) {
		if (ctxMenu->optionList[i] != NULL) {
			Rect pressRect;
			
			menuRect.h += SPLIT_TEXT_SPLIT;
			
			pressRect.x = menuRect.x;
			pressRect.y = menuRect.y + menuRect.h - SPLIT_TEXT_SPLIT;
			pressRect.w = menuRect.w;
			pressRect.h = SPLIT_TEXT_SCALE + SPLIT_TEXT_SPLIT * 2;
			
			if (geoCtx->mouse->pos.x >= pressRect.x && geoCtx->mouse->pos.x < pressRect.x + pressRect.w) {
				if (geoCtx->mouse->pos.y >= pressRect.y && geoCtx->mouse->pos.y < pressRect.y + pressRect.h) {
					ctxMenu->hoverNum = i;
					ctxMenu->hoverRect = pressRect;
					if (geoCtx->mouse->clickL.press) {
						ctxMenu->num = -(i + 1);
						
						return;
					}
				}
			}
			
			menuRect.h += SPLIT_TEXT_SCALE;
			menuRect.h += SPLIT_TEXT_SPLIT;
		} else {
			menuRect.h += SPLIT_TEXT_SPLIT;
		}
	}
	
	if (geoCtx->mouse->pos.x < ctxMenu->pos.x - SPLIT_CTXM_DIST ||
	    geoCtx->mouse->pos.x > ctxMenu->pos.x + menuRect.w + SPLIT_CTXM_DIST ||
	    geoCtx->mouse->pos.y < menuRect.y - SPLIT_CTXM_DIST ||
	    geoCtx->mouse->pos.y > menuRect.y + menuRect.h + SPLIT_CTXM_DIST
	) {
		ctxMenu->num = 0;
		ctxMenu->split = NULL;
	}
	
	if (geoCtx->mouse->pos.x > ctxMenu->pos.x ||
	    geoCtx->mouse->pos.x < ctxMenu->pos.x + menuRect.w ||
	    geoCtx->mouse->pos.y > menuRect.y ||
	    geoCtx->mouse->pos.y < menuRect.y + menuRect.h
	) {
		if (geoCtx->mouse->click.press) {
			ctxMenu->num = 0;
			ctxMenu->split = NULL;
			geoCtx->mouse->click.press = 0;
			geoCtx->mouse->clickL.press = 0;
			geoCtx->mouse->clickR.press = 0;
			geoCtx->mouse->clickMid.press = 0;
		}
	}
}

void GeoGrid_Draw_ContextMenu(GeoGridContext* geoCtx) {
	GeoCtxMenu* ctxMenu = &geoCtx->ctxMenu;
	void* vg = geoCtx->vg;
	Rect menuRect = {
		ctxMenu->pos.x,
		ctxMenu->pos.y,
		SPLIT_TEXT_SCALE* 16,
		0
	};
	
	for (s32 i = 0; i < ctxMenu->num; i++) {
		if (ctxMenu->optionList[i] != NULL) {
			menuRect.h += SPLIT_TEXT_SPLIT + SPLIT_TEXT_SPLIT + SPLIT_TEXT_SCALE;
		} else {
			menuRect.h += SPLIT_TEXT_SPLIT;
		}
	}
	
	if (menuRect.y > geoCtx->winDim->y * 0.5) {
		menuRect.y -= menuRect.h;
	}
	
	if (ctxMenu->num > 0) {
		glViewport(0, 0, geoCtx->winDim->x, geoCtx->winDim->y);
		nvgBeginFrame(vg, geoCtx->winDim->x, geoCtx->winDim->y, 1.0f); {
			nvgBeginPath(vg);
			nvgFillColor(vg, Theme_GetColor(THEME_SHDW, 255));
			nvgRoundedRect(vg, menuRect.x - 0.5, menuRect.y - 0.5, menuRect.w + 1.0, menuRect.h + 1.0, SPLIT_ROUND_R);
			nvgFill(vg);
			
			nvgBeginPath(vg);
			nvgFillColor(vg, Theme_GetColor(THEME_LINE, 255));
			nvgRoundedRect(vg, menuRect.x, menuRect.y, menuRect.w, menuRect.h, SPLIT_ROUND_R);
			nvgFill(vg);
			
			if (ctxMenu->hoverNum != -1) {
				nvgBeginPath(vg);
				nvgFillColor(vg, Theme_GetColor(THEME_SPBG, 255));
				nvgRoundedRect(
					vg,
					ctxMenu->hoverRect.x,
					ctxMenu->hoverRect.y,
					ctxMenu->hoverRect.w,
					ctxMenu->hoverRect.h,
					SPLIT_ROUND_R
				);
				nvgFill(vg);
			}
			
			menuRect.y = ctxMenu->pos.y;
			menuRect.x += SPLIT_TEXT_SPLIT * 4;
			
			if (menuRect.y > geoCtx->winDim->y * 0.5) {
				menuRect.y -= menuRect.h;
			}
			
			menuRect.y += SPLIT_TEXT_SPLIT * 0.25;
			
			nvgFillColor(geoCtx->vg, Theme_GetColor(THEME_TEXT, 255));
			nvgFontFace(vg, "sans");
			nvgFontSize(vg, SPLIT_TEXT_SCALE);
			nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
			
			for (s32 i = 0; i < ctxMenu->num; i++) {
				if (ctxMenu->optionList[i] != NULL) {
					nvgText(vg, menuRect.x, menuRect.y + SPLIT_TEXT_SPLIT, ctxMenu->optionList[i], NULL);
					menuRect.y += SPLIT_TEXT_SCALE + SPLIT_TEXT_SPLIT + SPLIT_TEXT_SPLIT;
				} else {
					nvgBeginPath(vg);
					nvgMoveTo(vg, ctxMenu->pos.x + SPLIT_TEXT_SPLIT, menuRect.y + SPLIT_TEXT_SPLIT * 0.5);
					nvgLineTo(vg, ctxMenu->pos.x + menuRect.w - SPLIT_TEXT_SPLIT, menuRect.y + SPLIT_TEXT_SPLIT * 0.5);
					nvgStrokeWidth(vg, 1.0f);
					nvgStrokeColor(vg, Theme_GetColor(THEME_TEXT, 255));
					nvgStroke(vg);
					menuRect.y += SPLIT_TEXT_SPLIT;
				}
			}
			
		} nvgEndFrame(vg);
	}
}

/* ───────────────────────────────────────────────────────────────────────── */

void GeoGrid_RemoveDublicates(GeoGridContext* geoCtx) {
	SplitVtx* vtx = geoCtx->vtxHead;
	SplitEdge* edge = geoCtx->edgeHead;
	
	while (vtx) {
		GeoGrid_Update_Vtx_RemoveDublicates(geoCtx, vtx);
		vtx = vtx->next;
	}
	
	while (edge) {
		GeoGrid_Update_Edge_RemoveDublicates(geoCtx, edge);
		edge = edge->next;
	}
}

void GeoGrid_UpdateWorkRect(GeoGridContext* geoCtx) {
	Vec2s* winDim = geoCtx->winDim;
	
	geoCtx->workRect = (Rect) { 0, 0 + geoCtx->bar[BAR_TOP].rect.h, winDim->x,
				    winDim->y - geoCtx->bar[BAR_BOT].rect.h -
				    geoCtx->bar[BAR_TOP].rect.h };
}

void GeoGrid_SetTopBarHeight(GeoGridContext* geoCtx, s32 h) {
	geoCtx->bar[BAR_TOP].rect.x = 0;
	geoCtx->bar[BAR_TOP].rect.y = 0;
	geoCtx->bar[BAR_TOP].rect.w = geoCtx->winDim->x;
	geoCtx->bar[BAR_TOP].rect.h = h;
	GeoGrid_UpdateWorkRect(geoCtx);
}

void GeoGrid_SetBotBarHeight(GeoGridContext* geoCtx, s32 h) {
	geoCtx->bar[BAR_BOT].rect.x = 0;
	geoCtx->bar[BAR_BOT].rect.y = geoCtx->winDim->y - h;
	geoCtx->bar[BAR_BOT].rect.w = geoCtx->winDim->x;
	geoCtx->bar[BAR_BOT].rect.h = h;
	GeoGrid_UpdateWorkRect(geoCtx);
}

void GeoGrid_Layout_SaveJson(GeoGridContext* geoCtx) {
	Split* split = geoCtx->splitHead;
	cJSON* layout = cJSON_CreateObject();
	cJSON* temp;
	cJSON* num;
	cJSON* jsonSplit = cJSON_CreateArray();
	
	cJSON_AddItemToObject(layout, "splits", jsonSplit);
	
	while (split) {
		cJSON_AddItemToObject(layout, "splits", jsonSplit);
		temp = cJSON_CreateObject();
		cJSON_AddItemToArray(jsonSplit, temp);
		
		num = cJSON_CreateNumber(split->vtx[0]->pos.x);
		cJSON_AddItemToObject(temp, "v0x", num);
		num = cJSON_CreateNumber(split->vtx[0]->pos.y);
		cJSON_AddItemToObject(temp, "v0y", num);
		
		num = cJSON_CreateNumber(split->vtx[1]->pos.x);
		cJSON_AddItemToObject(temp, "v1x", num);
		num = cJSON_CreateNumber(split->vtx[1]->pos.y);
		cJSON_AddItemToObject(temp, "v1y", num);
		
		num = cJSON_CreateNumber(split->vtx[2]->pos.x);
		cJSON_AddItemToObject(temp, "v2x", num);
		num = cJSON_CreateNumber(split->vtx[2]->pos.y);
		cJSON_AddItemToObject(temp, "v2y", num);
		
		num = cJSON_CreateNumber(split->vtx[3]->pos.x);
		cJSON_AddItemToObject(temp, "v3x", num);
		num = cJSON_CreateNumber(split->vtx[3]->pos.y);
		cJSON_AddItemToObject(temp, "v3y", num);
		
		num = cJSON_CreateNumber(split->id);
		cJSON_AddItemToObject(temp, "id", num);
		
		split = split->next;
	}
	
	jsonSplit = cJSON_CreateArray();
	cJSON_AddItemToObject(layout, "dim", jsonSplit);
	temp = cJSON_CreateObject();
	cJSON_AddItemToArray(jsonSplit, temp);
	
	num = cJSON_CreateNumber(geoCtx->winDim->x);
	cJSON_AddItemToObject(temp, "w", num);
	num = cJSON_CreateNumber(geoCtx->winDim->y);
	cJSON_AddItemToObject(temp, "h", num);
	
	FILE* file = fopen("Settings.json", "w");
	
	OsAssert(file);
	
	fprintf(file, "%s", cJSON_Print(layout));
	fclose(file);
	cJSON_Delete(layout);
}

Vec2s GeoGrid_Layout_LoadJson(GeoGridContext* geoCtx, Vec2s* winDim) {
	MemFile file = MemFile_Initialize();
	cJSON* json;
	cJSON* split;
	cJSON* temp = NULL;
	cJSON* dim;
	Vec2s ret;
	
	OsPrintfEx("Setting");
	
	MemFile_LoadFile(&file, "Settings.json");
	json = cJSON_ParseWithLength(file.data, file.dataSize);
	split = cJSON_GetObjectItem(json, "splits");
	dim = cJSON_GetObjectItem(json, "dim");
	
	cJSON_ArrayForEach(temp, dim) {
		cJSON* w = cJSON_GetObjectItemCaseSensitive(temp, "w");
		cJSON* h = cJSON_GetObjectItemCaseSensitive(temp, "h");
		
		geoCtx->winDim = winDim;
		winDim->x = ret.x = w->valueint;
		winDim->y = ret.y = h->valueint;
		
		GeoGrid_SetTopBarHeight(geoCtx, SPLIT_BAR_HEIGHT);
		GeoGrid_SetBotBarHeight(geoCtx, SPLIT_BAR_HEIGHT);
	}
	
	cJSON_ArrayForEach(temp, split) {
		cJSON* v0x = cJSON_GetObjectItemCaseSensitive(temp, "v0x");
		cJSON* v0y = cJSON_GetObjectItemCaseSensitive(temp, "v0y");
		cJSON* v1x = cJSON_GetObjectItemCaseSensitive(temp, "v1x");
		cJSON* v1y = cJSON_GetObjectItemCaseSensitive(temp, "v1y");
		cJSON* v2x = cJSON_GetObjectItemCaseSensitive(temp, "v2x");
		cJSON* v2y = cJSON_GetObjectItemCaseSensitive(temp, "v2y");
		cJSON* v3x = cJSON_GetObjectItemCaseSensitive(temp, "v3x");
		cJSON* v3y = cJSON_GetObjectItemCaseSensitive(temp, "v3y");
		cJSON* id = cJSON_GetObjectItemCaseSensitive(temp, "id");
		Split* split = Lib_Calloc(0, sizeof(Split));
		
		split->vtx[VTX_BOT_L] = GeoGrid_AddVtx(geoCtx, v0x->valuedouble, v0y->valuedouble);
		split->vtx[VTX_TOP_L] = GeoGrid_AddVtx(geoCtx, v1x->valuedouble, v1y->valuedouble);
		split->vtx[VTX_TOP_R] = GeoGrid_AddVtx(geoCtx, v2x->valuedouble, v2y->valuedouble);
		split->vtx[VTX_BOT_R] = GeoGrid_AddVtx(geoCtx, v3x->valuedouble, v3y->valuedouble);
		
		split->edge[EDGE_L] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_TOP_L]);
		split->edge[EDGE_T] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
		split->edge[EDGE_R] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
		split->edge[EDGE_B] = GeoGrid_AddEdge(geoCtx, split->vtx[VTX_BOT_R], split->vtx[VTX_BOT_L]);
		
		split->id = id->valueint;
		
		Node_Add(geoCtx->splitHead, split);
	}
	
	MemFile_Free(&file);
	
	geoCtx->jsonSettings = true;
	
	return ret;
}

/* ───────────────────────────────────────────────────────────────────────── */

void GeoGrid_Init(GeoGridContext* geoCtx, Vec2s* winDim, MouseInput* mouse, void* vg) {
	geoCtx->winDim = winDim;
	geoCtx->mouse = mouse;
	geoCtx->vg = vg;
	GeoGrid_SetTopBarHeight(geoCtx, SPLIT_BAR_HEIGHT);
	GeoGrid_SetBotBarHeight(geoCtx, SPLIT_BAR_HEIGHT);
	
	if (geoCtx->jsonSettings == false) {
		Rectf32 lHalf = {
			geoCtx->workRect.x,
			geoCtx->workRect.y,
			(f64)geoCtx->workRect.w * 0.75,
			geoCtx->workRect.h
		};
		Rectf32 rHalf = {
			geoCtx->workRect.x + (f64)geoCtx->workRect.w * 0.75,
			geoCtx->workRect.y,
			(f64)geoCtx->workRect.w * 0.25,
			geoCtx->workRect.h
		};
		GeoGrid_AddSplit(geoCtx, &lHalf)->id = 1;
		GeoGrid_AddSplit(geoCtx, &rHalf);
	}
	
	geoCtx->prevWorkRect = geoCtx->workRect;
	Elements_Init(geoCtx);
}

void GeoGrid_Update(GeoGridContext* geoCtx) {
	GeoGrid_SetTopBarHeight(geoCtx, geoCtx->bar[BAR_TOP].rect.h);
	GeoGrid_SetBotBarHeight(geoCtx, geoCtx->bar[BAR_BOT].rect.h);
	Elements_Update(geoCtx);
	GeoGrid_Update_ContextMenu(geoCtx);
	GeoGrid_Update_Vtx(geoCtx);
	GeoGrid_Update_Edges(geoCtx);
	GeoGrid_Update_Split(geoCtx);
	
	if (__inputCtx->key[KEY_ENTER].press) {
		GeoGrid_Layout_SaveJson(geoCtx);
	}
	
	geoCtx->prevWorkRect = geoCtx->workRect;
}

void GeoGrid_Draw(GeoGridContext* geoCtx) {
	Vec2s* winDim = geoCtx->winDim;
	
	// Draw Bars
	for (s32 i = 0; i < 2; i++) {
		glViewport(
			geoCtx->bar[i].rect.x,
			winDim->y - geoCtx->bar[i].rect.y - geoCtx->bar[i].rect.h,
			geoCtx->bar[i].rect.w,
			geoCtx->bar[i].rect.h
		);
		nvgBeginFrame(geoCtx->vg, geoCtx->bar[i].rect.w, geoCtx->bar[i].rect.h, 1.0f); {
			nvgBeginPath(geoCtx->vg);
			nvgRect(
				geoCtx->vg,
				0,
				0,
				geoCtx->bar[i].rect.w,
				geoCtx->bar[i].rect.h
			);
			nvgFillColor(geoCtx->vg, Theme_GetColor(THEME_HEDR, 255));
			nvgFill(geoCtx->vg);
			
			if (i == 1) {
				nvgFontSize(geoCtx->vg, SPLIT_TEXT_SCALE);
				nvgFontFace(geoCtx->vg, "sans");
				nvgTextAlign(geoCtx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
				
				nvgFillColor(geoCtx->vg, Theme_GetColor(THEME_SHDW, 255));
				nvgFontBlur(geoCtx->vg, 2.0f);
				nvgText(
					geoCtx->vg,
					6,
					6,
					gBuild,
					NULL
				);
				
				nvgFillColor(geoCtx->vg, Theme_GetColor(THEME_TEXT, 255));
				nvgFontBlur(geoCtx->vg, 0.0f);
				nvgText(
					geoCtx->vg,
					6,
					6,
					gBuild,
					NULL
				);
			}
		} nvgEndFrame(geoCtx->vg);
	}
	
	GeoGrid_Draw_Splits(geoCtx);
	// GeoGrid_Draw_Debug(geoCtx);
	GeoGrid_Draw_ContextMenu(geoCtx);
}