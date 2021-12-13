#include "Editor.h"

char* Debug_FlexUI_GetStateStrings(FlexSplit* split) {
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

FlexDir FlexUI_GetDir_Opposite(FlexDir dir) {
	return Lib_Wrap(dir + 2, DIR_L, DIR_B);
}

FlexDir FlexUI_GerDir_MouseToPressPos(FlexSplit* split) {
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

FlexVtx* FlexUI_AddVtx(FlexUIContext* flexCtx, f64 x, f64 y) {
	FlexVtx* head = flexCtx->vtxHead;
	
	while (head) {
		if (ABS(head->pos.x - x) <= 1.0 && ABS(head->pos.y - y) <= 1.0) {
			OsPrintfEx("VtxConnect %.2f %.2f", x, y);
			
			return head;
		}
		head = head->next;
	}
	
	FlexVtx* vtx = Lib_Calloc(0, sizeof(FlexVtx));
	
	vtx->pos.x = x;
	vtx->pos.y = y;
	Node_Add(flexCtx->vtxHead, vtx);
	
	return vtx;
}

FlexEdge* FlexUI_AddEdge(FlexUIContext* flexCtx, FlexVtx* v1, FlexVtx* v2) {
	FlexEdge* head = flexCtx->edgeHead;
	FlexEdge* edge = NULL;
	
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
		edge = Lib_Calloc(0, sizeof(FlexEdge));
		
		edge->vtx[0] = v1;
		edge->vtx[1] = v2;
		Node_Add(flexCtx->edgeHead, edge);
	}
	
	if (edge->vtx[0]->pos.y == edge->vtx[1]->pos.y) {
		edge->state |= EDGE_HORIZONTAL;
		edge->pos = edge->vtx[0]->pos.y;
		if (edge->pos < flexCtx->workRect.y + 1) {
			edge->state |= EDGE_STICK_T;
		}
		if (edge->pos > flexCtx->workRect.y + flexCtx->workRect.h - 1) {
			edge->state |= EDGE_STICK_B;
		}
	} else {
		edge->state |= EDGE_VERTICAL;
		edge->pos = edge->vtx[0]->pos.x;
		if (edge->pos < flexCtx->workRect.x + 1) {
			edge->state |= EDGE_STICK_L;
		}
		if (edge->pos > flexCtx->workRect.x + flexCtx->workRect.w - 1) {
			edge->state |= EDGE_STICK_R;
		}
	}
	
	return edge;
}

s32 FlexUI_Cursor_GetDistTo(SplitState flag, FlexSplit* split) {
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

SplitState FlexUI_GetState_CursorPos(FlexSplit* split, s32 range) {
	for (s32 i = 0; (1 << i) <= SPLIT_SIDE_B; i++) {
		if (FlexUI_Cursor_GetDistTo((1 << i), split) <= range) {
			return (1 << i);
		}
	}
	
	return SPLIT_POINT_NONE;
}

bool FlexUI_Cursor_InSplit(FlexSplit* split) {
	s32 resX = (split->mousePos.x < split->rect.w && split->mousePos.x >= 0);
	s32 resY = (split->mousePos.y < split->rect.h && split->mousePos.y >= 0);
	
	return (resX && resY);
}

void FlexUI_AddSplit(FlexUIContext* flexCtx, Rect* rect) {
	FlexSplit* split = Lib_Calloc(0, sizeof(FlexSplit));
	
	split->vtx[VTX_BOT_L] = FlexUI_AddVtx(flexCtx, rect->x, rect->y + rect->h);
	split->vtx[VTX_TOP_L] = FlexUI_AddVtx(flexCtx, rect->x, rect->y);
	split->vtx[VTX_TOP_R] = FlexUI_AddVtx(flexCtx, rect->x + rect->w, rect->y);
	split->vtx[VTX_BOT_R] =
	    FlexUI_AddVtx(flexCtx, rect->x + rect->w, rect->y + rect->h);
	
	split->edge[EDGE_L] =
	    FlexUI_AddEdge(flexCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_TOP_L]);
	split->edge[EDGE_T] =
	    FlexUI_AddEdge(flexCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
	split->edge[EDGE_R] =
	    FlexUI_AddEdge(flexCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	split->edge[EDGE_B] =
	    FlexUI_AddEdge(flexCtx, split->vtx[VTX_BOT_R], split->vtx[VTX_BOT_L]);
	
	Node_Add(flexCtx->splitHead, split);
}

void FlexUI_Edge_SetSlideClamp(FlexUIContext* flexCtx) {
	FlexEdge* tempEdge = flexCtx->edgeHead;
	FlexEdge* setEdge = flexCtx->actionEdge;
	FlexEdge* actionEdge = flexCtx->actionEdge;
	FlexVtx* vtxList[128] = { NULL };
	u32 vtxListCount = 0;
	u32 align = Lib_Wrap((actionEdge->state & EDGE_ALIGN), 0, 1);
	f64 posMin = actionEdge->vtx[0]->pos.s[align];
	f64 posMax = actionEdge->vtx[1]->pos.s[align];
	
	setEdge->state |= EDGE_EDIT;
	
	// Get edge with vtx closest to TOPLEFT
	while (tempEdge) {
		if ((tempEdge->state & EDGE_ALIGN) == (setEdge->state & EDGE_ALIGN)) {
			if (tempEdge->vtx[1] == setEdge->vtx[0]) {
				setEdge = tempEdge;
				tempEdge->state |= EDGE_EDIT;
				tempEdge = flexCtx->edgeHead;
				posMin = setEdge->vtx[0]->pos.s[align];
				continue;
			}
		}
		
		tempEdge = tempEdge->next;
	}
	
	tempEdge = flexCtx->edgeHead;
	
	// Set all below setEdgeA
	while (tempEdge) {
		if ((tempEdge->state & EDGE_ALIGN) == (setEdge->state & EDGE_ALIGN)) {
			if (tempEdge->vtx[0] == setEdge->vtx[1]) {
				tempEdge->state |= EDGE_EDIT;
				setEdge = tempEdge;
				tempEdge = flexCtx->edgeHead;
				posMax = setEdge->vtx[1]->pos.s[align];
				continue;
			}
		}
		
		tempEdge = tempEdge->next;
	}
	
	tempEdge = flexCtx->edgeHead;
	
	while (tempEdge) {
		if ((tempEdge->state & EDGE_ALIGN) == (actionEdge->state & EDGE_ALIGN)) {
			if (tempEdge->vtx[1]->pos.s[align] <= posMax && tempEdge->vtx[0]->pos.s[align] >= posMin) {
				tempEdge->state |= EDGE_EDIT;
			}
		}
		tempEdge = tempEdge->next;
	}
	
	flexCtx->edgeMovement.clampMin = flexCtx->workRect.y;
	flexCtx->edgeMovement.clampMax = flexCtx->workRect.w;
}

void FlexUI_Reset(FlexUIContext* flexCtx) {
	OsAssert(flexCtx->actionSplit);
	flexCtx->actionSplit->stateFlag &= ~(SPLIT_POINTS | SPLIT_SIDES);
	
	flexCtx->actionSplit = NULL;
}

void FlexUI_Split(FlexUIContext* flexCtx, FlexSplit* split, FlexDir dir) {
	FlexSplit* partner;
	FlexSplit* newSplit;
	FlexDir mirDir = FlexUI_GetDir_Opposite(dir);
	f64 splitPos = (dir == DIR_L || dir == DIR_R) ? flexCtx->mouse->pos.x : flexCtx->mouse->pos.y;
	
	// splitPos = CLAMP(splitPos - split->edge[mirDir]->pos, -SPLIT_CLAMP * 0.90f, SPLIT_CLAMP * 0.90f);
	// splitPos += split->edge[mirDir]->pos;
	
	newSplit = Lib_Calloc(0, sizeof(FlexSplit));
	
	Node_Add(flexCtx->splitHead, newSplit);
	
	OsPrintfEx("FlexDir: %d", dir);
	
	if (dir == DIR_L) {
		newSplit->vtx[0] = FlexUI_AddVtx(flexCtx, splitPos, split->vtx[0]->pos.y);
		newSplit->vtx[1] = FlexUI_AddVtx(flexCtx, splitPos, split->vtx[1]->pos.y);
		newSplit->vtx[2] =  FlexUI_AddVtx(flexCtx, split->vtx[2]->pos.x, split->vtx[2]->pos.y);
		newSplit->vtx[3] = FlexUI_AddVtx(flexCtx, split->vtx[3]->pos.x, split->vtx[3]->pos.y);
		split->vtx[2] = FlexUI_AddVtx(flexCtx, splitPos, split->vtx[2]->pos.y);
		split->vtx[3] = FlexUI_AddVtx(flexCtx, splitPos, split->vtx[3]->pos.y);
	}
	
	if (dir == DIR_R) {
		newSplit->vtx[0] = FlexUI_AddVtx(flexCtx, split->vtx[0]->pos.x, split->vtx[0]->pos.y);
		newSplit->vtx[1] = FlexUI_AddVtx(flexCtx, split->vtx[1]->pos.x, split->vtx[1]->pos.y);
		newSplit->vtx[2] = FlexUI_AddVtx(flexCtx, splitPos, split->vtx[2]->pos.y);
		newSplit->vtx[3] = FlexUI_AddVtx(flexCtx, splitPos, split->vtx[3]->pos.y);
		split->vtx[0] = FlexUI_AddVtx(flexCtx, splitPos, split->vtx[0]->pos.y);
		split->vtx[1] = FlexUI_AddVtx(flexCtx, splitPos, split->vtx[1]->pos.y);
	}
	
	if (dir == DIR_T) {
		newSplit->vtx[0] = FlexUI_AddVtx(flexCtx, split->vtx[0]->pos.x, split->vtx[0]->pos.y);
		newSplit->vtx[1] = FlexUI_AddVtx(flexCtx, split->vtx[1]->pos.x, splitPos);
		newSplit->vtx[2] = FlexUI_AddVtx(flexCtx, split->vtx[2]->pos.x, splitPos);
		newSplit->vtx[3] = FlexUI_AddVtx(flexCtx, split->vtx[3]->pos.x, split->vtx[3]->pos.y);
		split->vtx[3] = FlexUI_AddVtx(flexCtx, split->vtx[3]->pos.x, splitPos);
		split->vtx[0] = FlexUI_AddVtx(flexCtx, split->vtx[0]->pos.x, splitPos);
	}
	
	if (dir == DIR_B) {
		newSplit->vtx[0] = FlexUI_AddVtx(flexCtx, split->vtx[0]->pos.x, splitPos);
		newSplit->vtx[1] = FlexUI_AddVtx(flexCtx, split->vtx[1]->pos.x, split->vtx[1]->pos.y);
		newSplit->vtx[2] = FlexUI_AddVtx(flexCtx, split->vtx[2]->pos.x, split->vtx[2]->pos.y);
		newSplit->vtx[3] = FlexUI_AddVtx(flexCtx, split->vtx[3]->pos.x, splitPos);
		split->vtx[1] = FlexUI_AddVtx(flexCtx, split->vtx[1]->pos.x, splitPos);
		split->vtx[2] = FlexUI_AddVtx(flexCtx, split->vtx[2]->pos.x, splitPos);
	}
	
	split->edge[EDGE_L] = FlexUI_AddEdge(flexCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_TOP_L]);
	split->edge[EDGE_T] = FlexUI_AddEdge(flexCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
	split->edge[EDGE_R] = FlexUI_AddEdge(flexCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	split->edge[EDGE_B] = FlexUI_AddEdge(flexCtx, split->vtx[VTX_BOT_R], split->vtx[VTX_BOT_L]);
	
	newSplit->edge[EDGE_L] = FlexUI_AddEdge(flexCtx, newSplit->vtx[VTX_BOT_L], newSplit->vtx[VTX_TOP_L]);
	newSplit->edge[EDGE_T] = FlexUI_AddEdge(flexCtx, newSplit->vtx[VTX_TOP_L], newSplit->vtx[VTX_TOP_R]);
	newSplit->edge[EDGE_R] = FlexUI_AddEdge(flexCtx, newSplit->vtx[VTX_TOP_R], newSplit->vtx[VTX_BOT_R]);
	newSplit->edge[EDGE_B] = FlexUI_AddEdge(flexCtx, newSplit->vtx[VTX_BOT_R], newSplit->vtx[VTX_BOT_L]);
	
	flexCtx->actionEdge = newSplit->edge[dir];
	FlexUI_RemoveDublicates(flexCtx);
	FlexUI_Edge_SetSlideClamp(flexCtx);
}

void FlexUI_KillSplit(FlexUIContext* flexCtx, FlexSplit* split, FlexDir dir) {
	FlexEdge* sharedEdge = split->edge[dir];
	FlexSplit* killSplit = flexCtx->splitHead;
	FlexDir oppositeDir = FlexUI_GetDir_Opposite(dir);
	
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
		split->edge[EDGE_T] = FlexUI_AddEdge(flexCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
		split->edge[EDGE_B] = FlexUI_AddEdge(flexCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_BOT_R]);
	}
	
	if (dir == DIR_T) {
		split->vtx[VTX_TOP_L] = killSplit->vtx[VTX_TOP_L];
		split->vtx[VTX_TOP_R] = killSplit->vtx[VTX_TOP_R];
		split->edge[EDGE_L] = FlexUI_AddEdge(flexCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_BOT_L]);
		split->edge[EDGE_R] = FlexUI_AddEdge(flexCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	}
	
	if (dir == DIR_R) {
		split->vtx[VTX_BOT_R] = killSplit->vtx[VTX_BOT_R];
		split->vtx[VTX_TOP_R] = killSplit->vtx[VTX_TOP_R];
		split->edge[EDGE_T] = FlexUI_AddEdge(flexCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
		split->edge[EDGE_B] = FlexUI_AddEdge(flexCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_BOT_R]);
	}
	
	if (dir == DIR_B) {
		split->vtx[VTX_BOT_L] = killSplit->vtx[VTX_BOT_L];
		split->vtx[VTX_BOT_R] = killSplit->vtx[VTX_BOT_R];
		split->edge[EDGE_L] = FlexUI_AddEdge(flexCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_BOT_L]);
		split->edge[EDGE_R] = FlexUI_AddEdge(flexCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	}
	
	split->edge[dir] = killSplit->edge[dir];
	
	Node_Kill(flexCtx->splitHead, killSplit);
}

/* ───────────────────────────────────────────────────────────────────────── */

void FlexUI_Update_Vtx_RemoveDublicates(FlexUIContext* flexCtx, FlexVtx* vtx) {
	FlexVtx* vtx2 = flexCtx->vtxHead;
	
	while (vtx2) {
		if (vtx2 == vtx) {
			vtx2 = vtx2->next;
			continue;
		}
		
		if (Vec2_Equal(&vtx->pos, &vtx2->pos)) {
			FlexVtx* kill = vtx2;
			FlexSplit* s = flexCtx->splitHead;
			FlexEdge* e = flexCtx->edgeHead;
			
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
			Node_Kill(flexCtx->vtxHead, kill);
			OsPrintfEx("" PRNT_YELW "\aKill Dublicate Vtx!");
			continue;
		}
		
		vtx2 = vtx2->next;
	}
}

void FlexUI_Update_Vtx(FlexUIContext* flexCtx) {
	FlexVtx* vtx = flexCtx->vtxHead;
	static s32 clean;
	
	if (flexCtx->actionEdge != NULL) {
		clean = true;
	}
	
	while (vtx) {
		if (clean == true && flexCtx->actionEdge == NULL) {
			FlexUI_Update_Vtx_RemoveDublicates(flexCtx, vtx);
		}
		
		if (vtx->killFlag == true) {
			FlexSplit* s = flexCtx->splitHead;
			
			while (s) {
				for (s32 i = 0; i < 4; i++) {
					if (s->vtx[i] == vtx) {
						vtx->killFlag = false;
					}
				}
				s = s->next;
			}
			
			if (vtx->killFlag == true) {
				FlexVtx* killVtx = vtx;
				vtx = vtx->prev;
				OsPrintfEx("" PRNT_YELW "\aKill Tagged Vtx!");
				Node_Kill(flexCtx->vtxHead, killVtx);
				continue;
			}
		}
		
		vtx = vtx->next;
		if (vtx == NULL && flexCtx->actionEdge == NULL) {
			clean = false;
		}
	}
}

/* ───────────────────────────────────────────────────────────────────────── */

void FlexUI_Update_Edge_RemoveDublicates(FlexUIContext* flexCtx, FlexEdge* edge) {
	FlexEdge* edge2 = flexCtx->edgeHead;
	
	while (edge2) {
		if (edge2 == edge) {
			edge2 = edge2->next;
			continue;
		}
		
		if (edge2->vtx[0] == edge->vtx[0] && edge2->vtx[1] == edge->vtx[1]) {
			FlexEdge* kill = edge2;
			FlexSplit* s = flexCtx->splitHead;
			
			if (flexCtx->actionEdge == edge2) {
				flexCtx->actionEdge = edge;
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
			Node_Kill(flexCtx->edgeHead, kill);
			OsPrintfEx("" PRNT_YELW "\aKill Dublicate Edge!");
			continue;
		}
		
		edge2 = edge2->next;
	}
}

void FlexUI_Update_Edges(FlexUIContext* flexCtx) {
	FlexEdge* edge = flexCtx->edgeHead;
	f64 diffCentX = (f64)flexCtx->workRect.w / flexCtx->prevWorkRect.w;
	f64 diffCentY = (f64)flexCtx->workRect.h / flexCtx->prevWorkRect.h;
	
	if (!flexCtx->mouse->click.hold) {
		flexCtx->actionEdge = NULL;
	}
	
	while (edge) {
		if (edge->killFlag == true) {
			FlexEdge* temp = edge->next;
			OsPrintfEx("" PRNT_YELW "\aKill Tagged Edge!");
			Node_Kill(flexCtx->edgeHead, edge);
			edge = temp;
			continue;
		}
		edge->killFlag = true;
		
		if (flexCtx->actionEdge == NULL) {
			edge->state &= ~EDGE_EDIT;
		}
		
		FlexUI_Update_Edge_RemoveDublicates(flexCtx, edge);
		edge = edge->next;
	}
	
	edge = flexCtx->edgeHead;
	
	while (edge) {
		bool isEditEdge = (edge == flexCtx->actionEdge || edge->state & EDGE_EDIT);
		bool isCornerEdge = ((edge->state & EDGE_STICK) != 0);
		bool isHor = ((edge->state & EDGE_VERTICAL) == 0);
		bool matchesActionAlign = (flexCtx->actionEdge && ((edge->state & EDGE_ALIGN) == (flexCtx->actionEdge->state & EDGE_ALIGN)));
		
		OsAssert(!(edge->state & EDGE_HORIZONTAL && edge->state & EDGE_VERTICAL));
		
		if (isCornerEdge) {
			if (edge->state & EDGE_STICK_L) {
				edge->pos = flexCtx->workRect.x;
			}
			if (edge->state & EDGE_STICK_T) {
				edge->pos = flexCtx->workRect.y;
			}
			if (edge->state & EDGE_STICK_R) {
				edge->pos = flexCtx->workRect.x + flexCtx->workRect.w;
			}
			if (edge->state & EDGE_STICK_B) {
				edge->pos = flexCtx->workRect.y + flexCtx->workRect.h;
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
			FlexEdge* temp = flexCtx->edgeHead;
			s32 align = Lib_Wrap(edge->state & EDGE_ALIGN, 0, 1);
			
			while (temp) {
				for (s32 i = 0; i < 2; i++) {
					if (((temp->state & EDGE_ALIGN) != (edge->state & EDGE_ALIGN)) && temp->vtx[1] == edge->vtx[i]) {
						if (temp->vtx[0]->pos.s[align] > flexCtx->edgeMovement.clampMin) {
							flexCtx->edgeMovement.clampMin = temp->vtx[0]->pos.s[align];
							OsPrintfEx("foundMin %.2f", temp->vtx[0]->pos.s[align]);
						}
					}
				}
				
				for (s32 i = 0; i < 2; i++) {
					if (((temp->state & EDGE_ALIGN) != (edge->state & EDGE_ALIGN)) && temp->vtx[0] == edge->vtx[i]) {
						if (temp->vtx[1]->pos.s[align] < flexCtx->edgeMovement.clampMax) {
							flexCtx->edgeMovement.clampMax = temp->vtx[1]->pos.s[align];
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
			edge->pos = CLAMP_MIN(edge->pos, flexCtx->edgeMovement.clampMin + SPLIT_CLAMP);
			edge->pos = CLAMP_MAX(edge->pos, flexCtx->edgeMovement.clampMax - SPLIT_CLAMP);
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

void FlexUI_Update_ActionSplit(FlexUIContext* flexCtx) {
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
	FlexSplit* split = flexCtx->actionSplit;
	
	if (flexCtx->mouse->click.press) {
		split->stateFlag |= FlexUI_GetState_CursorPos(split, 20);
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
			OsPrintfEx("FlexSplit: %s", stringDirection[i]);
			
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
			
			flexCtx->actionEdge = split->edge[i];
			FlexUI_Edge_SetSlideClamp(flexCtx);
			FlexUI_Reset(flexCtx);
		}
	}
	
	if (flexCtx->mouse->click.hold) {
		if (split->stateFlag & SPLIT_POINTS) {
			s32 splitDist =
			    FlexUI_Cursor_GetDistTo(split->stateFlag & SPLIT_POINTS, split);
			s32 dist = Vec_Vec2s_DistXZ(&split->mousePos, &split->mousePressPos);
			
			if (dist > 1) {
				CursorIndex cid = FlexUI_GerDir_MouseToPressPos(split) + 1;
				Cursor_SetCursor(cid);
			}
			if (dist > SPLIT_CLAMP * 1.25) {
				FlexUI_Reset(flexCtx);
				if (split->mouseInSplit) {
					FlexUI_Split(flexCtx, split, FlexUI_GerDir_MouseToPressPos(split));
				} else {
					FlexUI_KillSplit(flexCtx, split, FlexUI_GerDir_MouseToPressPos(split));
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

void FlexUI_Update_SplitRect(FlexSplit* split) {
	split->rect = (Rect) {
		floor(split->vtx[1]->pos.x),
		floor(split->vtx[1]->pos.y),
		floor(split->vtx[3]->pos.x) - floor(split->vtx[1]->pos.x),
		floor(split->vtx[3]->pos.y) - floor(split->vtx[1]->pos.y)
	};
}

void FlexUI_Update_Split(FlexUIContext* flexCtx) {
	FlexSplit* split = flexCtx->splitHead;
	MouseInput* mouse = flexCtx->mouse;
	
	Cursor_SetCursor(CURSOR_DEFAULT);
	
	if (flexCtx->actionSplit != NULL && mouse->cursorAction == false) {
		FlexUI_Reset(flexCtx);
	}
	
	while (split) {
		FlexUI_Update_SplitRect(split);
		Vec2s rectPos = { split->rect.x, split->rect.y };
		Vec_Vec2s_Substract(&split->mousePos, &mouse->pos, (Vec2s*)&rectPos);
		
		split->mouseInSplit = FlexUI_Cursor_InSplit(split);
		split->center.x = split->rect.x + split->rect.w * 0.5f;
		split->center.y = split->rect.y + split->rect.h * 0.5f;
		
		if (flexCtx->actionSplit == NULL && split->mouseInSplit &&
		    mouse->cursorAction) {
			if (mouse->click.press) {
				split->mousePressPos = split->mousePos;
				flexCtx->actionSplit = split;
			}
		}
		
		if (flexCtx->actionSplit != NULL) {
			if (flexCtx->actionSplit == split)
				FlexUI_Update_ActionSplit(flexCtx);
		} else {
			if (flexCtx->mouse->click.hold == 0) {
				if (FlexUI_GetState_CursorPos(split, 20) & SPLIT_POINTS &&
				    split->mouseInSplit) {
					Cursor_SetCursor(CURSOR_CROSSHAIR);
				} else if (FlexUI_GetState_CursorPos(split, 10) & SPLIT_SIDE_H &&
				    split->mouseInSplit) {
					Cursor_SetCursor(CURSOR_ARROW_H);
				} else if (FlexUI_GetState_CursorPos(split, 10) & SPLIT_SIDE_V &&
				    split->mouseInSplit) {
					Cursor_SetCursor(CURSOR_ARROW_V);
				}
			}
		}
		
		FlexUI_Update_SplitRect(split);
		
		for (s32 i = 0; i < 4; i++) {
			OsAssert(split->edge[i] != NULL);
			split->edge[i]->killFlag = false;
		}
		split = split->next;
	}
}

/* ───────────────────────────────────────────────────────────────────────── */

void FlexUI_Draw_SplitBorder(NVGcontext* vg, Rect* rect, s32 iter) {
	nvgBeginPath(vg);
	nvgRect(vg, 0, 0, rect->w, rect->h);
	nvgPathWinding(vg, NVG_HOLE);
	nvgFillColor(vg, Theme_GetColor(THEME_BAS2));
	nvgFill(vg);
	
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
	nvgFillColor(vg, Theme_GetColor(THEME_BASE));
	nvgFill(vg);
}

void FlexUI_Draw_Splits(FlexUIContext* flexCtx) {
	FlexSplit* split = flexCtx->splitHead;
	Vec2s* winDim = flexCtx->winDim;
	s32 iter = 0;
	
	while (split != NULL) {
		glViewport(
			split->rect.x,
			winDim->y - split->rect.y - split->rect.h,
			split->rect.w,
			split->rect.h
		);
		nvgBeginFrame(flexCtx->vg, split->rect.w, split->rect.h, 1.0f); {
			if (split->update)
				split->update(split->passArg, split);
			
			FlexUI_Draw_SplitBorder(flexCtx->vg, &split->rect, iter);
		} nvgEndFrame(flexCtx->vg);
		
		split = split->next;
		iter++;
	}
}

/* ───────────────────────────────────────────────────────────────────────── */

void FlexUI_RemoveDublicates(FlexUIContext* flexCtx) {
	FlexVtx* vtx = flexCtx->vtxHead;
	FlexEdge* edge = flexCtx->edgeHead;
	
	while (vtx) {
		FlexUI_Update_Vtx_RemoveDublicates(flexCtx, vtx);
		vtx = vtx->next;
	}
	
	while (edge) {
		FlexUI_Update_Edge_RemoveDublicates(flexCtx, edge);
		edge = edge->next;
	}
}

void FlexUI_UpdateWorkRect(FlexUIContext* flexCtx) {
	Vec2s* winDim = flexCtx->winDim;
	
	flexCtx->workRect = (Rect) { 0, 0 + flexCtx->bar[BAR_TOP].rect.h, winDim->x,
				     winDim->y - flexCtx->bar[BAR_BOT].rect.h -
				     flexCtx->bar[BAR_TOP].rect.h };
}

void FlexUI_SetTopBarHeight(FlexUIContext* flexCtx, s32 h) {
	flexCtx->bar[BAR_TOP].rect.x = 0;
	flexCtx->bar[BAR_TOP].rect.y = 0;
	flexCtx->bar[BAR_TOP].rect.w = flexCtx->winDim->x;
	flexCtx->bar[BAR_TOP].rect.h = h;
	FlexUI_UpdateWorkRect(flexCtx);
}

void FlexUI_SetBotBarHeight(FlexUIContext* flexCtx, s32 h) {
	flexCtx->bar[BAR_BOT].rect.x = 0;
	flexCtx->bar[BAR_BOT].rect.y = flexCtx->winDim->y - h;
	flexCtx->bar[BAR_BOT].rect.w = flexCtx->winDim->x;
	flexCtx->bar[BAR_BOT].rect.h = h;
	FlexUI_UpdateWorkRect(flexCtx);
}

void FlexUI_Init(FlexUIContext* flexCtx, Vec2s* winDim, MouseInput* mouse, void* vg) {
	flexCtx->winDim = winDim;
	flexCtx->mouse = mouse;
	flexCtx->vg = vg;
	FlexUI_SetTopBarHeight(flexCtx, 30);
	FlexUI_SetBotBarHeight(flexCtx, 30);
	
	Rect lHalf = { flexCtx->workRect.x, flexCtx->workRect.y, flexCtx->workRect.w / 2,
		       flexCtx->workRect.h };
	Rect rHalf = { flexCtx->workRect.x + flexCtx->workRect.w / 2, flexCtx->workRect.y,
		       flexCtx->workRect.w / 2,                      flexCtx->workRect.h };
	
	FlexUI_AddSplit(flexCtx, &lHalf);
	FlexUI_AddSplit(flexCtx, &rHalf);
	
	flexCtx->prevWorkRect = flexCtx->workRect;
}

void FlexUI_Update(FlexUIContext* flexCtx) {
	FlexUI_SetTopBarHeight(flexCtx, flexCtx->bar[BAR_TOP].rect.h);
	FlexUI_SetBotBarHeight(flexCtx, flexCtx->bar[BAR_BOT].rect.h);
	FlexUI_Update_Vtx(flexCtx);
	FlexUI_Update_Edges(flexCtx);
	FlexUI_Update_Split(flexCtx);
	
	flexCtx->prevWorkRect = flexCtx->workRect;
}

void FlexUI_Draw(FlexUIContext* flexCtx) {
	Vec2s* winDim = flexCtx->winDim;
	
	// Draw Bars
	for (s32 i = 0; i < 2; i++) {
		glViewport(
			flexCtx->bar[i].rect.x,
			winDim->y - flexCtx->bar[i].rect.y - flexCtx->bar[i].rect.h,
			flexCtx->bar[i].rect.w,
			flexCtx->bar[i].rect.h
		);
		nvgBeginFrame(flexCtx->vg, flexCtx->bar[i].rect.w, flexCtx->bar[i].rect.h, 1.0f); {
			nvgBeginPath(flexCtx->vg);
			nvgRect(
				flexCtx->vg,
				0,
				0,
				flexCtx->bar[i].rect.w,
				flexCtx->bar[i].rect.h
			);
			nvgFillColor(flexCtx->vg, Theme_GetColor(THEME_DARK));
			nvgFill(flexCtx->vg);
			
			if (i == 1) {
				static FlexSplit* lastActive;
				
				if (lastActive == NULL) {
					lastActive = flexCtx->splitHead;
				}
				
				if (flexCtx->actionSplit) {
					lastActive = flexCtx->actionSplit;
				}
				
				nvgFillColor(flexCtx->vg, Theme_GetColor(THEME_TEXT));
				nvgFontSize(flexCtx->vg, 30 - 16);
				nvgFontFace(flexCtx->vg, "sans");
				nvgTextAlign(flexCtx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
				nvgText(
					flexCtx->vg,
					8,
					8,
					Debug_FlexUI_GetStateStrings(lastActive),
					NULL
				);
			}
		} nvgEndFrame(flexCtx->vg);
	}
	
	FlexUI_Draw_Splits(flexCtx);
	
	#if 1
	FlexVtx* vtx = flexCtx->vtxHead;
	FlexSplit* split = flexCtx->splitHead;
	s32 num = 0;
	glViewport(0, 0, winDim->x, winDim->y);
	nvgBeginFrame(flexCtx->vg, winDim->x, winDim->y, 1.0f); {
		while (split) {
			nvgBeginPath(flexCtx->vg);
			nvgLineCap(flexCtx->vg, NVG_ROUND);
			nvgStrokeWidth(flexCtx->vg, 0.8f);
			nvgMoveTo(
				flexCtx->vg,
				split->vtx[0]->pos.x + 2,
				split->vtx[0]->pos.y - 2
			);
			nvgLineTo(
				flexCtx->vg,
				split->vtx[1]->pos.x + 2,
				split->vtx[1]->pos.y + 2
			);
			nvgLineTo(
				flexCtx->vg,
				split->vtx[2]->pos.x - 2,
				split->vtx[2]->pos.y + 2
			);
			nvgLineTo(
				flexCtx->vg,
				split->vtx[3]->pos.x - 2,
				split->vtx[3]->pos.y - 2
			);
			nvgLineTo(
				flexCtx->vg,
				split->vtx[0]->pos.x + 2,
				split->vtx[0]->pos.y - 2
			);
			nvgStrokeColor(flexCtx->vg, nvgHSLA(0.111 * num, 1.0f, 0.4f, 255));
			nvgStroke(flexCtx->vg);
			
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
			nvgFontSize(flexCtx->vg, 16);
			nvgFontFace(flexCtx->vg, "sans");
			nvgTextAlign(flexCtx->vg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
			nvgFillColor(flexCtx->vg, Theme_GetColor(THEME_DARK));
			nvgFontBlur(flexCtx->vg, 1.5f);
			nvgText(flexCtx->vg, pos.x, pos.y, buf, 0);
			nvgFontBlur(flexCtx->vg, 0);
			nvgFillColor(flexCtx->vg, Theme_GetColor(THEME_TEXT));
			nvgText(flexCtx->vg, pos.x, pos.y, buf, 0);
			
			vtx = vtx->next;
			num++;
		}
		
	} nvgEndFrame(flexCtx->vg);
	#endif
}