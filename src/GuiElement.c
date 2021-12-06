#include "z64scene.h"

static ElemFunc sElemDrawFuncList[1024];
static Element* sElemInstList[1024];
static s32 sElemDrawListNum;

ElementInit* gElemInitTable[] = {
	&gElPanelInit
};

RGBA8 sGuiColorPalette[] = {
	{ 0x12, 0x14, 0x18, 0xFF },
	{ 0xF2, 0xE7, 0xC4, 0xFF }
};

NVGcolor Element_GetColor(GuiColorPalette pal) {
	return nvgRGBA(
		sGuiColorPalette[pal].r,
		sGuiColorPalette[pal].g,
		sGuiColorPalette[pal].b,
		sGuiColorPalette[pal].a
	);
}

Element* Element_Spawn(EditorContext* editorCtx, u32 id, u32 priority, Vec2f pos, char* title) {
	Element* elem = NULL;
	Element* slot;
	ElementInit* initInfo = NULL;
	
	if (id > ArrayCount(gElemInitTable)) {
		OsPrintfEx("Id exceeds table [%d / %d]", id, ArrayCount(gElemInitTable));
		
		return NULL;
	}
	
	initInfo = gElemInitTable[id];
	elem = Lib_Malloc(0, initInfo->size);
	
	if (elem == NULL) {
		OsPrintfEx("Failed to malloc Element instance size of [0x%08X]", initInfo->size);
		
		return NULL;
	}
	
	memset(elem, 0, initInfo->size);
	slot = editorCtx->elemCtx.node[priority].head;
	if (slot == NULL) {
		editorCtx->elemCtx.node[priority].head = elem;
	} else {
		while (slot->next != NULL) {
			slot = slot->next;
		}
		
		slot->next = elem;
		elem->prev = slot;
	}
	
	elem->init = initInfo->init;
	elem->update = initInfo->update;
	elem->draw = initInfo->draw;
	elem->destroy = initInfo->destroy;
	elem->pos = pos;
	elem->title = title;
	
	gElemInitTable[id]->init(editorCtx, elem);
	editorCtx->elemCtx.node[priority].numElements++;
	
	return elem;
}

void Element_UpdateElements(EditorContext* editorCtx, ElementContext* elemCtx) {
	sElemDrawListNum = 0;
	
	for (s32 i = 0; i < 16; i++) {
		ElementNode* node = &elemCtx->node[i];
		Element* elem = node->head;
		Element* freeElem;
		s32 numChecker = 0;
		
		if (node->numElements < 0) {
			OsPrintfEx("ElementNode[%d] numElements below zero! [%d]", i, node->numElements);
			continue;
		}
		
		if (node->numElements == 0) {
			continue;
		}
		
		while (elem != NULL) {
			if (elem->update) {
				elem->update(editorCtx, elem);
				if (elem->draw) {
					sElemInstList[sElemDrawListNum] = elem;
					sElemDrawFuncList[sElemDrawListNum++] = elem->draw;
				}
				elem = elem->next;
				numChecker++;
			} else {
				// Clean elem between next and prev node
				if (elem->next) {
					elem->next->prev = elem->prev;
				}
				
				if (elem->prev) {
					elem->prev->next = elem->next;
				} else {
					node->head = elem->next;
				}
				
				freeElem = elem;
				elem = elem->next;
				
				freeElem->destroy(editorCtx, freeElem);
				free(freeElem);
				node->numElements--;
				
				if (node->numElements < 0) {
					OsPrintfEx("ElementNode[%d] numElements below zero! [%d]", i, node->numElements);
				}
			}
		}
	}
}

void Element_DrawElements(EditorContext* editorCtx, ElementContext* elemCtx) {
	for (s32 i = 0; i < sElemDrawListNum; i++) {
		sElemDrawFuncList[i](editorCtx, sElemInstList[i]);
	}
}