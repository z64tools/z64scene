#include "Properties.h"

///////////////////////////////////////////////////////////////////////////////

static void SelectActor(Editor* editor, RoomHeader* room, Actor* actor) {
	Actor_UnselectAll(&editor->scene, room);
	Gizmo_UnselectAll(&editor->gizmo);
	
	if (!actor) return;
	
	Actor_Select(&editor->scene, actor);
	Actor_Focus(&editor->scene, actor);
	
	Gizmo_Select(&editor->gizmo, &actor->gizmo, &actor->pos, &actor->rot);
	Gizmo_Focus(&editor->gizmo, &actor->gizmo);
}

static int ActorPropertiesPanel(MenuActor* this, Actor* actor) {
	int numProp;
	DbProperty* listProp;
	int r = 0;
	
	if (!actor || !(actor->state & ACTOR_SELECTED)) return false;
	if (!(numProp = DbActor_NumProperties(actor->id))) return false;
	if (!(listProp = DbActor_Properties(actor->id))) return false;
	
	if (actor->id != this->prevIndex) {
		this->prevIndex = actor->id;
		
		for (int i = 0; i < this->num; i++)
			delete(this->list[i].list, this->list[i].element);
		delete(this->list);
		
		this->num = numProp;
		PropertyEntry* entry = this->list = new(PropertyEntry[this->num]);
		DbProperty* prop = listProp;
		
		for (int i = 0; i < this->num; i++, prop++, entry++) {
			entry->property = prop;
			
			if (prop->numDict) {
				entry->element = new(ElCombo);
				entry->type = PE_COMBO;
				
				Arli* list = entry->list = new(Arli);
				*list = Arli_New(DbDictionary);
				list->begin = (void*)prop->dict;
				list->max = list->num = prop->numDict;
				
				Arli_SetElemNameCallback(list, DatabaseArli_GetNameOfIndex);
				Element_Combo_SetArli(entry->combo, list);
			} else if (smask_bit(prop->mask) == 1) {
				entry->element = new(ElCheckbox);
				entry->type = PE_CHECK;
			} else {
				entry->element = new(ElTextbox);
				entry->type = PE_TEXT;
				entry->textBox->size = smask_byte(prop->mask);
				entry->textBox->align = NVG_ALIGN_RIGHT;
			}
			
			Element_SetNameLerp(entry->check, 0.5f);
			Element_SetName(entry->check, prop->name);
		}
	}
	
	if (!this->num)
		return false;
	
	PropertyEntry* entry = this->list;
	
	for (int i = 0; i < this->num; i++, entry++) {
		if (!entry->element) continue;
		
		DbProperty* prop = entry->property;
		u16 val;
		Arli* list = entry->list;
		DbDictionary* dict;
		
		Element_Row(entry->check, 1.0f);
		
		switch (entry->type) {
			case PE_TEXT:
				if (Element_Textbox(entry->textBox)) {
					r = true;
					Actor_wmask(actor, prop->source, shex(entry->textBox->txt), prop->mask);
				}
				
				val = Actor_rmask(actor, prop->source, prop->mask);
				Element_Textbox_SetText(entry->textBox, x_fmt("%0*X", smask_byte(prop->mask), val));
				break;
				
			case PE_COMBO: {
				int prevIndex = entry->combo->prevIndex;
				
				if (Element_Combo(entry->combo) > -1) {
					r = true;
					dict = Arli_At(list, list->cur);
					Actor_wmask(actor, prop->source, dict->val, prop->mask);
				} else if (prevIndex > -1 && entry->combo->prevIndex == -1)
					osMsg(ICON_WARNING, "Actor %s:\nUnhandled Exception! Seek help immediately!", DbActor_Name(actor->id));
				
				val = Actor_rmask(actor, prop->source, prop->mask);
				DatabaseArli_SetKey(list, val);
			} break;
			
			case PE_CHECK:
				if (Element_Checkbox(entry->check))
					Actor_wmask(actor, prop->source, entry->check->element.toggle, prop->mask);
				
				entry->check->element.toggle = Actor_rmask(actor, prop->source, prop->mask);
		}
	}
	
	return r;
}

