#include "Editor.h"

void GeoGrid_RemoveDublicates(GeoGridContext* geoGridCtx);
void GeoGrid_Update_SplitRect(Split* split);

/* ───────────────────────────────────────────────────────────────────────── */

SplitDir GeoGrid_GetDir_Opposite(SplitDir dir) {
	return Lib_Wrap(dir + 2, DIR_L, DIR_B);
}

SplitDir GeoGrid_GerDir_MouseToPressPos(Split* split) {
	Vec2s pos;
	
	Vec2_Substract(&pos, &split->mousePos, &split->mousePressPos);
	
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

SplitVtx* GeoGrid_AddVtx(GeoGridContext* geoGridCtx, f64 x, f64 y) {
	SplitVtx* head = geoGridCtx->vtxHead;
	
	while (head) {
		if (ABS(head->pos.x - x) <= 1.0 && ABS(head->pos.y - y) <= 1.0) {
			OsPrintfEx("VtxConnect %.2f %.2f", x, y);
			
			return head;
		}
		head = head->next;
	}
	
	SplitVtx* vtx = Lib_Calloc(0, sizeof(SplitVtx));
	
	vtx->pos.x = x;
	vtx->pos.y = y;
	Node_Add(geoGridCtx->vtxHead, vtx);
	
	return vtx;
}

SplitEdge* GeoGrid_AddEdge(GeoGridContext* geoGridCtx, SplitVtx* v1, SplitVtx* v2) {
	SplitEdge* head = geoGridCtx->edgeHead;
	SplitEdge* edge = NULL;
	
	OsAssert(v1 != NULL && v2 != NULL);
	
	if (v1->pos.y == v2->pos.y) {
		if (v1->pos.x > v2->pos.x) {
			Lib_Swap(v1, v2);
		}
	} else {
		if (v1->pos.y > v2->pos.y) {
			Lib_Swap(v1, v2);
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
		Node_Add(geoGridCtx->edgeHead, edge);
	}
	
	if (edge->vtx[0]->pos.y == edge->vtx[1]->pos.y) {
		edge->state |= EDGE_HORIZONTAL;
		edge->pos = edge->vtx[0]->pos.y;
		if (edge->pos < geoGridCtx->workRect.y + 1) {
			edge->state |= EDGE_STICK_T;
		}
		if (edge->pos > geoGridCtx->workRect.y + geoGridCtx->workRect.h - 1) {
			edge->state |= EDGE_STICK_B;
		}
	} else {
		edge->state |= EDGE_VERTICAL;
		edge->pos = edge->vtx[0]->pos.x;
		if (edge->pos < geoGridCtx->workRect.x + 1) {
			edge->state |= EDGE_STICK_L;
		}
		if (edge->pos > geoGridCtx->workRect.x + geoGridCtx->workRect.w - 1) {
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

bool GeoGrid_Cursor_InSplit(Split* split) {
	s32 resX = (split->mousePos.x < split->rect.w && split->mousePos.x >= 0);
	s32 resY = (split->mousePos.y < split->rect.h && split->mousePos.y >= 0);
	
	return (resX && resY);
}

void GeoGrid_AddSplit(GeoGridContext* geoGridCtx, Rect* rect) {
	Split* split = Lib_Calloc(0, sizeof(Split));
	
	split->vtx[VTX_BOT_L] = GeoGrid_AddVtx(geoGridCtx, rect->x, rect->y + rect->h);
	split->vtx[VTX_TOP_L] = GeoGrid_AddVtx(geoGridCtx, rect->x, rect->y);
	split->vtx[VTX_TOP_R] = GeoGrid_AddVtx(geoGridCtx, rect->x + rect->w, rect->y);
	split->vtx[VTX_BOT_R] =
	    GeoGrid_AddVtx(geoGridCtx, rect->x + rect->w, rect->y + rect->h);
	
	split->edge[EDGE_L] =
	    GeoGrid_AddEdge(geoGridCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_TOP_L]);
	split->edge[EDGE_T] =
	    GeoGrid_AddEdge(geoGridCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
	split->edge[EDGE_R] =
	    GeoGrid_AddEdge(geoGridCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	split->edge[EDGE_B] =
	    GeoGrid_AddEdge(geoGridCtx, split->vtx[VTX_BOT_R], split->vtx[VTX_BOT_L]);
	
	Node_Add(geoGridCtx->splitHead, split);
}

void GeoGrid_Edge_SetSlideClamp(GeoGridContext* geoGridCtx) {
	SplitEdge* tempEdge = geoGridCtx->edgeHead;
	SplitEdge* setEdge = geoGridCtx->actionEdge;
	SplitEdge* actionEdge = geoGridCtx->actionEdge;
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
				tempEdge = geoGridCtx->edgeHead;
				posMin = setEdge->vtx[0]->pos.s[align];
				continue;
			}
		}
		
		tempEdge = tempEdge->next;
	}
	
	tempEdge = geoGridCtx->edgeHead;
	
	// Set all below setEdgeA
	while (tempEdge) {
		if ((tempEdge->state & EDGE_ALIGN) == (setEdge->state & EDGE_ALIGN)) {
			if (tempEdge->vtx[0] == setEdge->vtx[1]) {
				tempEdge->state |= EDGE_EDIT;
				setEdge = tempEdge;
				tempEdge = geoGridCtx->edgeHead;
				posMax = setEdge->vtx[1]->pos.s[align];
				continue;
			}
		}
		
		tempEdge = tempEdge->next;
	}
	
	tempEdge = geoGridCtx->edgeHead;
	
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
	
	if (geoGridCtx->actionEdge->state & EDGE_VERTICAL) {
		geoGridCtx->edgeMovement.clampMin = geoGridCtx->workRect.x;
		geoGridCtx->edgeMovement.clampMax = geoGridCtx->workRect.x + geoGridCtx->workRect.w;
	} else {
		geoGridCtx->edgeMovement.clampMin = geoGridCtx->workRect.y;
		geoGridCtx->edgeMovement.clampMax = geoGridCtx->workRect.y + geoGridCtx->workRect.h;
	}
}

void GeoGrid_Reset(GeoGridContext* geoGridCtx) {
	OsAssert(geoGridCtx->actionSplit);
	geoGridCtx->actionSplit->stateFlag &= ~(SPLIT_POINTS | SPLIT_SIDES);
	
	geoGridCtx->actionSplit = NULL;
}

void GeoGrid_Split(GeoGridContext* geoGridCtx, Split* split, SplitDir dir) {
	Split* partner;
	Split* newSplit;
	SplitDir mirDir = GeoGrid_GetDir_Opposite(dir);
	f64 splitPos = (dir == DIR_L || dir == DIR_R) ? geoGridCtx->mouse->pos.x : geoGridCtx->mouse->pos.y;
	
	newSplit = Lib_Calloc(0, sizeof(Split));
	
	Node_Add(geoGridCtx->splitHead, newSplit);
	
	OsPrintfEx("SplitDir: %d", dir);
	
	if (dir == DIR_L) {
		newSplit->vtx[0] = GeoGrid_AddVtx(geoGridCtx, splitPos, split->vtx[0]->pos.y);
		newSplit->vtx[1] = GeoGrid_AddVtx(geoGridCtx, splitPos, split->vtx[1]->pos.y);
		newSplit->vtx[2] =  GeoGrid_AddVtx(geoGridCtx, split->vtx[2]->pos.x, split->vtx[2]->pos.y);
		newSplit->vtx[3] = GeoGrid_AddVtx(geoGridCtx, split->vtx[3]->pos.x, split->vtx[3]->pos.y);
		split->vtx[2] = GeoGrid_AddVtx(geoGridCtx, splitPos, split->vtx[2]->pos.y);
		split->vtx[3] = GeoGrid_AddVtx(geoGridCtx, splitPos, split->vtx[3]->pos.y);
	}
	
	if (dir == DIR_R) {
		newSplit->vtx[0] = GeoGrid_AddVtx(geoGridCtx, split->vtx[0]->pos.x, split->vtx[0]->pos.y);
		newSplit->vtx[1] = GeoGrid_AddVtx(geoGridCtx, split->vtx[1]->pos.x, split->vtx[1]->pos.y);
		newSplit->vtx[2] = GeoGrid_AddVtx(geoGridCtx, splitPos, split->vtx[2]->pos.y);
		newSplit->vtx[3] = GeoGrid_AddVtx(geoGridCtx, splitPos, split->vtx[3]->pos.y);
		split->vtx[0] = GeoGrid_AddVtx(geoGridCtx, splitPos, split->vtx[0]->pos.y);
		split->vtx[1] = GeoGrid_AddVtx(geoGridCtx, splitPos, split->vtx[1]->pos.y);
	}
	
	if (dir == DIR_T) {
		newSplit->vtx[0] = GeoGrid_AddVtx(geoGridCtx, split->vtx[0]->pos.x, split->vtx[0]->pos.y);
		newSplit->vtx[1] = GeoGrid_AddVtx(geoGridCtx, split->vtx[1]->pos.x, splitPos);
		newSplit->vtx[2] = GeoGrid_AddVtx(geoGridCtx, split->vtx[2]->pos.x, splitPos);
		newSplit->vtx[3] = GeoGrid_AddVtx(geoGridCtx, split->vtx[3]->pos.x, split->vtx[3]->pos.y);
		split->vtx[3] = GeoGrid_AddVtx(geoGridCtx, split->vtx[3]->pos.x, splitPos);
		split->vtx[0] = GeoGrid_AddVtx(geoGridCtx, split->vtx[0]->pos.x, splitPos);
	}
	
	if (dir == DIR_B) {
		newSplit->vtx[0] = GeoGrid_AddVtx(geoGridCtx, split->vtx[0]->pos.x, splitPos);
		newSplit->vtx[1] = GeoGrid_AddVtx(geoGridCtx, split->vtx[1]->pos.x, split->vtx[1]->pos.y);
		newSplit->vtx[2] = GeoGrid_AddVtx(geoGridCtx, split->vtx[2]->pos.x, split->vtx[2]->pos.y);
		newSplit->vtx[3] = GeoGrid_AddVtx(geoGridCtx, split->vtx[3]->pos.x, splitPos);
		split->vtx[1] = GeoGrid_AddVtx(geoGridCtx, split->vtx[1]->pos.x, splitPos);
		split->vtx[2] = GeoGrid_AddVtx(geoGridCtx, split->vtx[2]->pos.x, splitPos);
	}
	
	split->edge[EDGE_L] = GeoGrid_AddEdge(geoGridCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_TOP_L]);
	split->edge[EDGE_T] = GeoGrid_AddEdge(geoGridCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
	split->edge[EDGE_R] = GeoGrid_AddEdge(geoGridCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	split->edge[EDGE_B] = GeoGrid_AddEdge(geoGridCtx, split->vtx[VTX_BOT_R], split->vtx[VTX_BOT_L]);
	
	newSplit->edge[EDGE_L] = GeoGrid_AddEdge(geoGridCtx, newSplit->vtx[VTX_BOT_L], newSplit->vtx[VTX_TOP_L]);
	newSplit->edge[EDGE_T] = GeoGrid_AddEdge(geoGridCtx, newSplit->vtx[VTX_TOP_L], newSplit->vtx[VTX_TOP_R]);
	newSplit->edge[EDGE_R] = GeoGrid_AddEdge(geoGridCtx, newSplit->vtx[VTX_TOP_R], newSplit->vtx[VTX_BOT_R]);
	newSplit->edge[EDGE_B] = GeoGrid_AddEdge(geoGridCtx, newSplit->vtx[VTX_BOT_R], newSplit->vtx[VTX_BOT_L]);
	
	geoGridCtx->actionEdge = newSplit->edge[dir];
	GeoGrid_RemoveDublicates(geoGridCtx);
	GeoGrid_Edge_SetSlideClamp(geoGridCtx);
	GeoGrid_Update_SplitRect(split);
	GeoGrid_Update_SplitRect(newSplit);
}

void GeoGrid_KillSplit(GeoGridContext* geoGridCtx, Split* split, SplitDir dir) {
	SplitEdge* sharedEdge = split->edge[dir];
	Split* killSplit = geoGridCtx->splitHead;
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
		split->edge[EDGE_T] = GeoGrid_AddEdge(geoGridCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
		split->edge[EDGE_B] = GeoGrid_AddEdge(geoGridCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_BOT_R]);
	}
	
	if (dir == DIR_T) {
		split->vtx[VTX_TOP_L] = killSplit->vtx[VTX_TOP_L];
		split->vtx[VTX_TOP_R] = killSplit->vtx[VTX_TOP_R];
		split->edge[EDGE_L] = GeoGrid_AddEdge(geoGridCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_BOT_L]);
		split->edge[EDGE_R] = GeoGrid_AddEdge(geoGridCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	}
	
	if (dir == DIR_R) {
		split->vtx[VTX_BOT_R] = killSplit->vtx[VTX_BOT_R];
		split->vtx[VTX_TOP_R] = killSplit->vtx[VTX_TOP_R];
		split->edge[EDGE_T] = GeoGrid_AddEdge(geoGridCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
		split->edge[EDGE_B] = GeoGrid_AddEdge(geoGridCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_BOT_R]);
	}
	
	if (dir == DIR_B) {
		split->vtx[VTX_BOT_L] = killSplit->vtx[VTX_BOT_L];
		split->vtx[VTX_BOT_R] = killSplit->vtx[VTX_BOT_R];
		split->edge[EDGE_L] = GeoGrid_AddEdge(geoGridCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_BOT_L]);
		split->edge[EDGE_R] = GeoGrid_AddEdge(geoGridCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	}
	
	split->edge[dir] = killSplit->edge[dir];
	
	Node_Kill(geoGridCtx->splitHead, killSplit);
	GeoGrid_RemoveDublicates(geoGridCtx);
	GeoGrid_Update_SplitRect(split);
}

/* ───────────────────────────────────────────────────────────────────────── */

void GeoGrid_Update_Vtx_RemoveDublicates(GeoGridContext* geoGridCtx, SplitVtx* vtx) {
	SplitVtx* vtx2 = geoGridCtx->vtxHead;
	
	while (vtx2) {
		if (vtx2 == vtx) {
			vtx2 = vtx2->next;
			continue;
		}
		
		if (Vec2_Equal(&vtx->pos, &vtx2->pos)) {
			SplitVtx* kill = vtx2;
			Split* s = geoGridCtx->splitHead;
			SplitEdge* e = geoGridCtx->edgeHead;
			
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
			Node_Kill(geoGridCtx->vtxHead, kill);
			OsPrintfEx("" PRNT_YELW "Kill Dublicate Vtx!");
			continue;
		}
		
		vtx2 = vtx2->next;
	}
}

void GeoGrid_Update_Vtx(GeoGridContext* geoGridCtx) {
	SplitVtx* vtx = geoGridCtx->vtxHead;
	static s32 clean;
	
	if (geoGridCtx->actionEdge != NULL) {
		clean = true;
	}
	
	while (vtx) {
		if (clean == true && geoGridCtx->actionEdge == NULL) {
			GeoGrid_Update_Vtx_RemoveDublicates(geoGridCtx, vtx);
		}
		
		if (vtx->killFlag == true) {
			Split* s = geoGridCtx->splitHead;
			
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
				Node_Kill(geoGridCtx->vtxHead, killVtx);
				continue;
			}
		}
		
		vtx = vtx->next;
		if (vtx == NULL && geoGridCtx->actionEdge == NULL) {
			clean = false;
		}
	}
}

/* ───────────────────────────────────────────────────────────────────────── */

void GeoGrid_Update_Edge_RemoveDublicates(GeoGridContext* geoGridCtx, SplitEdge* edge) {
	SplitEdge* edge2 = geoGridCtx->edgeHead;
	
	while (edge2) {
		if (edge2 == edge) {
			edge2 = edge2->next;
			continue;
		}
		
		if (edge2->vtx[0] == edge->vtx[0] && edge2->vtx[1] == edge->vtx[1]) {
			SplitEdge* kill = edge2;
			Split* s = geoGridCtx->splitHead;
			
			if (geoGridCtx->actionEdge == edge2) {
				geoGridCtx->actionEdge = edge;
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
			Node_Kill(geoGridCtx->edgeHead, kill);
			OsPrintfEx("" PRNT_YELW "Kill Dublicate Edge!");
			continue;
		}
		
		edge2 = edge2->next;
	}
}

void GeoGrid_Update_Edge_SetSlide(GeoGridContext* geoGridCtx) {
	SplitEdge* edge = geoGridCtx->edgeHead;
	f64 diffCentX = (f64)geoGridCtx->workRect.w / geoGridCtx->prevWorkRect.w;
	f64 diffCentY = (f64)geoGridCtx->workRect.h / geoGridCtx->prevWorkRect.h;
	
	while (edge) {
		bool clampFail = false;
		bool isEditEdge = (edge == geoGridCtx->actionEdge || edge->state & EDGE_EDIT);
		bool isCornerEdge = ((edge->state & EDGE_STICK) != 0);
		bool isHor = ((edge->state & EDGE_VERTICAL) == 0);
		bool matchesActionAlign = (geoGridCtx->actionEdge && ((edge->state & EDGE_ALIGN) == (geoGridCtx->actionEdge->state & EDGE_ALIGN)));
		
		OsAssert(!(edge->state & EDGE_HORIZONTAL && edge->state & EDGE_VERTICAL));
		
		if (isCornerEdge) {
			if (edge->state & EDGE_STICK_L) {
				edge->pos = geoGridCtx->workRect.x;
			}
			if (edge->state & EDGE_STICK_T) {
				edge->pos = geoGridCtx->workRect.y;
			}
			if (edge->state & EDGE_STICK_R) {
				edge->pos = geoGridCtx->workRect.x + geoGridCtx->workRect.w;
			}
			if (edge->state & EDGE_STICK_B) {
				edge->pos = geoGridCtx->workRect.y + geoGridCtx->workRect.h;
			}
		} else {
			if (edge->state & EDGE_HORIZONTAL) {
				edge->pos *= diffCentY;
			}
			if (edge->state & EDGE_VERTICAL) {
				edge->pos *= diffCentX;
			}
		}
		
		if (isEditEdge && isCornerEdge == false) {
			SplitEdge* temp = geoGridCtx->edgeHead;
			s32 align = Lib_Wrap(edge->state & EDGE_ALIGN, 0, 1);
			
			while (temp) {
				for (s32 i = 0; i < 2; i++) {
					if (((temp->state & EDGE_ALIGN) != (edge->state & EDGE_ALIGN)) && temp->vtx[1] == edge->vtx[i]) {
						if (temp->vtx[0]->pos.s[align] > geoGridCtx->edgeMovement.clampMin) {
							geoGridCtx->edgeMovement.clampMin = temp->vtx[0]->pos.s[align];
							OsPrintfEx("foundMin %.2f", temp->vtx[0]->pos.s[align]);
						}
					}
				}
				
				for (s32 i = 0; i < 2; i++) {
					if (((temp->state & EDGE_ALIGN) != (edge->state & EDGE_ALIGN)) && temp->vtx[0] == edge->vtx[i]) {
						if (temp->vtx[1]->pos.s[align] < geoGridCtx->edgeMovement.clampMax) {
							geoGridCtx->edgeMovement.clampMax = temp->vtx[1]->pos.s[align];
							OsPrintfEx("foundMax %.2f", temp->vtx[1]->pos.s[align]);
						}
					}
				}
				
				temp = temp->next;
			}
			
			if (geoGridCtx->edgeMovement.clampMax - SPLIT_CLAMP > geoGridCtx->edgeMovement.clampMin + SPLIT_CLAMP) {
				if (edge->state & EDGE_HORIZONTAL) {
					edge->pos = __inputCtx->mouse.pos.y + 4;
				}
				if (edge->state & EDGE_VERTICAL) {
					edge->pos = __inputCtx->mouse.pos.x + 4;
				}
				edge->pos = CLAMP_MIN(edge->pos, geoGridCtx->edgeMovement.clampMin + SPLIT_CLAMP);
				edge->pos = CLAMP_MAX(edge->pos, geoGridCtx->edgeMovement.clampMax - SPLIT_CLAMP);
			} else {
				clampFail = true;
			}
		}
		
		if (isCornerEdge) {
			s32 revAlign = Lib_Wrap(isHor + 1, 0, 1);
			edge->vtx[0]->pos.s[isHor] = edge->pos;
			edge->vtx[1]->pos.s[isHor] = edge->pos;
		} else {
			if (isEditEdge && isCornerEdge == false && clampFail == false) {
				edge->vtx[0]->pos.s[isHor] = floor(edge->pos * 0.125f) * 8.0f;
				edge->vtx[1]->pos.s[isHor] = floor(edge->pos * 0.125f) * 8.0f;
				edge->pos = floor(edge->pos * 0.125f) * 8.0f;
			} else {
				edge->vtx[0]->pos.s[isHor] = edge->pos;
				edge->vtx[1]->pos.s[isHor] = edge->pos;
			}
		}
		
		edge = edge->next;
	}
}

void GeoGrid_Update_Edges(GeoGridContext* geoGridCtx) {
	SplitEdge* edge = geoGridCtx->edgeHead;
	f64 diffCentX = (f64)geoGridCtx->workRect.w / geoGridCtx->prevWorkRect.w;
	f64 diffCentY = (f64)geoGridCtx->workRect.h / geoGridCtx->prevWorkRect.h;
	
	if (!geoGridCtx->mouse->click.hold) {
		geoGridCtx->actionEdge = NULL;
	}
	
	while (edge) {
		if (edge->killFlag == true) {
			SplitEdge* temp = edge->next;
			OsPrintfEx("" PRNT_YELW "Kill Tagged Edge!");
			Node_Kill(geoGridCtx->edgeHead, edge);
			edge = temp;
			continue;
		}
		edge->killFlag = true;
		
		if (geoGridCtx->actionEdge == NULL) {
			edge->state &= ~EDGE_EDIT;
		}
		
		GeoGrid_Update_Edge_RemoveDublicates(geoGridCtx, edge);
		edge = edge->next;
	}
	
	GeoGrid_Update_Edge_SetSlide(geoGridCtx);
}

/* ───────────────────────────────────────────────────────────────────────── */

void GeoGrid_Update_ActionSplit(GeoGridContext* geoGridCtx) {
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
	Split* split = geoGridCtx->actionSplit;
	
	if (geoGridCtx->mouse->click.press) {
		SplitState tempStateA = GeoGrid_GetState_CursorPos(split, SPLIT_GRAB_DIST);
		SplitState tempStateB = GeoGrid_GetState_CursorPos(split, SPLIT_GRAB_DIST * 4);
		if (tempStateA & SPLIT_SIDES) {
			split->stateFlag |= tempStateA;
		}
		if (tempStateB & SPLIT_POINTS) {
			split->stateFlag |= tempStateB;
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
			
			geoGridCtx->actionEdge = split->edge[i];
			GeoGrid_Edge_SetSlideClamp(geoGridCtx);
			GeoGrid_Reset(geoGridCtx);
		}
	}
	
	if (geoGridCtx->mouse->click.hold) {
		if (split->stateFlag & SPLIT_POINTS) {
			s32 splitDist =
			    GeoGrid_Cursor_GetDistTo(split->stateFlag & SPLIT_POINTS, split);
			s32 dist = Vec_Vec2s_DistXZ(&split->mousePos, &split->mousePressPos);
			
			if (dist > 1) {
				CursorIndex cid = GeoGrid_GerDir_MouseToPressPos(split) + 1;
				Cursor_SetCursor(cid);
			}
			if (dist > SPLIT_CLAMP * 1.05) {
				GeoGrid_Reset(geoGridCtx);
				if (split->mouseInSplit) {
					GeoGrid_Split(geoGridCtx, split, GeoGrid_GerDir_MouseToPressPos(split));
				} else {
					GeoGrid_KillSplit(geoGridCtx, split, GeoGrid_GerDir_MouseToPressPos(split));
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

void GeoGrid_Update_Split(GeoGridContext* geoGridCtx) {
	Split* split = geoGridCtx->splitHead;
	MouseInput* mouse = geoGridCtx->mouse;
	
	Cursor_SetCursor(CURSOR_DEFAULT);
	
	if (geoGridCtx->actionSplit != NULL && mouse->cursorAction == false) {
		GeoGrid_Reset(geoGridCtx);
	}
	
	while (split) {
		GeoGrid_Update_SplitRect(split);
		Vec2s rectPos = { split->rect.x, split->rect.y };
		Vec_Vec2s_Substract(&split->mousePos, &mouse->pos, (Vec2s*)&rectPos);
		
		split->mouseInSplit = GeoGrid_Cursor_InSplit(split);
		split->center.x = split->rect.x + split->rect.w * 0.5f;
		split->center.y = split->rect.y + split->rect.h * 0.5f;
		
		if (geoGridCtx->actionSplit == NULL && split->mouseInSplit &&
		    mouse->cursorAction) {
			if (mouse->click.press) {
				split->mousePressPos = split->mousePos;
				geoGridCtx->actionSplit = split;
			}
		}
		
		if (geoGridCtx->actionSplit != NULL) {
			if (geoGridCtx->actionSplit == split)
				GeoGrid_Update_ActionSplit(geoGridCtx);
		} else {
			if (geoGridCtx->mouse->click.hold == 0) {
				if (GeoGrid_GetState_CursorPos(split, SPLIT_GRAB_DIST * 4) & SPLIT_POINTS &&
				    split->mouseInSplit) {
					Cursor_SetCursor(CURSOR_CROSSHAIR);
				} else if (GeoGrid_GetState_CursorPos(split, SPLIT_GRAB_DIST) & SPLIT_SIDE_H &&
				    split->mouseInSplit) {
					Cursor_SetCursor(CURSOR_ARROW_H);
				} else if (GeoGrid_GetState_CursorPos(split, SPLIT_GRAB_DIST) & SPLIT_SIDE_V &&
				    split->mouseInSplit) {
					Cursor_SetCursor(CURSOR_ARROW_V);
				}
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

void GeoGrid_Draw_Debug(GeoGridContext* geoGridCtx) {
	SplitVtx* vtx = geoGridCtx->vtxHead;
	Split* split = geoGridCtx->splitHead;
	s32 num = 0;
	Vec2s* winDim = geoGridCtx->winDim;
	
	glViewport(0, 0, winDim->x, winDim->y);
	nvgBeginFrame(geoGridCtx->vg, winDim->x, winDim->y, 1.0f); {
		while (split) {
			nvgBeginPath(geoGridCtx->vg);
			nvgLineCap(geoGridCtx->vg, NVG_ROUND);
			nvgStrokeWidth(geoGridCtx->vg, 0.8f);
			nvgMoveTo(
				geoGridCtx->vg,
				split->vtx[0]->pos.x + 2,
				split->vtx[0]->pos.y - 2
			);
			nvgLineTo(
				geoGridCtx->vg,
				split->vtx[1]->pos.x + 2,
				split->vtx[1]->pos.y + 2
			);
			nvgLineTo(
				geoGridCtx->vg,
				split->vtx[2]->pos.x - 2,
				split->vtx[2]->pos.y + 2
			);
			nvgLineTo(
				geoGridCtx->vg,
				split->vtx[3]->pos.x - 2,
				split->vtx[3]->pos.y - 2
			);
			nvgLineTo(
				geoGridCtx->vg,
				split->vtx[0]->pos.x + 2,
				split->vtx[0]->pos.y - 2
			);
			nvgStrokeColor(geoGridCtx->vg, nvgHSLA(0.111 * num, 1.0f, 0.4f, 255));
			nvgStroke(geoGridCtx->vg);
			
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
			nvgFontSize(geoGridCtx->vg, 16);
			nvgFontFace(geoGridCtx->vg, "sans");
			nvgTextAlign(geoGridCtx->vg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
			nvgFillColor(geoGridCtx->vg, Theme_GetColor(THEME_HEDR));
			nvgFontBlur(geoGridCtx->vg, 1.5f);
			nvgText(geoGridCtx->vg, pos.x, pos.y, buf, 0);
			nvgFontBlur(geoGridCtx->vg, 0);
			nvgFillColor(geoGridCtx->vg, Theme_GetColor(THEME_TEXT));
			nvgText(geoGridCtx->vg, pos.x, pos.y, buf, 0);
			
			vtx = vtx->next;
			num++;
		}
		
	} nvgEndFrame(geoGridCtx->vg);
}

void GeoGrid_Draw_SplitBorder(NVGcontext* vg, Split* split) {
	Rect* rect = &split->rect;
	Rectf32 adjRect = {
		0 + SPLIT_SPLIT_W,
		0 + SPLIT_SPLIT_W,
		rect->w - SPLIT_SPLIT_W * 2,
		rect->h - SPLIT_SPLIT_W * 2
	};
	
	if (split->edge[0]->state & EDGE_STICK_L) {
		adjRect.x += SPLIT_SPLIT_W * 0.25;
	}
	
	if (split->edge[1]->state & EDGE_STICK_T) {
		adjRect.y += SPLIT_SPLIT_W * 0.25;
	}
	
	if (split->edge[2]->state & EDGE_STICK_R) {
		adjRect.w -= SPLIT_SPLIT_W * 0.5;
	}
	
	if (split->edge[3]->state & EDGE_STICK_B) {
		adjRect.h -= SPLIT_SPLIT_W * 0.5;
	}
	
	nvgBeginPath(vg);
	nvgRect(vg, 0, 0, rect->w, rect->h);
	nvgPathWinding(vg, NVG_HOLE);
	nvgFillColor(vg, Theme_GetColor(THEME_SPBG));
	nvgFill(vg);
	
	if (split->update) {
		nvgEndFrame(vg);
		nvgBeginFrame(vg, split->rect.w, split->rect.h, 1.0f);
		split->update(split->passArg, split);
		nvgEndFrame(vg);
		nvgBeginFrame(vg, split->rect.w, split->rect.h, 1.0f);
	}
	
	nvgBeginPath(vg);
	nvgRect(vg, 0, rect->h - SPLIT_BAR_HEIGHT * 1.125, rect->w, rect->h);
	nvgPathWinding(vg, NVG_HOLE);
	nvgFillColor(vg, Theme_GetColor(THEME_BASE));
	nvgFill(vg);
	
	nvgBeginPath(vg);
	nvgRect(vg, 0, 0, rect->w, rect->h);
	nvgRoundedRect(
		vg,
		adjRect.x,
		adjRect.y,
		adjRect.w,
		adjRect.h,
		SPLIT_ROUND_R
	);
	nvgPathWinding(vg, NVG_HOLE);
	nvgFillColor(vg, Theme_GetColor(THEME_LINE));
	nvgFill(vg);
}

void GeoGrid_Draw_Splits(GeoGridContext* geoGridCtx) {
	Split* split = geoGridCtx->splitHead;
	Vec2s* winDim = geoGridCtx->winDim;
	
	while (split != NULL) {
		glViewport(
			split->rect.x,
			winDim->y - split->rect.y - split->rect.h,
			split->rect.w,
			split->rect.h
		);
		nvgBeginFrame(geoGridCtx->vg, split->rect.w, split->rect.h, 1.0f); {
			GeoGrid_Draw_SplitBorder(geoGridCtx->vg, split);
		} nvgEndFrame(geoGridCtx->vg);
		
		split = split->next;
	}
}

/* ───────────────────────────────────────────────────────────────────────── */

void GeoGrid_RemoveDublicates(GeoGridContext* geoGridCtx) {
	SplitVtx* vtx = geoGridCtx->vtxHead;
	SplitEdge* edge = geoGridCtx->edgeHead;
	
	while (vtx) {
		GeoGrid_Update_Vtx_RemoveDublicates(geoGridCtx, vtx);
		vtx = vtx->next;
	}
	
	while (edge) {
		GeoGrid_Update_Edge_RemoveDublicates(geoGridCtx, edge);
		edge = edge->next;
	}
}

void GeoGrid_UpdateWorkRect(GeoGridContext* geoGridCtx) {
	Vec2s* winDim = geoGridCtx->winDim;
	
	geoGridCtx->workRect = (Rect) { 0, 0 + geoGridCtx->bar[BAR_TOP].rect.h, winDim->x,
					winDim->y - geoGridCtx->bar[BAR_BOT].rect.h -
					geoGridCtx->bar[BAR_TOP].rect.h };
}

void GeoGrid_SetTopBarHeight(GeoGridContext* geoGridCtx, s32 h) {
	geoGridCtx->bar[BAR_TOP].rect.x = 0;
	geoGridCtx->bar[BAR_TOP].rect.y = 0;
	geoGridCtx->bar[BAR_TOP].rect.w = geoGridCtx->winDim->x;
	geoGridCtx->bar[BAR_TOP].rect.h = h;
	GeoGrid_UpdateWorkRect(geoGridCtx);
}

void GeoGrid_SetBotBarHeight(GeoGridContext* geoGridCtx, s32 h) {
	geoGridCtx->bar[BAR_BOT].rect.x = 0;
	geoGridCtx->bar[BAR_BOT].rect.y = geoGridCtx->winDim->y - h;
	geoGridCtx->bar[BAR_BOT].rect.w = geoGridCtx->winDim->x;
	geoGridCtx->bar[BAR_BOT].rect.h = h;
	GeoGrid_UpdateWorkRect(geoGridCtx);
}

void GeoGrid_Init(GeoGridContext* geoGridCtx, Vec2s* winDim, MouseInput* mouse, void* vg) {
	geoGridCtx->winDim = winDim;
	geoGridCtx->mouse = mouse;
	geoGridCtx->vg = vg;
	GeoGrid_SetTopBarHeight(geoGridCtx, SPLIT_BAR_HEIGHT);
	GeoGrid_SetBotBarHeight(geoGridCtx, SPLIT_BAR_HEIGHT);
	
	Rect lHalf = { geoGridCtx->workRect.x, geoGridCtx->workRect.y, geoGridCtx->workRect.w / 2,
		       geoGridCtx->workRect.h };
	Rect rHalf = { geoGridCtx->workRect.x + geoGridCtx->workRect.w / 2, geoGridCtx->workRect.y,
		       geoGridCtx->workRect.w / 2,                      geoGridCtx->workRect.h };
	
	GeoGrid_AddSplit(geoGridCtx, &lHalf);
	GeoGrid_AddSplit(geoGridCtx, &rHalf);
	
	geoGridCtx->prevWorkRect = geoGridCtx->workRect;
}

void GeoGrid_Update(GeoGridContext* geoGridCtx) {
	GeoGrid_SetTopBarHeight(geoGridCtx, geoGridCtx->bar[BAR_TOP].rect.h);
	GeoGrid_SetBotBarHeight(geoGridCtx, geoGridCtx->bar[BAR_BOT].rect.h);
	GeoGrid_Update_Vtx(geoGridCtx);
	GeoGrid_Update_Edges(geoGridCtx);
	GeoGrid_Update_Split(geoGridCtx);
	
	geoGridCtx->prevWorkRect = geoGridCtx->workRect;
}

void GeoGrid_Draw(GeoGridContext* geoGridCtx) {
	Vec2s* winDim = geoGridCtx->winDim;
	
	// Draw Bars
	for (s32 i = 0; i < 2; i++) {
		glViewport(
			geoGridCtx->bar[i].rect.x,
			winDim->y - geoGridCtx->bar[i].rect.y - geoGridCtx->bar[i].rect.h,
			geoGridCtx->bar[i].rect.w,
			geoGridCtx->bar[i].rect.h
		);
		nvgBeginFrame(geoGridCtx->vg, geoGridCtx->bar[i].rect.w, geoGridCtx->bar[i].rect.h, 1.0f); {
			nvgBeginPath(geoGridCtx->vg);
			nvgRect(
				geoGridCtx->vg,
				0,
				0,
				geoGridCtx->bar[i].rect.w,
				geoGridCtx->bar[i].rect.h
			);
			nvgFillColor(geoGridCtx->vg, Theme_GetColor(THEME_HEDR));
			nvgFill(geoGridCtx->vg);
			
			if (i == 1) {
				nvgFontSize(geoGridCtx->vg, SPLIT_BAR_HEIGHT - 12);
				nvgFontFace(geoGridCtx->vg, "sans");
				nvgTextAlign(geoGridCtx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
				
				nvgFillColor(geoGridCtx->vg, Theme_GetColor(THEME_SHDW));
				nvgFontBlur(geoGridCtx->vg, 2.0f);
				nvgText(
					geoGridCtx->vg,
					6,
					6,
					gBuild,
					NULL
				);
				
				nvgFillColor(geoGridCtx->vg, Theme_GetColor(THEME_TEXT));
				nvgFontBlur(geoGridCtx->vg, 0.0f);
				nvgText(
					geoGridCtx->vg,
					6,
					6,
					gBuild,
					NULL
				);
			}
		} nvgEndFrame(geoGridCtx->vg);
	}
	
	GeoGrid_Draw_Splits(geoGridCtx);
	// GeoGrid_Draw_Debug(geoGridCtx);
}