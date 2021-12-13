#include "Editor.h"

char* Debug_GeoGui_GetStateStrings(GeoSplit* split) {
	static char buffer[1024];
	s32 t = 0;
	char* states[] = {
		"NONE",     "POINT_BL",
		"POINT_TL", "POINT_TR",
		"POINT_BR", "SIDE_L",
		"SIDE_T",   "SIDE_R",
		"SIDE_B",
	};
	
	sprintf(buffer, "%5d %5d  ", __appInfo->winDim.x, __appInfo->winDim.y);
	
	for (s32 i = 0; (1 << i) <= 0xFFFF; i++) {
		if (split->stateFlag & (1 << i)) {
			if (t++ == 0) {
				String_Merge(buffer, states[i + 1]);
			} else {
				String_Merge(buffer, "|");
				String_Merge(buffer, states[i + 1]);
			}
		}
	}
	
	return buffer;
}

GeoDir GeoGui_GetDir_Opposite(GeoDir dir) {
	return Lib_Wrap(dir + 2, DIR_L, DIR_B);
}

GeoDir GeoGui_GerDir_MouseToPressPos(GeoSplit* split) {
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

GeoVtx* GeoGui_AddVtx(GeoGuiContext* geoCtx, f64 x, f64 y) {
	GeoVtx* head = geoCtx->vtxHead;
	
	while (head) {
		if (ABS(head->pos.x - x) <= 1.0 && ABS(head->pos.y - y) <= 1.0) {
			OsPrintfEx("VtxConnect %.2f %.2f", x, y);
			
			return head;
		}
		head = head->next;
	}
	
	GeoVtx* vtx = Lib_Calloc(0, sizeof(GeoVtx));
	
	vtx->pos.x = x;
	vtx->pos.y = y;
	Node_Add(geoCtx->vtxHead, vtx);
	
	return vtx;
}

GeoEdge* GeoGui_AddEdge(GeoGuiContext* geoCtx, GeoVtx* v1, GeoVtx* v2) {
	GeoEdge* head = geoCtx->edgeHead;
	GeoEdge* edge = NULL;
	
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
		edge = Lib_Calloc(0, sizeof(GeoEdge));
		
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

s32 GeoGui_Cursor_GetDistTo(SplitState flag, GeoSplit* split) {
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

SplitState GeoGui_GetState_CursorPos(GeoSplit* split, s32 range) {
	for (s32 i = 0; (1 << i) <= SPLIT_SIDE_B; i++) {
		if (GeoGui_Cursor_GetDistTo((1 << i), split) <= range) {
			return (1 << i);
		}
	}
	
	return SPLIT_POINT_NONE;
}

bool GeoGui_Cursor_InSplit(GeoSplit* split) {
	s32 resX = (split->mousePos.x < split->rect.w && split->mousePos.x >= 0);
	s32 resY = (split->mousePos.y < split->rect.h && split->mousePos.y >= 0);
	
	return (resX && resY);
}

void GeoGui_AddSplit(GeoGuiContext* geoCtx, Rect* rect) {
	GeoSplit* split = Lib_Calloc(0, sizeof(GeoSplit));
	
	split->vtx[VTX_BOT_L] = GeoGui_AddVtx(geoCtx, rect->x, rect->y + rect->h);
	split->vtx[VTX_TOP_L] = GeoGui_AddVtx(geoCtx, rect->x, rect->y);
	split->vtx[VTX_TOP_R] = GeoGui_AddVtx(geoCtx, rect->x + rect->w, rect->y);
	split->vtx[VTX_BOT_R] =
	    GeoGui_AddVtx(geoCtx, rect->x + rect->w, rect->y + rect->h);
	
	split->edge[EDGE_L] =
	    GeoGui_AddEdge(geoCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_TOP_L]);
	split->edge[EDGE_T] =
	    GeoGui_AddEdge(geoCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
	split->edge[EDGE_R] =
	    GeoGui_AddEdge(geoCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	split->edge[EDGE_B] =
	    GeoGui_AddEdge(geoCtx, split->vtx[VTX_BOT_R], split->vtx[VTX_BOT_L]);
	
	Node_Add(geoCtx->splitHead, split);
}

void GeoGui_Edge_SetSlideClamp(GeoGuiContext* geoCtx) {
	GeoEdge* tempEdge = geoCtx->edgeHead;
	GeoEdge* setEdgeA = geoCtx->actionEdge;
	GeoEdge* setEdgeB = geoCtx->edgeHead;
	
	setEdgeA->state |= EDGE_EDIT;
	
	// Get edge with vtx closest to TOPLEFT
	while (tempEdge) {
		if ((tempEdge->state & EDGE_ALIGN) == (setEdgeA->state & EDGE_ALIGN)) {
			if (tempEdge->vtx[1] == setEdgeA->vtx[0]) {
				setEdgeA = tempEdge;
				tempEdge->state |= EDGE_EDIT;
				tempEdge = geoCtx->edgeHead;
				continue;
			}
		}
		
		tempEdge = tempEdge->next;
	}
	
	// Get sibling edge that does not connect with setEdgeA
	while (setEdgeB) {
		if ((setEdgeB->state & EDGE_ALIGN) == (setEdgeA->state & EDGE_ALIGN)) {
			if (setEdgeB->vtx[0] == setEdgeA->vtx[0] && setEdgeB->vtx[1] != setEdgeA->vtx[1]) {
				setEdgeB->state |= EDGE_EDIT;
				break;
			}
		}
		
		setEdgeB = setEdgeB->next;
	}
	
	tempEdge = geoCtx->edgeHead;
	
	// Set all below setEdgeA
	while (tempEdge) {
		if ((tempEdge->state & EDGE_ALIGN) == (setEdgeA->state & EDGE_ALIGN)) {
			if (tempEdge->vtx[0] == setEdgeA->vtx[1]) {
				tempEdge->state |= EDGE_EDIT;
				setEdgeA = tempEdge;
				tempEdge = geoCtx->edgeHead;
				continue;
			}
		}
		
		tempEdge = tempEdge->next;
	}
	
	tempEdge = geoCtx->edgeHead;
	
	// Set all below setEdgeB
	if (setEdgeB) {
		while (tempEdge) {
			if ((tempEdge->state & EDGE_ALIGN) == (setEdgeB->state & EDGE_ALIGN)) {
				if (tempEdge->vtx[0] == setEdgeB->vtx[1]) {
					tempEdge->state |= EDGE_EDIT;
					setEdgeB = tempEdge;
					tempEdge = geoCtx->edgeHead;
					continue;
				}
			}
			
			tempEdge = tempEdge->next;
		}
	}
	
	geoCtx->edgeMovement.clampMin = geoCtx->workRect.y;
	geoCtx->edgeMovement.clampMax = geoCtx->workRect.w;
}

void GeoGui_Reset(GeoGuiContext* geoCtx) {
	OsAssert(geoCtx->actionSplit);
	geoCtx->actionSplit->stateFlag &= ~(SPLIT_POINTS | SPLIT_SIDES);
	
	geoCtx->actionSplit = NULL;
}

void GeoGui_Split(GeoGuiContext* geoCtx, GeoSplit* split, GeoDir dir) {
	GeoSplit* partner;
	GeoSplit* newSplit;
	GeoDir mirDir = GeoGui_GetDir_Opposite(dir);
	s16 splitPos = (dir == DIR_L || dir == DIR_R) ? __inputCtx->mouse.pos.x : __inputCtx->mouse.pos.y;
	
	splitPos = CLAMP(splitPos - split->edge[mirDir]->pos, -SPLIT_CLAMP * 0.90f, SPLIT_CLAMP * 0.90f);
	splitPos += split->edge[mirDir]->pos;
	
	newSplit = Lib_Calloc(0, sizeof(GeoSplit));
	
	Node_Add(geoCtx->splitHead, newSplit);
	
	OsPrintfEx("GeoDir: %d", dir);
	
	if (dir == DIR_L) {
		newSplit->vtx[0] = GeoGui_AddVtx(geoCtx, splitPos, split->vtx[0]->pos.y);
		newSplit->vtx[1] = GeoGui_AddVtx(geoCtx, splitPos, split->vtx[1]->pos.y);
		newSplit->vtx[2] =
		    GeoGui_AddVtx(geoCtx, split->vtx[2]->pos.x, split->vtx[2]->pos.y);
		newSplit->vtx[3] =
		    GeoGui_AddVtx(geoCtx, split->vtx[3]->pos.x, split->vtx[3]->pos.y);
		split->vtx[2] = GeoGui_AddVtx(geoCtx, splitPos, split->vtx[2]->pos.y);
		split->vtx[3] = GeoGui_AddVtx(geoCtx, splitPos, split->vtx[3]->pos.y);
	}
	
	if (dir == DIR_R) {
		newSplit->vtx[0] =
		    GeoGui_AddVtx(geoCtx, split->vtx[0]->pos.x, split->vtx[0]->pos.y);
		newSplit->vtx[1] =
		    GeoGui_AddVtx(geoCtx, split->vtx[1]->pos.x, split->vtx[1]->pos.y);
		newSplit->vtx[2] = GeoGui_AddVtx(geoCtx, splitPos, split->vtx[2]->pos.y);
		newSplit->vtx[3] = GeoGui_AddVtx(geoCtx, splitPos, split->vtx[3]->pos.y);
		split->vtx[0] = GeoGui_AddVtx(geoCtx, splitPos, split->vtx[0]->pos.y);
		split->vtx[1] = GeoGui_AddVtx(geoCtx, splitPos, split->vtx[1]->pos.y);
	}
	
	if (dir == DIR_T) {
		newSplit->vtx[0] =
		    GeoGui_AddVtx(geoCtx, split->vtx[0]->pos.x, split->vtx[0]->pos.y);
		newSplit->vtx[1] = GeoGui_AddVtx(geoCtx, split->vtx[1]->pos.x, splitPos);
		newSplit->vtx[2] = GeoGui_AddVtx(geoCtx, split->vtx[2]->pos.x, splitPos);
		newSplit->vtx[3] =
		    GeoGui_AddVtx(geoCtx, split->vtx[3]->pos.x, split->vtx[3]->pos.y);
		split->vtx[3] = GeoGui_AddVtx(geoCtx, split->vtx[3]->pos.x, splitPos);
		split->vtx[0] = GeoGui_AddVtx(geoCtx, split->vtx[0]->pos.x, splitPos);
	}
	
	if (dir == DIR_B) {
		newSplit->vtx[0] = GeoGui_AddVtx(geoCtx, split->vtx[0]->pos.x, splitPos);
		newSplit->vtx[1] =
		    GeoGui_AddVtx(geoCtx, split->vtx[1]->pos.x, split->vtx[1]->pos.y);
		newSplit->vtx[2] =
		    GeoGui_AddVtx(geoCtx, split->vtx[2]->pos.x, split->vtx[2]->pos.y);
		newSplit->vtx[3] = GeoGui_AddVtx(geoCtx, split->vtx[3]->pos.x, splitPos);
		split->vtx[1] = GeoGui_AddVtx(geoCtx, split->vtx[1]->pos.x, splitPos);
		split->vtx[2] = GeoGui_AddVtx(geoCtx, split->vtx[2]->pos.x, splitPos);
	}
	
	split->edge[EDGE_L] =
	    GeoGui_AddEdge(geoCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_TOP_L]);
	split->edge[EDGE_T] =
	    GeoGui_AddEdge(geoCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
	split->edge[EDGE_R] =
	    GeoGui_AddEdge(geoCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	split->edge[EDGE_B] =
	    GeoGui_AddEdge(geoCtx, split->vtx[VTX_BOT_R], split->vtx[VTX_BOT_L]);
	
	newSplit->edge[EDGE_L] =
	    GeoGui_AddEdge(geoCtx, newSplit->vtx[VTX_BOT_L], newSplit->vtx[VTX_TOP_L]);
	newSplit->edge[EDGE_T] =
	    GeoGui_AddEdge(geoCtx, newSplit->vtx[VTX_TOP_L], newSplit->vtx[VTX_TOP_R]);
	newSplit->edge[EDGE_R] =
	    GeoGui_AddEdge(geoCtx, newSplit->vtx[VTX_TOP_R], newSplit->vtx[VTX_BOT_R]);
	newSplit->edge[EDGE_B] =
	    GeoGui_AddEdge(geoCtx, newSplit->vtx[VTX_BOT_R], newSplit->vtx[VTX_BOT_L]);
	
	geoCtx->actionEdge = newSplit->edge[dir];
	GeoGui_Edge_SetSlideClamp(geoCtx);
}

void GeoGui_KillSplit(GeoGuiContext* geoCtx, GeoSplit* split, GeoDir dir) {
	GeoEdge* sharedEdge = split->edge[dir];
	GeoSplit* killSplit = geoCtx->splitHead;
	GeoDir oppositeDir = GeoGui_GetDir_Opposite(dir);
	
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
		split->edge[EDGE_T] =
		    GeoGui_AddEdge(geoCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
		split->edge[EDGE_B] =
		    GeoGui_AddEdge(geoCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_BOT_R]);
	}
	
	if (dir == DIR_T) {
		split->vtx[VTX_TOP_L] = killSplit->vtx[VTX_TOP_L];
		split->vtx[VTX_TOP_R] = killSplit->vtx[VTX_TOP_R];
		split->edge[EDGE_L] =
		    GeoGui_AddEdge(geoCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_BOT_L]);
		split->edge[EDGE_R] =
		    GeoGui_AddEdge(geoCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	}
	
	if (dir == DIR_R) {
		split->vtx[VTX_BOT_R] = killSplit->vtx[VTX_BOT_R];
		split->vtx[VTX_TOP_R] = killSplit->vtx[VTX_TOP_R];
		split->edge[EDGE_T] =
		    GeoGui_AddEdge(geoCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
		split->edge[EDGE_B] =
		    GeoGui_AddEdge(geoCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_BOT_R]);
	}
	
	if (dir == DIR_B) {
		split->vtx[VTX_BOT_L] = killSplit->vtx[VTX_BOT_L];
		split->vtx[VTX_BOT_R] = killSplit->vtx[VTX_BOT_R];
		split->edge[EDGE_L] =
		    GeoGui_AddEdge(geoCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_BOT_L]);
		split->edge[EDGE_R] =
		    GeoGui_AddEdge(geoCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	}
	
	split->edge[dir] = killSplit->edge[dir];
	
	Node_Kill(geoCtx->splitHead, killSplit);
}

/* ───────────────────────────────────────────────────────────────────────── */

void GeoGui_Update_Vtx_RemoveDublicates(GeoGuiContext* geoCtx, GeoVtx* vtx) {
	GeoVtx* vtx2 = geoCtx->vtxHead;
	
	while (vtx2) {
		if (vtx2 == vtx) {
			vtx2 = vtx2->next;
			continue;
		}
		
		if (Vec2_Equal(&vtx->pos, &vtx2->pos)) {
			GeoVtx* kill = vtx2;
			GeoSplit* s = geoCtx->splitHead;
			GeoEdge* e = geoCtx->edgeHead;
			
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
			OsPrintfEx("" PRNT_YELW "\aKill Dublicate Vtx!");
			continue;
		}
		
		vtx2 = vtx2->next;
	}
}

void GeoGui_Update_Vtx(GeoGuiContext* geoCtx) {
	GeoVtx* vtx = geoCtx->vtxHead;
	static s32 clean;
	
	if (geoCtx->actionEdge != NULL) {
		clean = true;
	}
	
	while (vtx) {
		if (clean == true && geoCtx->actionEdge == NULL) {
			GeoGui_Update_Vtx_RemoveDublicates(geoCtx, vtx);
		}
		
		if (vtx->killFlag == true) {
			GeoSplit* s = geoCtx->splitHead;
			
			while (s) {
				for (s32 i = 0; i < 4; i++) {
					if (s->vtx[i] == vtx) {
						vtx->killFlag = false;
					}
				}
				s = s->next;
			}
			
			if (vtx->killFlag == true) {
				GeoVtx* killVtx = vtx;
				vtx = vtx->prev;
				OsPrintfEx("" PRNT_YELW "\aKill Tagged Vtx!");
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

void GeoGui_Update_Edge_RemoveDublicates(GeoGuiContext* geoCtx, GeoEdge* edge) {
	GeoEdge* edge2 = geoCtx->edgeHead;
	
	while (edge2) {
		if (edge2 == edge) {
			edge2 = edge2->next;
			continue;
		}
		
		if (edge2->vtx[0] == edge->vtx[0] && edge2->vtx[1] == edge->vtx[1]) {
			GeoEdge* kill = edge2;
			GeoSplit* s = geoCtx->splitHead;
			
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
			OsPrintfEx("" PRNT_YELW "\aKill Dublicate Edge!");
			continue;
		}
		
		edge2 = edge2->next;
	}
}

void GeoGui_Update_Edges(GeoGuiContext* geoCtx) {
	GeoEdge* edge = geoCtx->edgeHead;
	f64 diffCentX = (f64)geoCtx->workRect.w / geoCtx->prevWorkRect.w;
	f64 diffCentY = (f64)geoCtx->workRect.h / geoCtx->prevWorkRect.h;
	
	if (!__inputCtx->mouse.click.hold) {
		geoCtx->actionEdge = NULL;
	}
	
	while (edge) {
		if (edge->killFlag == true) {
			GeoEdge* temp = edge->next;
			OsPrintfEx("" PRNT_YELW "\aKill Tagged Edge!");
			Node_Kill(geoCtx->edgeHead, edge);
			edge = temp;
			continue;
		}
		edge->killFlag = true;
		
		if (geoCtx->actionEdge == NULL) {
			edge->state &= ~EDGE_EDIT;
		}
		
		GeoGui_Update_Edge_RemoveDublicates(geoCtx, edge);
		edge = edge->next;
	}
	
	edge = geoCtx->edgeHead;
	
	while (edge) {
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
				edge->pos *= diffCentY;
			}
			if (edge->state & EDGE_VERTICAL) {
				edge->pos *= diffCentX;
			}
		}
		
		if (isEditEdge && !isCornerEdge) {
			GeoEdge* temp = geoCtx->edgeHead;
			s32 align = Lib_Wrap(edge->state & EDGE_ALIGN, 0, 1);
			
			while (temp) {
				for (s32 i = 0; i < 2; i++) {
					if (((temp->state & EDGE_ALIGN) != (edge->state & EDGE_ALIGN)) && temp->vtx[1] == edge->vtx[i]) {
						if (temp->vtx[0]->pos.s[align] > geoCtx->edgeMovement.clampMin) {
							geoCtx->edgeMovement.clampMin = temp->vtx[0]->pos.s[align];
							OsPrintfEx("foundMin %.2f", temp->vtx[0]->pos.s[align]);
						}
					}
				}
				
				for (s32 i = 0; i < 2; i++) {
					if (((temp->state & EDGE_ALIGN) != (edge->state & EDGE_ALIGN)) && temp->vtx[0] == edge->vtx[i]) {
						if (temp->vtx[1]->pos.s[align] < geoCtx->edgeMovement.clampMax) {
							geoCtx->edgeMovement.clampMax = temp->vtx[1]->pos.s[align];
							OsPrintfEx("foundMax %.2f", temp->vtx[1]->pos.s[align]);
						}
					}
				}
				
				temp = temp->next;
			}
			
			if (edge->state & EDGE_HORIZONTAL) {
				edge->pos = __inputCtx->mouse.pos.y + 4;
			}
			if (edge->state & EDGE_VERTICAL) {
				edge->pos = __inputCtx->mouse.pos.x + 4;
			}
			edge->pos = CLAMP_MIN(edge->pos, geoCtx->edgeMovement.clampMin + SPLIT_CLAMP);
			edge->pos = CLAMP_MAX(edge->pos, geoCtx->edgeMovement.clampMax - SPLIT_CLAMP);
		}
		
		if (isCornerEdge) {
			s32 revAlign = Lib_Wrap(isHor + 1, 0, 1);
			edge->vtx[0]->pos.s[isHor] = edge->pos;
			edge->vtx[1]->pos.s[isHor] = edge->pos;
		} else {
			if (isEditEdge && !isCornerEdge) {
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

/* ───────────────────────────────────────────────────────────────────────── */

void GeoGui_Update_ActionSplit(GeoGuiContext* geoCtx) {
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
	GeoSplit* split = geoCtx->actionSplit;
	
	if (__inputCtx->mouse.click.press) {
		split->stateFlag |= GeoGui_GetState_CursorPos(split, 20);
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
			OsPrintfEx("GeoSplit: %s", stringDirection[i]);
			
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
			GeoGui_Edge_SetSlideClamp(geoCtx);
			GeoGui_Reset(geoCtx);
		}
	}
	
	if (__inputCtx->mouse.click.hold) {
		if (split->stateFlag & SPLIT_POINTS) {
			s32 splitDist =
			    GeoGui_Cursor_GetDistTo(split->stateFlag & SPLIT_POINTS, split);
			s32 dist = Vec_Vec2s_DistXZ(&split->mousePos, &split->mousePressPos);
			
			if (dist > 1) {
				CursorIndex cid = GeoGui_GerDir_MouseToPressPos(split) + 1;
				Cursor_SetCursor(cid);
			}
			if (dist > SPLIT_CLAMP * 1.25) {
				GeoGui_Reset(geoCtx);
				if (split->mouseInRegion) {
					GeoGui_Split(geoCtx, split, GeoGui_GerDir_MouseToPressPos(split));
				} else {
					GeoGui_KillSplit(geoCtx, split, GeoGui_GerDir_MouseToPressPos(split));
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

void GeoGui_Update_SplitRect(GeoSplit* split) {
	split->rect = (Rect) {
		floor(split->vtx[1]->pos.x),
		floor(split->vtx[1]->pos.y),
		floor(split->vtx[3]->pos.x) - floor(split->vtx[1]->pos.x),
		floor(split->vtx[3]->pos.y) - floor(split->vtx[1]->pos.y)
	};
}

void GeoGui_Update_Split(EditorContext* editorCtx) {
	GeoGuiContext* geoCtx = &editorCtx->geoCtx;
	GeoSplit* split = geoCtx->splitHead;
	MouseInput* mouse = &editorCtx->inputCtx.mouse;
	
	Cursor_SetCursor(CURSOR_DEFAULT);
	
	if (geoCtx->actionSplit != NULL && mouse->cursorAction == false) {
		GeoGui_Reset(geoCtx);
	}
	
	while (split) {
		GeoGui_Update_SplitRect(split);
		Vec2s rectPos = { split->rect.x, split->rect.y };
		Vec_Vec2s_Substract(&split->mousePos, &mouse->pos, (Vec2s*)&rectPos);
		
		split->mouseInRegion = GeoGui_Cursor_InSplit(split);
		split->center.x = split->rect.x + split->rect.w * 0.5f;
		split->center.y = split->rect.y + split->rect.h * 0.5f;
		
		if (geoCtx->actionSplit == NULL && split->mouseInRegion &&
		    mouse->cursorAction) {
			if (mouse->click.press) {
				split->mousePressPos = split->mousePos;
				geoCtx->actionSplit = split;
			}
		}
		
		if (geoCtx->actionSplit != NULL) {
			if (geoCtx->actionSplit == split)
				GeoGui_Update_ActionSplit(geoCtx);
		} else {
			if (__inputCtx->mouse.click.hold == 0) {
				if (GeoGui_GetState_CursorPos(split, 20) & SPLIT_POINTS &&
				    split->mouseInRegion) {
					Cursor_SetCursor(CURSOR_CROSSHAIR);
				} else if (GeoGui_GetState_CursorPos(split, 10) & SPLIT_SIDE_H &&
				    split->mouseInRegion) {
					Cursor_SetCursor(CURSOR_ARROW_H);
				} else if (GeoGui_GetState_CursorPos(split, 10) & SPLIT_SIDE_V &&
				    split->mouseInRegion) {
					Cursor_SetCursor(CURSOR_ARROW_V);
				}
			}
		}
		
		if (split->update) {
			if (split != geoCtx->splitHead &&
			    ((geoCtx->actionSplit && geoCtx->actionSplit == split) ||
			    geoCtx->actionSplit == NULL)) {
				split->update(editorCtx, split);
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

void GeoGui_Draw_SplitBorder(NVGcontext* vg, Rect* rect, s32 iter) {
	nvgBeginPath(vg);
	nvgRect(vg, 0, 0, rect->w, rect->h);
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
	
	#ifndef NDEBUG
	nvgCircle(vg, rect->w * 0.5, rect->h * 0.5, 25.0f);
	char buf[16] = { 0 };
	
	sprintf(buf, "%d", iter + 1);
	nvgFillColor(vg, nvgHSLA((f32)iter * 0.111f, 1.0f, 0.40f, 50));
	#endif
	nvgFill(vg);
	
	#ifndef NDEBUG
	nvgFillColor(vg, Theme_GetColor(THEME_BASE_CONT));
	nvgFontSize(vg, 30 - 16);
	nvgFontFace(vg, "sans");
	nvgTextAlign(vg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
	nvgText(vg, rect->w * 0.5, rect->h * 0.5, buf, 0);
	#endif
}

void GeoGui_Draw_Splits(EditorContext* editorCtx) {
	GeoGuiContext* geoCtx = &editorCtx->geoCtx;
	GeoSplit* split = geoCtx->splitHead;
	Vec2s* winDim = &editorCtx->appInfo.winDim;
	s32 iter = 0;
	
	while (split != NULL) {
		glViewport(
			split->rect.x,
			winDim->y - split->rect.y - split->rect.h,
			split->rect.w,
			split->rect.h
		);
		nvgBeginFrame(editorCtx->vg, split->rect.w, split->rect.h, 1.0f);
		{
			if (split->draw)
				split->draw(editorCtx, split);
			
			GeoGui_Draw_SplitBorder(editorCtx->vg, &split->rect, iter);
		}
		nvgEndFrame(editorCtx->vg);
		
		split = split->next;
		iter++;
	}
}

/* ───────────────────────────────────────────────────────────────────────── */

void GeoGui_UpdateWorkRegion(EditorContext* editorCtx) {
	GeoGuiContext* geoCtx = &editorCtx->geoCtx;
	Vec2s* winDim = &editorCtx->appInfo.winDim;
	
	geoCtx->workRect = (Rect) { 0, 0 + geoCtx->bar[GUI_BAR_TOP].rect.h, winDim->x,
				    winDim->y - geoCtx->bar[GUI_BAR_BOT].rect.h -
				    geoCtx->bar[GUI_BAR_TOP].rect.h };
}

void GeoGui_SetTopBarHeight(EditorContext* editorCtx, s32 h) {
	GeoGuiContext* geoCtx = &editorCtx->geoCtx;
	Vec2s* winDim = &editorCtx->appInfo.winDim;
	
	geoCtx->bar[GUI_BAR_TOP].rect.x = 0;
	geoCtx->bar[GUI_BAR_TOP].rect.y = 0;
	geoCtx->bar[GUI_BAR_TOP].rect.w = winDim->x;
	geoCtx->bar[GUI_BAR_TOP].rect.h = h;
	GeoGui_UpdateWorkRegion(editorCtx);
}

void GeoGui_SetBotBarHeight(EditorContext* editorCtx, s32 h) {
	GeoGuiContext* geoCtx = &editorCtx->geoCtx;
	Vec2s* winDim = &editorCtx->appInfo.winDim;
	
	geoCtx->bar[GUI_BAR_BOT].rect.x = 0;
	geoCtx->bar[GUI_BAR_BOT].rect.y = winDim->y - h;
	geoCtx->bar[GUI_BAR_BOT].rect.w = winDim->x;
	geoCtx->bar[GUI_BAR_BOT].rect.h = h;
	GeoGui_UpdateWorkRegion(editorCtx);
}

void GeoGui_Init(EditorContext* editorCtx) {
	GeoGuiContext* geoCtx = &editorCtx->geoCtx;
	Vec2s* winDim = &editorCtx->appInfo.winDim;
	
	GeoGui_SetTopBarHeight(editorCtx, 30);
	GeoGui_SetBotBarHeight(editorCtx, 30);
	
	Rect lHalf = { geoCtx->workRect.x, geoCtx->workRect.y, geoCtx->workRect.w / 2,
		       geoCtx->workRect.h };
	Rect rHalf = { geoCtx->workRect.x + geoCtx->workRect.w / 2, geoCtx->workRect.y,
		       geoCtx->workRect.w / 2,                      geoCtx->workRect.h };
	
	GeoGui_AddSplit(geoCtx, &lHalf);
	GeoGui_AddSplit(geoCtx, &rHalf);
	
	geoCtx->prevWorkRect = geoCtx->workRect;
}

void GeoGui_Update(EditorContext* editorCtx) {
	GeoGuiContext* geoCtx = &editorCtx->geoCtx;
	
	GeoGui_SetTopBarHeight(editorCtx, geoCtx->bar[GUI_BAR_TOP].rect.h);
	GeoGui_SetBotBarHeight(editorCtx, geoCtx->bar[GUI_BAR_BOT].rect.h);
	GeoGui_Update_Vtx(geoCtx);
	GeoGui_Update_Edges(geoCtx);
	GeoGui_Update_Split(editorCtx);
	
	geoCtx->prevWorkRect = geoCtx->workRect;
}

void GeoGui_Draw(EditorContext* editorCtx) {
	GeoGuiContext* geoCtx = &editorCtx->geoCtx;
	Vec2s* winDim = &editorCtx->appInfo.winDim;
	
	// Draw Bars
	for (s32 i = 0; i < 2; i++) {
		glViewport(
			geoCtx->bar[i].rect.x,
			winDim->y - geoCtx->bar[i].rect.y - geoCtx->bar[i].rect.h,
			geoCtx->bar[i].rect.w,
			geoCtx->bar[i].rect.h
		);
		nvgBeginFrame(
			editorCtx->vg,
			geoCtx->bar[i].rect.w,
			geoCtx->bar[i].rect.h,
			1.0f
		);
		{
			nvgBeginPath(editorCtx->vg);
			nvgRect(
				editorCtx->vg,
				0,
				0,
				geoCtx->bar[i].rect.w,
				geoCtx->bar[i].rect.h
			);
			nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_SPLITTER_DARKER));
			nvgFill(editorCtx->vg);
			
			if (i == 1) {
				static GeoSplit* lastActive;
				
				if (lastActive == NULL) {
					lastActive = geoCtx->splitHead;
				}
				
				if (geoCtx->actionSplit) {
					lastActive = geoCtx->actionSplit;
				}
				
				nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_BASE_CONT));
				nvgFontSize(editorCtx->vg, 30 - 16);
				nvgFontFace(editorCtx->vg, "sans");
				nvgTextAlign(editorCtx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
				nvgText(
					editorCtx->vg,
					8,
					8,
					Debug_GeoGui_GetStateStrings(lastActive),
					NULL
				);
			}
		}
		nvgEndFrame(editorCtx->vg);
	}
	
	GeoGui_Draw_Splits(editorCtx);
	
	#ifndef NDEBUG
	if (geoCtx->actionEdge != NULL) {
		GeoEdge* edge = geoCtx->actionEdge;
		glViewport(0, 0, editorCtx->appInfo.winDim.x, editorCtx->appInfo.winDim.y);
		
		nvgBeginFrame(
			editorCtx->vg,
			editorCtx->appInfo.winDim.x,
			editorCtx->appInfo.winDim.y,
			1.0f
		);
		{
			nvgBeginPath(editorCtx->vg);
			nvgLineCap(editorCtx->vg, NVG_ROUND);
			nvgStrokeWidth(editorCtx->vg, 2.36f);
			nvgMoveTo(editorCtx->vg, edge->vtx[0]->pos.x, edge->vtx[0]->pos.y);
			nvgLineTo(editorCtx->vg, edge->vtx[1]->pos.x, edge->vtx[1]->pos.y);
			nvgStrokeColor(editorCtx->vg, nvgRGBA(255, 0, 0, 125));
			nvgStroke(editorCtx->vg);
		}
		nvgEndFrame(editorCtx->vg);
	}
	
	GeoVtx* vtx = geoCtx->vtxHead;
	GeoSplit* split = geoCtx->splitHead;
	s32 num = 0;
	glViewport(0, 0, editorCtx->appInfo.winDim.x, editorCtx->appInfo.winDim.y);
	nvgBeginFrame(
		editorCtx->vg,
		editorCtx->appInfo.winDim.x,
		editorCtx->appInfo.winDim.y,
		1.0f
	);
	
	while (split) {
		nvgBeginPath(editorCtx->vg);
		nvgLineCap(editorCtx->vg, NVG_ROUND);
		nvgStrokeWidth(editorCtx->vg, 2.5f);
		nvgMoveTo(
			editorCtx->vg,
			split->vtx[0]->pos.x + 4,
			split->vtx[0]->pos.y - 4
		);
		nvgLineTo(
			editorCtx->vg,
			split->vtx[1]->pos.x + 4,
			split->vtx[1]->pos.y + 4
		);
		nvgLineTo(
			editorCtx->vg,
			split->vtx[2]->pos.x - 4,
			split->vtx[2]->pos.y + 4
		);
		nvgLineTo(
			editorCtx->vg,
			split->vtx[3]->pos.x - 4,
			split->vtx[3]->pos.y - 4
		);
		nvgLineTo(
			editorCtx->vg,
			split->vtx[0]->pos.x + 4,
			split->vtx[0]->pos.y - 4
		);
		nvgStrokeColor(editorCtx->vg, nvgHSLA(0.111 * num, 1.0f, 0.4f, 255));
		nvgStroke(editorCtx->vg);
		
		split = split->next;
		num++;
	}
	
	num = 0;
	
	while (vtx) {
		char buf[128];
		Vec2f pos = {
			vtx->pos.x + CLAMP(
				editorCtx->appInfo.winDim.x * 0.5 - vtx->pos.x,
				-150.0f,
				150.0f
			) *
			0.1f,
			vtx->pos.y + CLAMP(
				editorCtx->appInfo.winDim.y * 0.5 - vtx->pos.y,
				-150.0f,
				150.0f
			) *
			0.1f
		};
		
		sprintf(buf, "%d", num);
		nvgFontSize(editorCtx->vg, 16);
		nvgFontFace(editorCtx->vg, "sans");
		nvgTextAlign(editorCtx->vg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
		nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_SPLITTER_DARKER));
		nvgFontBlur(editorCtx->vg, 1.5f);
		nvgText(editorCtx->vg, pos.x, pos.y, buf, 0);
		nvgFontBlur(editorCtx->vg, 0);
		nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_BASE_CONT));
		nvgText(editorCtx->vg, pos.x, pos.y, buf, 0);
		
		vtx = vtx->next;
		num++;
	}
	
	nvgEndFrame(editorCtx->vg);
	#endif
}