static void ReadProperties(MenuActor* this, RoomHeader* room, Actor* actor, bool set) {
	if (actor) {
		if (set) {
			actor->gizmo.refresh = true;
			
			info("refresh actor");
			
			UndoEvent* undo = Undo_New();
			Undo_Register(undo, &actor->pos, sizeof(Vec3f));
			Undo_Register(undo, &actor->rot, sizeof(Vec3s));
			Undo_Register(undo, &actor->id, sizeof(u16));
			Undo_Register(undo, &actor->param, sizeof(u16));
			Undo_Register(undo, &actor->gizmo.refresh, sizeof(u8));
			
			actor->id = shex(this->index.txt);
			actor->param = shex(this->variable.txt);
			actor->rot.x = shex(this->rotX.txt);
			actor->rot.y = shex(this->rotY.txt);
			actor->rot.z = shex(this->rotZ.txt);
			actor->pos.x = sint(this->posX.txt);
			actor->pos.y = sint(this->posY.txt);
			actor->pos.z = sint(this->posZ.txt);
			
			if (this->buttonSelected.state && room) {
				Actor* a = Arli_Head(&room->actorList);
				Actor* aend = a + room->actorList.num;
				
				for (; a < aend; a++) {
					if (a == actor) continue;
					if (!(a->state & ACTOR_SELECTED)) continue;
					
					a->id = actor->id;
					a->param = actor->param;
				}
			}
		}
		
		Element_Textbox_SetText(&this->index, x_fmt("%04X", actor->id));
		Element_Textbox_SetText(&this->variable, x_fmt("%04X", actor->param));
		Element_Textbox_SetText(&this->rotX, x_fmt("%04X", (u32)(u16)actor->rot.x));
		Element_Textbox_SetText(&this->rotY, x_fmt("%04X", (u32)(u16)actor->rot.y));
		Element_Textbox_SetText(&this->rotZ, x_fmt("%04X", (u32)(u16)actor->rot.z));
		Element_Textbox_SetText(&this->posX, x_fmt("%d", (int)actor->pos.x));
		Element_Textbox_SetText(&this->posY, x_fmt("%d", (int)actor->pos.y));
		Element_Textbox_SetText(&this->posZ, x_fmt("%d", (int)actor->pos.z));
	}
}

void MenuActor_Init(Editor* editor, void* __this, Split* split) {
	MenuActor* this = __this;
	RoomHeader* room = Scene_GetRoomHeader(&editor->scene, editor->scene.curRoom);
	
	Element_Combo_SetArli(&this->actorEntry, room ? &room->actorList : NULL);
	
	Actor* actor = room ? Arli_At(&room->actorList, room->actorList.cur) : NULL;
	ReadProperties(this, room, actor, false);
	
	struct {
		ElTextbox*  box;
		const char* name;
	} boxTbl[] = {
		{ &this->index,    "Index"     },
		{ &this->variable, "Variable " },
		{ &this->posX,     "X"         },
		{ &this->posY,     "Y"         },
		{ &this->posZ,     "Z"         },
		{ &this->rotX,     "X"         },
		{ &this->rotY,     "Y"         },
		{ &this->rotZ,     "Z"         },
	};
	
	for (int i = 0; i < ArrCount(boxTbl); i++)
		boxTbl[i].box->size = 4,
		Element_SetName(boxTbl[i].box, boxTbl[i].name);
	
	this->prevIndex = 0xFFFF;
	this->actorEntry.showDecID = true;
	
	this->buttonAdd.element.colOvrdLight = THEME_NEW;
	this->buttonIns.element.colOvrdLight = THEME_PRIM;
	this->buttonRem.element.colOvrdLight = THEME_DELETE;
	
	Element_SetName(&this->buttonSelected, "Group Edit");
	Element_SetName(&this->buttonAdd, "Add");
	Element_SetName(&this->buttonIns, "Insert");
	Element_SetName(&this->buttonRem, "Delete");
	Element_SetName(&this->refreshDatabase, "Refresh");
	Element_Button_SetProperties(&this->buttonSelected, true, 0);
	Element_Button_SetProperties(&this->buttonAdd, 0, 0);
	Element_Button_SetProperties(&this->buttonIns, 0, 0);
	Element_Button_SetProperties(&this->buttonRem, 0, 0);
	Element_Button_SetProperties(&this->refreshDatabase, 0, 0);
	
	this->index.align =
		this->variable.align =
		this->rotX.align =
		this->rotY.align =
		this->rotZ.align =
		this->posX.align =
		this->posY.align =
		this->posZ.align = NVG_ALIGN_RIGHT;
}

