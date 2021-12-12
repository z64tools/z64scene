#include "Editor.h"

char* Debug_Split_GetStateStrings(Split* split) {
	static char buffer[1024];
	s32 t = 0;
	char* states[] = {
		"NONE",   "POINT_BL", "POINT_TL", "POINT_TR", "POINT_BR",
		"SIDE_L", "SIDE_T",   "SIDE_R",   "SIDE_B",
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

SplitDir Split_GetDir_Opposite(SplitDir dir) {
	return Lib_Wrap(dir + 2, DIR_L, DIR_B);
}

SplitVtx* Split_AddVertex(GuiContext* guiCtx, f64 x, f64 y) {
	SplitVtx* head = guiCtx->vtxHead;
	
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
	Node_Add(guiCtx->vtxHead, vtx);
	
	return vtx;
}

SplitEdge* Split_AddEdge(GuiContext* guiCtx, SplitVtx* v1, SplitVtx* v2) {
	SplitEdge* head = guiCtx->edgeHead;
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
		Node_Add(guiCtx->edgeHead, edge);
	}
	
	if (edge->vtx[0]->pos.y == edge->vtx[1]->pos.y) {
		edge->state |= EDGE_HORIZONTAL;
		edge->pos = edge->vtx[0]->pos.y;
		if (edge->pos < guiCtx->workRect.y + 1) {
			edge->state |= EDGE_STICK_T;
		}
		if (edge->pos > guiCtx->workRect.y + guiCtx->workRect.h - 1) {
			edge->state |= EDGE_STICK_B;
		}
	} else {
		edge->state |= EDGE_VERTICAL;
		edge->pos = edge->vtx[0]->pos.x;
		if (edge->pos < guiCtx->workRect.x + 1) {
			edge->state |= EDGE_STICK_L;
		}
		if (edge->pos > guiCtx->workRect.x + guiCtx->workRect.w - 1) {
			edge->state |= EDGE_STICK_R;
		}
	}
	
	return edge;
}

SplitDir Split_GerDir_MouseToPressPos(Split* split) {
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

s32 Split_Cursor_GetDistTo(SplitState flag, Split* split) {
	Vec2s mouse[] = {
		{ split->mousePos.x, split->mousePos.y },
		{ split->mousePos.x, split->mousePos.y },
		{ split->mousePos.x, split->mousePos.y },
		{ split->mousePos.x, split->mousePos.y },
		{ split->mousePos.x, 0 },
		{ 0, split->mousePos.y },
		{ split->mousePos.x, 0 },
		{ 0, split->mousePos.y },
	};
	Vec2s pos[] = {
		{
			0,
			split->rect.h,
		},
		{
			0,
			0,
		},
		{
			split->rect.w,
			0,
		},
		{
			split->rect.w,
			split->rect.h,
		},
		{
			0,
			0,
		},
		{
			0,
			0,
		},
		{
			split->rect.w,
			0,
		},
		{
			0,
			split->rect.h,
		},
	};
	s32 i;
	
	for (i = 0; (1 << i) <= SPLIT_SIDE_B; i++) {
		if (flag & (1 << i)) {
			break;
		}
	}
	
	return Vec_Vec2s_DistXZ(&mouse[i], &pos[i]);
}

SplitState Split_GetState_CursorPos(Split* split, s32 range) {
	for (s32 i = 0; (1 << i) <= SPLIT_SIDE_B; i++) {
		if (Split_Cursor_GetDistTo((1 << i), split) <= range) {
			return (1 << i);
		}
	}
	
	return SPLIT_POINT_NONE;
}

bool Split_Cursor_InSplit(Split* split) {
	s32 resX = (split->mousePos.x < split->rect.w && split->mousePos.x >= 0);
	s32 resY = (split->mousePos.y < split->rect.h && split->mousePos.y >= 0);
	
	return (resX && resY);
}

void Split_AddSplit(GuiContext* guiCtx, Rect* rect) {
	Split* split = Lib_Calloc(0, sizeof(Split));
	
	split->vtx[VTX_BOT_L] = Split_AddVertex(guiCtx, rect->x, rect->y + rect->h);
	split->vtx[VTX_TOP_L] = Split_AddVertex(guiCtx, rect->x, rect->y);
	split->vtx[VTX_TOP_R] = Split_AddVertex(guiCtx, rect->x + rect->w, rect->y);
	split->vtx[VTX_BOT_R] =
	    Split_AddVertex(guiCtx, rect->x + rect->w, rect->y + rect->h);
	
	split->edge[EDGE_L] =
	    Split_AddEdge(guiCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_TOP_L]);
	split->edge[EDGE_T] =
	    Split_AddEdge(guiCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
	split->edge[EDGE_R] =
	    Split_AddEdge(guiCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	split->edge[EDGE_B] =
	    Split_AddEdge(guiCtx, split->vtx[VTX_BOT_R], split->vtx[VTX_BOT_L]);
	
	Node_Add(guiCtx->splitHead, split);
}

void Split_SetEdgeMoveClamps(GuiContext* guiCtx) {
	SplitEdge* a = guiCtx->actionEdge;
	s32 align = Lib_Wrap(a->state & EDGE_ALIGN, 0, 1);
	
	guiCtx->edgeMovement.clampMin = guiCtx->workRect.y;
	guiCtx->edgeMovement.clampMax = guiCtx->workRect.w;
}

void Split_Reset(GuiContext* guiCtx) {
	OsAssert(guiCtx->actionSplit);
	guiCtx->actionSplit->stateFlag &= ~(SPLIT_POINTS | SPLIT_SIDES);
	
	guiCtx->actionSplit = NULL;
}

void Split_Split(GuiContext* guiCtx, Split* split, SplitDir dir) {
	Split* partner;
	Split* newSplit;
	SplitDir mirDir = Split_GetDir_Opposite(dir);
	s16 splitPos = (dir == DIR_L || dir == DIR_R) ? __inputCtx->mouse.pos.x
						: __inputCtx->mouse.pos.y;
	
	newSplit = Lib_Calloc(0, sizeof(Split));
	
	Node_Add(guiCtx->splitHead, newSplit);
	
	OsPrintfEx("SplitDir: %d", dir);
	
	if (dir == DIR_L) {
		newSplit->vtx[0] = Split_AddVertex(guiCtx, splitPos, split->vtx[0]->pos.y);
		newSplit->vtx[1] = Split_AddVertex(guiCtx, splitPos, split->vtx[1]->pos.y);
		newSplit->vtx[2] =
		    Split_AddVertex(guiCtx, split->vtx[2]->pos.x, split->vtx[2]->pos.y);
		newSplit->vtx[3] =
		    Split_AddVertex(guiCtx, split->vtx[3]->pos.x, split->vtx[3]->pos.y);
		split->vtx[2] = Split_AddVertex(guiCtx, splitPos, split->vtx[2]->pos.y);
		split->vtx[3] = Split_AddVertex(guiCtx, splitPos, split->vtx[3]->pos.y);
	}
	
	if (dir == DIR_R) {
		newSplit->vtx[0] =
		    Split_AddVertex(guiCtx, split->vtx[0]->pos.x, split->vtx[0]->pos.y);
		newSplit->vtx[1] =
		    Split_AddVertex(guiCtx, split->vtx[1]->pos.x, split->vtx[1]->pos.y);
		newSplit->vtx[2] = Split_AddVertex(guiCtx, splitPos, split->vtx[2]->pos.y);
		newSplit->vtx[3] = Split_AddVertex(guiCtx, splitPos, split->vtx[3]->pos.y);
		split->vtx[0] = Split_AddVertex(guiCtx, splitPos, split->vtx[0]->pos.y);
		split->vtx[1] = Split_AddVertex(guiCtx, splitPos, split->vtx[1]->pos.y);
	}
	
	if (dir == DIR_T) {
		newSplit->vtx[0] =
		    Split_AddVertex(guiCtx, split->vtx[0]->pos.x, split->vtx[0]->pos.y);
		newSplit->vtx[1] = Split_AddVertex(guiCtx, split->vtx[1]->pos.x, splitPos);
		newSplit->vtx[2] = Split_AddVertex(guiCtx, split->vtx[2]->pos.x, splitPos);
		newSplit->vtx[3] =
		    Split_AddVertex(guiCtx, split->vtx[3]->pos.x, split->vtx[3]->pos.y);
		split->vtx[3] = Split_AddVertex(guiCtx, split->vtx[3]->pos.x, splitPos);
		split->vtx[0] = Split_AddVertex(guiCtx, split->vtx[0]->pos.x, splitPos);
	}
	
	if (dir == DIR_B) {
		newSplit->vtx[0] = Split_AddVertex(guiCtx, split->vtx[0]->pos.x, splitPos);
		newSplit->vtx[1] =
		    Split_AddVertex(guiCtx, split->vtx[1]->pos.x, split->vtx[1]->pos.y);
		newSplit->vtx[2] =
		    Split_AddVertex(guiCtx, split->vtx[2]->pos.x, split->vtx[2]->pos.y);
		newSplit->vtx[3] = Split_AddVertex(guiCtx, split->vtx[3]->pos.x, splitPos);
		split->vtx[1] = Split_AddVertex(guiCtx, split->vtx[1]->pos.x, splitPos);
		split->vtx[2] = Split_AddVertex(guiCtx, split->vtx[2]->pos.x, splitPos);
	}
	
	split->edge[EDGE_L] =
	    Split_AddEdge(guiCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_TOP_L]);
	split->edge[EDGE_T] =
	    Split_AddEdge(guiCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
	split->edge[EDGE_R] =
	    Split_AddEdge(guiCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	split->edge[EDGE_B] =
	    Split_AddEdge(guiCtx, split->vtx[VTX_BOT_R], split->vtx[VTX_BOT_L]);
	
	newSplit->edge[EDGE_L] =
	    Split_AddEdge(guiCtx, newSplit->vtx[VTX_BOT_L], newSplit->vtx[VTX_TOP_L]);
	newSplit->edge[EDGE_T] =
	    Split_AddEdge(guiCtx, newSplit->vtx[VTX_TOP_L], newSplit->vtx[VTX_TOP_R]);
	newSplit->edge[EDGE_R] =
	    Split_AddEdge(guiCtx, newSplit->vtx[VTX_TOP_R], newSplit->vtx[VTX_BOT_R]);
	newSplit->edge[EDGE_B] =
	    Split_AddEdge(guiCtx, newSplit->vtx[VTX_BOT_R], newSplit->vtx[VTX_BOT_L]);
	
	guiCtx->actionEdge = newSplit->edge[dir];
	Split_SetEdgeMoveClamps(guiCtx);
}

void Split_KillSplit(GuiContext* guiCtx, Split* split, SplitDir dir) {
	SplitEdge* sharedEdge = split->edge[dir];
	Split* killSplit = guiCtx->splitHead;
	SplitDir oppositeDir = Split_GetDir_Opposite(dir);
	
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
		    Split_AddEdge(guiCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
		split->edge[EDGE_B] =
		    Split_AddEdge(guiCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_BOT_R]);
	}
	
	if (dir == DIR_T) {
		split->vtx[VTX_TOP_L] = killSplit->vtx[VTX_TOP_L];
		split->vtx[VTX_TOP_R] = killSplit->vtx[VTX_TOP_R];
		split->edge[EDGE_L] =
		    Split_AddEdge(guiCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_BOT_L]);
		split->edge[EDGE_R] =
		    Split_AddEdge(guiCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	}
	
	if (dir == DIR_R) {
		split->vtx[VTX_BOT_R] = killSplit->vtx[VTX_BOT_R];
		split->vtx[VTX_TOP_R] = killSplit->vtx[VTX_TOP_R];
		split->edge[EDGE_T] =
		    Split_AddEdge(guiCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_TOP_R]);
		split->edge[EDGE_B] =
		    Split_AddEdge(guiCtx, split->vtx[VTX_BOT_L], split->vtx[VTX_BOT_R]);
	}
	
	if (dir == DIR_B) {
		split->vtx[VTX_BOT_L] = killSplit->vtx[VTX_BOT_L];
		split->vtx[VTX_BOT_R] = killSplit->vtx[VTX_BOT_R];
		split->edge[EDGE_L] =
		    Split_AddEdge(guiCtx, split->vtx[VTX_TOP_L], split->vtx[VTX_BOT_L]);
		split->edge[EDGE_R] =
		    Split_AddEdge(guiCtx, split->vtx[VTX_TOP_R], split->vtx[VTX_BOT_R]);
	}
	
	split->edge[dir] = killSplit->edge[dir];
	
	Node_Kill(guiCtx->splitHead, killSplit);
}

/* 𝗦𝗽𝗹𝗶𝘁𝗩𝘁𝘅 𝗨𝗽𝗱𝗮𝘁𝗲 */

void Split_Update_Vtx_RemoveDublicates(GuiContext* guiCtx, SplitVtx* vtx) {
	SplitVtx* vtx2 = guiCtx->vtxHead;
	
	while (vtx2) {
		if (vtx2 == vtx) {
			vtx2 = vtx2->next;
			continue;
		}
		
		if (Vec2_Equal(&vtx->pos, &vtx2->pos)) {
			SplitVtx* kill = vtx2;
			Split* s = guiCtx->splitHead;
			SplitEdge* e = guiCtx->edgeHead;
			
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
			Node_Kill(guiCtx->vtxHead, kill);
			OsPrintfEx("" PRNT_YELW "\aKill Dublicate Vtx!");
			continue;
		}
		
		vtx2 = vtx2->next;
	}
}

void Split_Update_Vtx(GuiContext* guiCtx) {
	SplitVtx* vtx = guiCtx->vtxHead;
	static s32 clean;
	
	if (guiCtx->actionEdge != NULL) {
		clean = true;
	}
	
	while (vtx) {
		if (clean == true && guiCtx->actionEdge == NULL) {
			Split_Update_Vtx_RemoveDublicates(guiCtx, vtx);
		}
		
		if (vtx->killFlag == true) {
			Split* s = guiCtx->splitHead;
			
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
				OsPrintfEx("" PRNT_YELW "\aKill Tagged Vtx!");
				Node_Kill(guiCtx->vtxHead, killVtx);
				continue;
			}
		}
		
		vtx = vtx->next;
		if (vtx == NULL && guiCtx->actionEdge == NULL) {
			clean = false;
		}
	}
}

/* 𝗦𝗽𝗹𝗶𝘁𝗘𝗱𝗴𝗲 𝗨𝗽𝗱𝗮𝘁𝗲 */

void Split_Update_Edge_RemoveDublicates(GuiContext* guiCtx, SplitEdge* edge) {
	SplitEdge* edge2 = guiCtx->edgeHead;
	
	while (edge2) {
		if (edge2 == edge) {
			edge2 = edge2->next;
			continue;
		}
		
		if (edge2->vtx[0] == edge->vtx[0] && edge2->vtx[1] == edge->vtx[1]) {
			SplitEdge* kill = edge2;
			Split* s = guiCtx->splitHead;
			
			if (guiCtx->actionEdge == edge2) {
				guiCtx->actionEdge = edge;
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
			Node_Kill(guiCtx->edgeHead, kill);
			OsPrintfEx("" PRNT_YELW "\aKill Dublicate Edge!");
			continue;
		}
		
		edge2 = edge2->next;
	}
}

void Split_Update_Edges(GuiContext* guiCtx) {
	SplitEdge* edge = guiCtx->edgeHead;
	f64 diffCentX = (f64)guiCtx->workRect.w / guiCtx->prevWorkRect.w;
	f64 diffCentY = (f64)guiCtx->workRect.h / guiCtx->prevWorkRect.h;
	
	if (!__inputCtx->mouse.cursorAction) {
		guiCtx->actionEdge = NULL;
	}
	
	while (edge) {
		Split_Update_Edge_RemoveDublicates(guiCtx, edge);
		edge = edge->next;
	}
	
	edge = guiCtx->edgeHead;
	
	while (edge) {
		bool isEditEdge = (edge == guiCtx->actionEdge || edge->state & EDGE_EDIT);
		bool isCornerEdge = ((edge->state & EDGE_STICK) != 0);
		bool isHor = ((edge->state & EDGE_VERTICAL) == 0);
		bool matchesActionAlign = (guiCtx->actionEdge && ((edge->state & EDGE_ALIGN) == (guiCtx->actionEdge->state & EDGE_ALIGN)));
		
		if (guiCtx->actionEdge == NULL) {
			edge->state &= ~EDGE_EDIT;
		}
		
		if (edge->killFlag >= 2) {
			SplitEdge* temp = edge->next;
			OsPrintfEx("" PRNT_YELW "\aKill Tagged Edge!");
			Node_Kill(guiCtx->edgeHead, edge);
			edge = temp;
			continue;
		}
		
		OsAssert(!(edge->state & EDGE_HORIZONTAL && edge->state & EDGE_VERTICAL));
		
		if (isCornerEdge) {
			if (edge->state & EDGE_STICK_L) {
				edge->pos = guiCtx->workRect.x;
			}
			if (edge->state & EDGE_STICK_T) {
				edge->pos = guiCtx->workRect.y;
			}
			if (edge->state & EDGE_STICK_R) {
				edge->pos = guiCtx->workRect.x + guiCtx->workRect.w;
			}
			if (edge->state & EDGE_STICK_B) {
				edge->pos = guiCtx->workRect.y + guiCtx->workRect.h;
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
			if (edge->state & EDGE_HORIZONTAL) {
				edge->pos = __inputCtx->mouse.pos.y + 4;
			}
			if (edge->state & EDGE_VERTICAL) {
				edge->pos = __inputCtx->mouse.pos.x + 4;
			}
			edge->pos = CLAMP_MIN(edge->pos, guiCtx->edgeMovement.clampMin + 40);
			edge->pos = CLAMP_MAX(edge->pos, guiCtx->edgeMovement.clampMax - 40);
		}
		
		if (isCornerEdge) {
			s32 revAlign = Lib_Wrap(isHor + 1, 0, 1);
			edge->vtx[0]->pos.s[isHor] = edge->pos;
			edge->vtx[1]->pos.s[isHor] = edge->pos;
		} else {
			if (edge != guiCtx->actionEdge && matchesActionAlign) {
				if (edge->vtx[0]->pos.s[isHor] != edge->pos) {
					edge->state |= EDGE_EDIT;
				} else if (edge->vtx[1]->pos.s[isHor] != edge->pos) {
					edge->state |= EDGE_EDIT;
				}
			}
			
			if (isEditEdge && !isCornerEdge) {
				edge->vtx[0]->pos.s[isHor] = floor(edge->pos * 0.125f) * 8.0f;
				edge->vtx[1]->pos.s[isHor] = floor(edge->pos * 0.125f) * 8.0f;
				edge->pos = floor(edge->pos * 0.125f) * 8.0f;
			} else {
				edge->vtx[0]->pos.s[isHor] = edge->pos;
				edge->vtx[1]->pos.s[isHor] = edge->pos;
			}
		}
		
		edge->killFlag++;
		edge = edge->next;
	}
}

/* 𝗦𝗽𝗹𝗶𝘁 𝗨𝗽𝗱𝗮𝘁𝗲 */

void Split_Update_ActionSplit(GuiContext* guiCtx) {
	const char* stringDirection[] = {
		"EDGE_L",
		"EDGE_T",
		"EDGE_R",
		"EDGE_B",
	};
	const char* stringEdgeState[] = {
		"EDGE_HORIZONTAL", "EDGE_VERTICAL", "EDGE_STICK_L",
		"EDGE_STICK_T",    "EDGE_STICK_R",  "EDGE_STICK_B",
	};
	Split* split = guiCtx->actionSplit;
	
	if (split->mousePress) {
		split->stateFlag |= Split_GetState_CursorPos(split, 20);
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
			
			guiCtx->actionEdge = split->edge[i];
			Split_SetEdgeMoveClamps(guiCtx);
		}
	}
	
	if (__inputCtx->mouse.click.hold) {
		if (split->stateFlag & SPLIT_POINTS) {
			s32 splitDist =
			    Split_Cursor_GetDistTo(split->stateFlag & SPLIT_POINTS, split);
			s32 dist = Vec_Vec2s_DistXZ(&split->mousePos, &split->mousePressPos);
			
			if (dist > 1) {
				CursorIndex cid = Split_GerDir_MouseToPressPos(split) + 1;
				Cursor_SetCursor(cid);
			}
			if (dist > 40) {
				Split_Reset(guiCtx);
				if (split->mouseInRegion) {
					Split_Split(guiCtx, split, Split_GerDir_MouseToPressPos(split));
				} else {
					Split_KillSplit(guiCtx, split, Split_GerDir_MouseToPressPos(split));
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

void Split_Update_Split_SetRect(Split* split) {
	split->rect =
	    (Rect) { floor(split->vtx[1]->pos.x), floor(split->vtx[1]->pos.y),
		     
		     floor(split->vtx[3]->pos.x) - floor(split->vtx[1]->pos.x),
		     floor(split->vtx[3]->pos.y) - floor(split->vtx[1]->pos.y) };
}

void Split_Update_Split(EditorContext* editorCtx) {
	GuiContext* guiCtx = &editorCtx->guiCtx;
	Split* split = guiCtx->splitHead;
	MouseInput* mouse = &editorCtx->inputCtx.mouse;
	
	Cursor_SetCursor(CURSOR_DEFAULT);
	
	if (guiCtx->actionSplit != NULL && mouse->cursorAction == false) {
		Split_Reset(guiCtx);
	}
	
	while (split) {
		Split_Update_Split_SetRect(split);
		Vec2s rectPos = { split->rect.x, split->rect.y };
		Vec_Vec2s_Substract(&split->mousePos, &mouse->pos, (Vec2s*)&rectPos);
		
		split->mouseInRegion = Split_Cursor_InSplit(split);
		split->center.x = split->rect.x + split->rect.w * 0.5f;
		split->center.y = split->rect.y + split->rect.h * 0.5f;
		
		if (guiCtx->actionSplit == NULL && split->mouseInRegion &&
		    mouse->cursorAction) {
			if (mouse->click.press) {
				split->mousePressPos = split->mousePos;
				split->mousePress = true;
				guiCtx->actionSplit = split;
			}
		}
		
		if (guiCtx->actionSplit != NULL) {
			Split_Update_ActionSplit(guiCtx);
		} else {
			if (__inputCtx->mouse.click.hold == 0) {
				if (Split_GetState_CursorPos(split, 20) & SPLIT_POINTS &&
				    split->mouseInRegion) {
					Cursor_SetCursor(CURSOR_CROSSHAIR);
				} else if (Split_GetState_CursorPos(split, 10) & SPLIT_SIDE_H &&
				    split->mouseInRegion) {
					Cursor_SetCursor(CURSOR_ARROW_H);
				} else if (Split_GetState_CursorPos(split, 10) & SPLIT_SIDE_V &&
				    split->mouseInRegion) {
					Cursor_SetCursor(CURSOR_ARROW_V);
				}
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
		for (s32 i = 0; i < 4; i++) {
			OsAssert(split->edge[i] != NULL);
			split->edge[i]->killFlag = false;
		}
		split = split->next;
	}
}

/* 𝗦𝗽𝗹𝗶𝘁 𝗗𝗿𝗮𝘄 */

void Split_Draw_SplitBorder(NVGcontext* vg, Rect* rect, s32 iter) {
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

void Split_Draw_Splits(EditorContext* editorCtx) {
	GuiContext* guiCtx = &editorCtx->guiCtx;
	Split* split = guiCtx->splitHead;
	MouseInput* mouse = &editorCtx->inputCtx.mouse;
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
			
			Split_Draw_SplitBorder(editorCtx->vg, &split->rect, iter);
		}
		nvgEndFrame(editorCtx->vg);
		
		split = split->next;
		iter++;
	}
}

/* 𝗚𝘂𝗶 𝗠𝗮𝗶𝗻 𝗙𝘂𝗻𝗰𝘁𝗶𝗼𝗻𝘀 */

void Gui_UpdateWorkRegion(EditorContext* editorCtx) {
	GuiContext* guiCtx = &editorCtx->guiCtx;
	Vec2s* winDim = &editorCtx->appInfo.winDim;
	
	guiCtx->workRect = (Rect) { 0, 0 + guiCtx->bar[GUI_BAR_TOP].rect.h, winDim->x,
				    winDim->y - guiCtx->bar[GUI_BAR_BOT].rect.h -
				    guiCtx->bar[GUI_BAR_TOP].rect.h };
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

void Gui_Init(EditorContext* editorCtx) {
	GuiContext* guiCtx = &editorCtx->guiCtx;
	Vec2s* winDim = &editorCtx->appInfo.winDim;
	
	Gui_SetTopBarHeight(editorCtx, 30);
	Gui_SetBotBarHeight(editorCtx, 30);
	
	Rect lHalf = { guiCtx->workRect.x, guiCtx->workRect.y, guiCtx->workRect.w / 2,
		       guiCtx->workRect.h };
	Rect rHalf = { guiCtx->workRect.x + guiCtx->workRect.w / 2, guiCtx->workRect.y,
		       guiCtx->workRect.w / 2, guiCtx->workRect.h };
	
	Split_AddSplit(guiCtx, &lHalf);
	Split_AddSplit(guiCtx, &rHalf);
	
	guiCtx->prevWorkRect = guiCtx->workRect;
}

void Gui_Update(EditorContext* editorCtx) {
	GuiContext* guiCtx = &editorCtx->guiCtx;
	
	Gui_SetTopBarHeight(editorCtx, guiCtx->bar[GUI_BAR_TOP].rect.h);
	Gui_SetBotBarHeight(editorCtx, guiCtx->bar[GUI_BAR_BOT].rect.h);
	Split_Update_Vtx(guiCtx);
	Split_Update_Edges(guiCtx);
	if (guiCtx->actionEdge) {
		Split_Update_Edges(guiCtx);
	}
	Split_Update_Split(editorCtx);
	
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
		nvgBeginFrame(
			editorCtx->vg,
			guiCtx->bar[i].rect.w,
			guiCtx->bar[i].rect.h,
			1.0f
		);
		{
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
				
				nvgFillColor(editorCtx->vg, Theme_GetColor(THEME_BASE_CONT));
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
		}
		nvgEndFrame(editorCtx->vg);
	}
	
	Split_Draw_Splits(editorCtx);
	
	#ifndef NDEBUG
	if (guiCtx->actionEdge != NULL) {
		SplitEdge* edge = guiCtx->actionEdge;
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
	
	SplitVtx* vtx = guiCtx->vtxHead;
	Split* split = guiCtx->splitHead;
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