void MenuActor_Update(Editor* editor, void* __this, Split* split) {
	MenuActor* this = __this;
	Input* input = &editor->input;
	RoomHeader* room = Scene_GetRoomHeader(&editor->scene, editor->scene.curRoom);
	
	Element_Combo_SetArli(&this->actorEntry, room ? &room->actorList : NULL);
	Arli* list = this->actorEntry.list;
	Actor* actor = list ? Arli_At(list, list->cur) : NULL;
	int set = 0;
	
	Element_Header(split->taskCombo, 92, &this->refreshDatabase, 78, &this->buttonSelected, 92);
	Element_Combo(split->taskCombo);
	Element_Button(&this->buttonSelected);
	if (Element_Button(&this->refreshDatabase)) {
		Database_Refresh();
		this->prevIndex = 0xFFFF;
	}
	
	Element_Row(&this->buttonAdd, 0.333f, &this->buttonIns, 0.333f, &this->buttonRem, 0.333f);
	Element_Separator(false);
	Element_Row(&this->actorEntry, 1.0f);
	Element_Separator(false);
	Element_Row(&this->index, 0.5f, &this->variable, 0.5f);
	
	if (Element_Box(BOX_START, &this->panelPosRot, "Transforms")) {
		Element_Row(&this->posX, 0.5f, &this->rotX, 0.5f);
		Element_Row(&this->posY, 0.5f, &this->rotY, 0.5f);
		Element_Row(&this->posZ, 0.5f, &this->rotZ, 0.5f);
		
		Element_Condition(&this->rotX, actor != NULL && actor->state & ACTOR_SELECTED);
		Element_Condition(&this->rotY, actor != NULL && actor->state & ACTOR_SELECTED);
		Element_Condition(&this->rotZ, actor != NULL && actor->state & ACTOR_SELECTED);
		Element_Condition(&this->posX, actor != NULL && actor->state & ACTOR_SELECTED);
		Element_Condition(&this->posY, actor != NULL && actor->state & ACTOR_SELECTED);
		Element_Condition(&this->posZ, actor != NULL && actor->state & ACTOR_SELECTED);
		
		set += !!Element_Textbox(&this->rotX);
		set += !!Element_Textbox(&this->rotY);
		set += !!Element_Textbox(&this->rotZ);
		set += !!Element_Textbox(&this->posX);
		set += !!Element_Textbox(&this->posY);
		set += !!Element_Textbox(&this->posZ);
		
	}
	Element_Box(BOX_END, &this->panelPosRot);
	
	Element_Condition(&this->buttonAdd,  list != NULL);
	Element_Condition(&this->actorEntry, actor != NULL);
	Element_Condition(&this->buttonRem,  actor != NULL && actor->state & ACTOR_SELECTED);
	Element_Condition(&this->index,      actor != NULL && actor->state & ACTOR_SELECTED);
	Element_Condition(&this->variable,   actor != NULL && actor->state & ACTOR_SELECTED);
	
	if (editor->searchMenu) {
		int ret;
		Element_Disable(&this->index);
		
		switch (ActorSearchContextMenu_State(&ret)) {
			case 1:
				strcpy(this->index.txt, x_fmt("%04X", ret));
				set++;
				break;
				
			case -1:
				ActorSearchContextMenu_Free();
				break;
				
			default:
				break;
		}
	}
	
	if (Element_Textbox(&this->index))
		set++;
	
	else if (Element_Operatable(&this->index))
		if (Input_SelectClick(input, CLICK_R))
			ActorSearchContextMenu_New(Rect_AddPos(this->index.element.rect, split->dispRect), actor->id);
	
	set += !!Element_Textbox(&this->variable);
	
	if (Element_Button(&this->buttonAdd) && !set) {
		Actor new = { .id = 0x0015 };
		
		actor = Arli_Add(list, &new);
		SelectActor(editor, room, actor);
		set = false;
	}
	
	if (Element_Button(&this->buttonIns) && !set) {
		Actor new = { .id = 0x0015 };
		
		actor = Arli_Insert(list, list->cur, 1, &new);
		SelectActor(editor, room, actor);
		set = false;
	}
	
	if (Element_Button(&this->buttonRem) && !set) {
		_log("remove");
		Arli_Remove(list, list->cur, 1);
		_log("at");
		actor = Arli_At(list, list->cur);
		
		SelectActor(editor, room, actor);
		set = false;
	}
	
	if (Element_Combo(&this->actorEntry) > -1) {
		Actor* actor = Arli_At(list, list->cur);
		
		SelectActor(editor, room, actor);
	}
	
	ReadProperties(this, room, actor, set);
	if (Element_Box(BOX_START, &this->panelProperties, "Properties"))
		ActorPropertiesPanel(this, actor);
	Element_Box(BOX_END, &this->panelProperties);
}
