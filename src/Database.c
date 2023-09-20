#include "Database.h"
#include "Editor.h"

typedef struct DbRender {
	void* object;
	u8    segment;
	u32   dlist;
	u32   skelAnime;
	u32   anim;
	f32   scale;
	char  regex[64];
} DbRender;

typedef struct {
	u16 index;
	u16 object;
	const char* name;
	
	DbVariable* variable;
	int numVariable;
	
	DbProperty* property;
	int numProperty;
	
	DbRender* render;
	int       numRender;
} DbActor;

static DbActor** sDatabaseActor;
static int sDatabaseNum;
static const char* sRomProjectFolder = NULL;

////////////////////////////////////////////////////////////////////////////////

static void FreeDbDict(DbDictionary* this, int num) {
	for (int i = 0; i < num; i++, this++)
		delete(this->text);
}

////////////////////////////////////////////////////////////////////////////////

static void ActorPropEnt_ParseVariable(Toml* toml, DbActor* entry, int i) {
	entry->numVariable = Toml_ArrCount(toml, "actor[%d].variable", i);
	if (!entry->numVariable) return;
	
	DbVariable* v = entry->variable = new(DbVariable[entry->numVariable]);
	
	for (int k = 0; k < entry->numVariable; k++, v++) {
		v->variable = Toml_GetInt(toml, "actor[%d].variable[%d][0]", i, k);
		v->text = Toml_GetStr(toml, "actor[%d].variable[%d][1]", i, k);
	}
}

static void ActorPropEnt_ParsePropDict(Toml* toml, DbProperty* p, int i, int k) {
	const char* table = NULL;
	
	p->numDict = Toml_ArrCount(toml, "actor[%d].property[%d].variable", i, k);
	
	if (!p->numDict) {
		if (!Toml_Var(toml, "actor[%d].property[%d].variable", i, k))
			return;
		
		table = Toml_GetStr(toml, "actor[%d].property[%d].variable", i, k);
		
		if (!(p->numDict = Toml_ArrCount(toml, table)))
			goto bail_out;
	} else
		table = fmt("actor[%d].property[%d].variable", i, k);
	
	DbDictionary* de = p->dict = new(DbDictionary[p->numDict]);
	
	for (int j = 0; j < p->numDict; j++, de++) {
		de->val = Toml_GetInt(toml, "%s[%d][0]", table, j);
		de->text = Toml_GetStr(toml, "%s[%d][1]", table, j);
	}
	
	bail_out:
	delete(table);
}

static void ActorPropEnt_ParseProperty(Toml* toml, DbActor* entry, int i) {
	entry->numProperty = Toml_ArrCount(toml, "actor[%d].property", i);
	if (!entry->numProperty) return;
	
	DbProperty* p = entry->property = new(DbProperty[entry->numProperty]);
	
	for (int k = 0; k < entry->numProperty; k++, p++) {
		const char* src;
		const struct {
			char* text;
			int   value;
		} sourceTbl[] = {
			{ "var_t",   DB_VAR   },
			{ "pos_x", DB_POSX  },
			{ "pos_y", DB_POSY  },
			{ "pos_z", DB_POSZ  },
			{ "rot_x", DB_ROTX  },
			{ "rot_y", DB_ROTY  },
			{ "rot_z", DB_ROTZ  },
		};
		
		p->name = Toml_GetStr(toml, "actor[%d].property[%d].name", i, k);
		p->mask = Toml_GetInt(toml, "actor[%d].property[%d].mask", i, k);
		src = Toml_GetStr(toml, "actor[%d].property[%d].source", i, k);
		
		if (!p->mask) {
			errr(
				"" PRNT_REDD "D a t a b a s e   E r r o r !\n" PRNT_RSET
				"Null Mask"
				"\n"
				"[[" PRNT_BLUE "actor" PRNT_RSET "]]\n"
				"\tindex = 0x%04X\n"
				"\tname = \"%s\"\n"
				"\t[[" PRNT_BLUE "actor.property" PRNT_RSET "]]\n"
				"\t\tname = \"%s\"\n"
				"\t\tmask = 0x%04X\n"
				"\t\tsource = \"%s\"\n",
				entry->index, entry->name,
				p->name, p->mask, src);
		}
		
		for (int j = 0; src && j < ArrCount(sourceTbl); j++)
			if (streq(src, sourceTbl[j].text))
				p->source = sourceTbl[j].value;
		delete(src);
		
		ActorPropEnt_ParsePropDict(toml, p, i, k);
	}
}

static void ActorPropEnt_ParseRender(Toml* toml, DbActor* entry, int i) {
	entry->numRender = Toml_ArrCount(toml, "actor[%d].render", i);
	if (!entry->numRender) return;
	
	DbRender* render = entry->render = new(DbRender[entry->numRender]);
	
	for (int j = 0; j < entry->numRender; j++, render++) {
		struct {
			u32* u32;
			f32* f32;
			const char* path;
		} v[] = {
			{
				.u32 = &render->anim,
				.path = "actor[%d].render[%d].animation"
			},{
				.u32 = &render->dlist,
				.path = "actor[%d].render[%d].displist"
			},{
				.u32 = &render->skelAnime,
				.path = "actor[%d].render[%d].skeleton"
			},{
				.f32 = &render->scale,
				.path = "actor[%d].render[%d].scale"
			},
		};
		
		render->scale = 0.01f;
		
		for (int k = 0; k < ArrCount(v); k++) {
			if (Toml_Var(toml, v[k].path, i, j)) {
				if (v[k].u32)
					*v[k].u32 = Toml_GetInt(toml, v[k].path, i, j);
				if (v[k].f32)
					*v[k].f32 = Toml_GetFloat(toml, v[k].path, i, j);
			}
		}
		
		if (render->dlist)
			render->segment = rmask(render->dlist, 0xFF000000);
		else if (render->skelAnime)
			render->segment = rmask(render->skelAnime, 0xFF000000);
		
		if (Toml_Var(toml, "actor[%d].render[%d].regex", i, j)) {
			char* s = Toml_GetStr(toml, "actor[%d].render[%d].regex", i, j);
			
			strncpy(render->regex, s, 64);
			delete(s);
		} else {
			strcpy(render->regex, "....");
		}
		
		osMsg(
			ICON_INFO,
			"Registered Render Info:\n"
			"DisplayList: %08X\n"
			"SkelAnime:   %08X\n"
			"Animation:   %08X",
			render->dlist, render->skelAnime, render->anim);
	}
}

////////////////////////////////////////////////////////////////////////////////

typedef struct {
	const char*   name;
	DbDictionary* dict;
	int  numDict;
	Arli arli;
} ScenePropertyEntry;

static ScenePropertyEntry sSceneEntry[SCENE_DATA_MAX];

static const char* sExpected[SCENE_DATA_MAX] = {
	"file",
	"options",
	"room.title_time",
	"room.title_env",
	"room.title_light",
	"room.global",
	"room.time_speed",
	"room.echo",
	"room.skybox",
	"room.sunmoon",
	"room.behaviour1",
	"room.behaviour2",
};

////////////////////////////////////////////////////////////////////////////////

const char* DatabaseArli_GetNameOfIndex(Arli* arli, size_t index) {
	DbDictionary* dict = Arli_At(arli, index);
	
	_log("get index: %d", index);
	if (!dict) return "Unknown";
	return dict->text;
}

int DatabaseArli_GetKeyOfIndex(Arli* arli, size_t index) {
	DbDictionary* dict = Arli_At(arli, index);
	
	_log("get index: %d", index);
	if (!dict) return -1;
	return dict->val;
}

void DatabaseArli_SetKey(Arli* arli, size_t index) {
	DbDictionary* dict = Arli_Head(arli);
	DbDictionary* end = dict + arli->num;
	
	arli->cur = -1;
	
	for (; dict < end; dict++) {
		if (dict->val == index) {
			arli->cur = Arli_IndexOf(arli, dict);
			return;
		}
	}
}

int DatabaseArli_GetKey(Arli* arli) {
	return DatabaseArli_GetKeyOfIndex(arli, arli->cur);
}

////////////////////////////////////////////////////////////////////////////////

Arli* DatabaseScene_GetList(ContextDatabase type) {
	return &sSceneEntry[type].arli;
}

const char* DatabaseScene_GetName(ContextDatabase type) {
	return sSceneEntry[type].name;
}

void DatabaseScene_SetKey(ContextDatabase type, int index) {
	DatabaseArli_SetKey(&sSceneEntry[type].arli, index);
}

int DatabaseScene_GetKey(ContextDatabase type) {
	return DatabaseArli_GetKey(&sSceneEntry[type].arli);
}

////////////////////////////////////////////////////////////////////////////////

void Database_Init() {
	Toml toml = Toml_New();
	const char* actor_property_toml = "resources/actor_property.toml";
	const char* scene_property_toml = "resources/scene_property.toml";
	
	if (!sys_stat(scene_property_toml))
		errr("Missing Resource: %s", scene_property_toml);
	
	if (sys_stat(actor_property_toml)) {
		Toml_Load(&toml, actor_property_toml);
		
		int num = Toml_ArrCount(&toml, "actor[]");
		
		for (int i = 0; i < num; i++) {
			int index = Toml_GetInt(&toml, "actor[%d].index", i);
			
			sDatabaseNum = Max(sDatabaseNum, index);
		}
		
		sDatabaseActor = new(DbActor[sDatabaseNum]);
		
		for (int i = 0; i < num; i++) {
			DbActor* entry = new(DbActor);
			
			entry->index = Toml_GetInt(&toml, "actor[%d].index", i);
			entry->name = Toml_GetStr(&toml, "actor[%d].name", i);
			entry->object = 0xFFFF;
			
			_log(
				"[[" PRNT_BLUE "actor" PRNT_RSET "]]\n"
				"\tindex = 0x%04X\n"
				"\tname = \"%s\"\n",
				entry->index, entry->name);
			
			if (Toml_Var(&toml, "actor[%d].object", i))
				entry->object = Toml_GetInt(&toml, "actor[%d].object", i);
			
			_log("actor[%d].variable", i);
			ActorPropEnt_ParseVariable(&toml, entry, i);
			_log("actor[%d].property", i);
			ActorPropEnt_ParseProperty(&toml, entry, i);
			ActorPropEnt_ParseRender(&toml, entry, i);
			
			sDatabaseActor[entry->index] = entry;
		}
		
		Toml_Free(&toml);
	}
	
	ScenePropertyEntry* entry = sSceneEntry;
	Toml_Load(&toml, scene_property_toml);
	
	for (int i = 0; i < ArrCount(sSceneEntry); i++, entry++) {
		if (Toml_Var(&toml, "%s.name", sExpected[i])) {
			entry->name = Toml_GetStr(&toml, "%s.name", sExpected[i]);
			entry->numDict = Toml_ArrCount(&toml, "%s.variable", sExpected[i]);
			entry->arli = Arli_New(DbDictionary);
			
			DbDictionary* dict = entry->dict = new(DbDictionary[entry->numDict]);
			
			entry->arli.num = entry->numDict;
			entry->arli.begin = (void*)dict;
			entry->arli.elemSize = sizeof(DbDictionary);
			
			Arli_SetElemNameCallback(&entry->arli, DatabaseArli_GetNameOfIndex);
			
			for (int k = 0; k < entry->numDict; k++, dict++) {
				dict->val = Toml_GetInt(&toml, "%s.variable[%d][0]", sExpected[i], k);
				dict->text = Toml_GetStr(&toml, "%s.variable[%d][1]", sExpected[i], k);
			}
			
		} else if (Toml_Var(&toml, "%s", sExpected[i]))
			entry->name = Toml_GetStr(&toml, "%s", sExpected[i]);
		
		else
			errr("Missing \"%s\" from \"scene_property.toml\"", sExpected[i]);
	}
	
	Toml_Free(&toml);
}

void Database_Free() {
	_log("Free");
	
	for (int i = 0; i < sDatabaseNum; i++) {
		DbActor* e = sDatabaseActor[i];
		
		if (!e) continue;
		
		DbVariable* vari = e->variable;
		
		for (int j = 0; j < e->numVariable; j++, vari++)
			delete(vari->text);
		
		DbProperty* prop = e->property;
		
		for (int j = 0; j < e->numProperty; j++, prop++) {
			FreeDbDict(prop->dict, prop->numDict);
			delete(prop->name, prop->dict);
		}
		
		delete(e->name, e->variable, e->property, e);
	}
	
	ScenePropertyEntry* entry = sSceneEntry;
	for (int i = 0; i < ArrCount(sSceneEntry); i++, entry++) {
		DbDictionary* dict = entry->dict;
		
		for (int k = 0; k < entry->numDict; k++, dict++)
			delete(dict->text);
		delete(entry->name, entry->dict);
		memset(entry, 0, sizeof(*entry));
	}
	
	delete(sDatabaseActor);
	sDatabaseNum = 0;
}

void Database_Refresh() {
	Database_Free();
	Database_Init();
}

////////////////////////////////////////////////////////////////////////////////

typedef struct DbActorSearchMenu {
	NanoGrid  nano;
	Split     split;
	ElTextbox textboxSearch;
	
	DbActor* entryList;
	int      numEntry;
	int      change;
	int      current;
	
	s8    init;
	s8    initialSearch;
	Vec2s initialCursorPos;
} DbActorSearchMenu;

static void ActorSearchContextMenu_Init(NanoGrid* __no_no, ContextMenu* contextMenu) {
	DbActorSearchMenu* this = contextMenu->udata;
	Input* input = &GetEditor()->input;
	
	NanoGrid_Init(&this->nano, &GetEditor()->window, NULL);
	
	this->textboxSearch.clearIcon = true;
	this->textboxSearch.align = NVG_ALIGN_LEFT;
	this->entryList = new(DbActor[sDatabaseNum]);
	
	this->initialSearch = 1;
	this->initialCursorPos = input->cursor.pos;
	
	contextMenu->rect.w = 128 + 64;
	contextMenu->rect.h = SPLIT_ELEM_X_PADDING * 3 + SPLIT_TEXT_H * 17;
}

static void ActorSearchContextMenu_Draw(NanoGrid* __no_no, ContextMenu* contextMenu) {
	DbActorSearchMenu* this = contextMenu->udata;
	NanoGrid* nano = &this->nano;
	void* vg = nano->vg;
	Input* input = nano->input;
	int focusSlot = -1;
	
	DummyGrid_Push(nano);
	DummySplit_Push(nano, &this->split, contextMenu->rect);
	
	this->textboxSearch.element.rect = contextMenu->rect;
	this->textboxSearch.element.rect.h = SPLIT_TEXT_H;
	this->textboxSearch.element.rect.x += SPLIT_ELEM_X_PADDING;
	this->textboxSearch.element.rect.y += SPLIT_ELEM_X_PADDING;
	this->textboxSearch.element.rect.w -= SPLIT_ELEM_X_PADDING * 2;
	
	Element_Textbox(&this->textboxSearch);
	Element_SetActiveTextbox(nano, &this->split, &this->textboxSearch);
	
	Input_SetState(input, INPUT_PERMISSIVE);
	
	if (this->textboxSearch.modified || !this->init) {
		bool textMatch = strlen(this->textboxSearch.txt) ? true : false;
		
		focusSlot = 0;
		this->numEntry = 0;
		
		for (int i = 0; i < sDatabaseNum; i++) {
			if (!sDatabaseActor[i])
				continue;
			
			if (!textMatch) {
				this->entryList[this->numEntry++] = *sDatabaseActor[i];
			} else {
				if (stristr(sDatabaseActor[i]->name, this->textboxSearch.txt))
					this->entryList[this->numEntry++] = *sDatabaseActor[i];
			}
			
			if (!this->init && this->current == this->entryList[this->numEntry - 1].index)
				focusSlot = this->numEntry - 1;
		}
		
		ScrollBar_Init(&contextMenu->scroll, this->numEntry, SPLIT_TEXT_H);
	}
	
	Rect mainr = contextMenu->rect;
	
	mainr = Rect_Scale(mainr, -SPLIT_ELEM_X_PADDING, -SPLIT_ELEM_X_PADDING);
	mainr = Rect_ShrinkY(mainr, (SPLIT_TEXT_H + SPLIT_ELEM_X_PADDING) * 2);
	mainr = Rect_Translate(mainr, 0, SPLIT_TEXT_H + SPLIT_ELEM_X_PADDING);
	
	Gfx_DrawRounderOutline(vg, mainr, Theme_GetColor(THEME_ELEMENT_LIGHT, 255, 1.0f));
	Gfx_DrawRounderRect(vg, mainr, Theme_GetColor(THEME_ELEMENT_DARK, 255, 1.0f));
	
	ScrollBar_FocusSlot(&contextMenu->scroll, focusSlot, false);
	int busy = ScrollBar_Update(&contextMenu->scroll, input, input->cursor.pos, mainr, mainr);
	
	nvgScissor(vg, UnfoldRect(mainr));
	for (int i = 0; i < this->numEntry; i++) {
		Rect r = ScrollBar_GetRect(&contextMenu->scroll, i);
		DbActor* entry = &this->entryList[i];
		
		if (!IsBetween(r.y, mainr.y - r.h, mainr.y + mainr.h))
			continue;
		
		if (!busy && Rect_PointIntersect(&r, UnfoldVec2(input->cursor.pos))) {
			if (Input_GetCursor(input, CLICK_L)->dual && entry->index == this->current)
				contextMenu->state.setCondition = true;
			else if (Input_SelectClick(input, CLICK_L))
				this->current = this->change = entry->index;
		}
		
		Gfx_DrawRounderRect(vg, r, Theme_GetColor(THEME_ELEMENT_BASE, 80, 1.0f));
		if (entry->index == this->current)
			Gfx_DrawRounderRect(vg, r, Theme_GetColor(THEME_PRIM, 200, 1.0f));
		Gfx_Text(vg, r, NVG_ALIGN_LEFT, Theme_GetColor(THEME_TEXT, 255, 1.0f), entry->name);
	}
	nvgResetScissor(vg);
	
	ScrollBar_Draw(&contextMenu->scroll, vg);
	
	DummySplit_Pop(nano, &this->split);
	DummyGrid_Pop(nano);
	this->init = true;
	
	Input_ClearState(input, INPUT_PERMISSIVE);
}

////////////////////////////////////////////////////////////////////////////////

void ActorSearchContextMenu_New(Rect rect, u16 id) {
	Editor* editor = GetEditor();
	
	if (!sDatabaseNum) return;
	
	editor->searchMenu = new(DbActorSearchMenu);
	editor->searchMenu->change = -1;
	editor->searchMenu->current = id;
	ContextMenu_Custom(&editor->nano, editor->searchMenu, NULL, ActorSearchContextMenu_Init, ActorSearchContextMenu_Draw, NULL, rect);
}

int ActorSearchContextMenu_State(int* ret) {
	Editor* editor = GetEditor();
	NanoGrid* nano = &editor->nano;
	DbActorSearchMenu* this = editor->searchMenu;
	
	if (nano->contextMenu.udata != this)
		return -1;
	if (this->change > -1) {
		*ret = this->change;
		this->change = -1;
		
		return 1;
	}
	return 9;
}

void ActorSearchContextMenu_Free() {
	Editor* editor = GetEditor();
	DbActorSearchMenu* this = editor->searchMenu;
	
	Element_ClearActiveTextbox(&this->nano);
	delete(this->nano.elemState, this->entryList, editor->searchMenu);
}

////////////////////////////////////////////////////////////////////////////////

const char* DbActor_Name(u16 index) {
	if (index < sDatabaseNum && sDatabaseActor[index]) {
		if (sDatabaseActor[index]->name)
			return sDatabaseActor[index]->name;
		return x_fmt("missingnm_%04X", index);
	}
	return x_fmt("unk_%04X", index);
}

u16 DbActor_ObjectID(u16 index) {
	if (index < sDatabaseNum && sDatabaseActor[index])
		return sDatabaseActor[index]->object;
	return 0xFFFF;
}

DbProperty* DbActor_Properties(u16 index) {
	if (index < sDatabaseNum && sDatabaseActor[index])
		return sDatabaseActor[index]->property;
	return NULL;
}

int DbActor_NumProperties(u16 index) {
	if (index < sDatabaseNum && sDatabaseActor[index])
		return sDatabaseActor[index]->numProperty;
	return 0;
}

DbVariable* DbActor_Variables(u16 index) {
	if (index < sDatabaseNum && sDatabaseActor[index])
		return sDatabaseActor[index]->variable;
	return NULL;
}

int DbActor_NumVariables(u16 index) {
	if (index < sDatabaseNum && sDatabaseActor[index])
		return sDatabaseActor[index]->numVariable;
	return 0;
}

void* sRenderFile[0x1000];

static DbRender* DbActor_GetRender(u16 index) {
	if (index < sDatabaseNum && sDatabaseActor[index]) {
		if (sDatabaseActor[index]->render)
			sDatabaseActor[index]->render->object = sRenderFile[DbActor_ObjectID(index)];
		
		return sDatabaseActor[index]->render;
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

static void ClearRenderFiles(void) {
	void** a = sRenderFile;
	void** end = sRenderFile + ArrCount(sRenderFile);
	
	for (; a < end; a++)
		delete(*a);
}

static int sLoadedModObj;
static int sLoadedVanObj;

static void LoadRenderfiles(void) {
	List list = List_New();
	const char* path = fmt("%s/rom/object/", sRomProjectFolder);
	
	List_SetFilters(&list, CONTAIN_START, "0x");
	List_Walk(&list, path, 0, LIST_FOLDERS | LIST_RELATIVE);
	
	sLoadedModObj = 0;
	sLoadedVanObj = 0;
	
	forlist(item, list) {
		u32 index = shex(item);
		const char* lf = x_fmt("%s/%s/object.zobj", path, item);
		
		if (sys_stat(lf) && index < 0x1000)
			sRenderFile[index] = loadbin(lf),
			sLoadedModObj++;
	}
	
	delete(path); path = fmt("%s/rom/object/.vanilla/", sRomProjectFolder);
	List_FreeItems(&list);
	
	List_Walk(&list, path, 0, LIST_FOLDERS | LIST_RELATIVE);
	
	forlist(item, list) {
		u32 index = shex(item);
		const char* lf = x_fmt("%s/%s/object.zobj", path, item);
		
		if (!sRenderFile[index])
			if (sys_stat(lf) && index < 0x1000)
				sRenderFile[index] = loadbin(lf),
				sLoadedVanObj++;
	}
	
	List_Free(&list);
}

////////////////////////////////////////////////////////////////////////////////

extern DataFile gCube;
#include "../assets/3D/Cube.h"

static u32 GetRenderUUID(DbRender* this) {
	u32 uuid = this->dlist | this->skelAnime;
	
	uuid |= wmask(!!this->skelAnime, 0x80000000);
	uuid |= wmask(!!this->dlist, 0x40000000);
	
	return uuid;
}

void DatabaseRender_Render(Actor* this) {
	DbRender* render = DbActor_GetRender(this->id);
	Vec3f pos = this->pos;
	Vec3s rot = this->rot;
	
	if (this->gizmo.interact && this->gizmo.selected) {
		pos = this->gizmo.pos;
		rot = this->gizmo.rot;
	}
	
	Matrix_Translate(UnfoldVec3(pos), MTXMODE_NEW);
	Matrix_RotateY_s(rot.y, MTXMODE_APPLY);
	Matrix_RotateX_s(rot.x, MTXMODE_APPLY);
	Matrix_RotateZ_s(rot.z, MTXMODE_APPLY);
	
	// Check Render
	if (render) {
		if (!render->object)
			render = NULL;
		else if (render->skelAnime && render->anim);
		else if (render->dlist);
		else
			render = NULL;
	}
	
	// Scale & Segment
	if (render) {
		_log("Segment: %02X %08X", render->segment, render->object);
		_assert(render->object != NULL);
		gSegment[render->segment] = render->object;
		gSPSegment(POLY_OPA_DISP++, render->segment, render->object);
		Matrix_Scale(render->scale, render->scale, render->scale, MTXMODE_APPLY);
	} else {
		gSegment[6] = gCube.data;
		gSPSegment(POLY_OPA_DISP++, 6, gCube.data);
		gDPSetEnvColor(POLY_OPA_DISP++, 0xF0, 0xF0, 0xF0, 0xFF);
		Matrix_Scale(0.01, 0.01, 0.01, MTXMODE_APPLY);
	}
	
	gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
	
	if (this->state & ACTOR_SELECTED) {
		gDPPipeSync(POLY_OPA_DISP++);
		gDPSetEnvColor(POLY_OPA_DISP++, 0xC0, 0xC0, 0xC0, 0xFF);
		if (this->gizmo.focus)
			gXPSetHighlightColor(POLY_OPA_DISP++, 0xFF, 0x85, 0x00, 0x80, DODGE);
		else
			gXPSetHighlightColor(POLY_OPA_DISP++, 0xFF, 0x85, 0x00, 0xC0, MUL);
	}
	
	if (render) {
		u32 renderUUID = GetRenderUUID(render);
		
		if (this->renderUUID != renderUUID) {
			if (render->skelAnime) {
				SkelAnime_Free(&this->skelAnime);
				SkelAnime_Init(&this->skelAnime, render->skelAnime, render->anim);
				info("Initialize Skelanime: %08X", renderUUID);
			}
			
			this->renderUUID = renderUUID;
		}
		
		if (render->skelAnime) {
			SkelAnime_Update(&this->skelAnime);
			SkelAnime_Draw(&this->skelAnime, SKELANIME_FLEX);
		} else
			gSPDisplayList(POLY_OPA_DISP++, render->dlist);
	} else
		gSPDisplayList(POLY_OPA_DISP++, gCube_DlActorEntry);
	
	if (this->state & ACTOR_SELECTED) {
		gDPPipeSync(POLY_OPA_DISP++);
		gXPClearHighlightColor(POLY_OPA_DISP++);
		gDPSetEnvColor(POLY_OPA_DISP++, 0xF0, 0xF0, 0xF0, 0xFF);
	}
}

////////////////////////////////////////////////////////////////////////////////

onexit_func_t DeleteRomProject() {
	delete(sRomProjectFolder);
}

void LoadRomProject(const char* file) {
	
	if (sys_stat(file)) {
		_log("Load Project: %s", file);
		ClearRenderFiles();
		delete(sRomProjectFolder);
		sRomProjectFolder = path(file);
		*strend(sRomProjectFolder, "/") = '\0';
		LoadRenderfiles();
		
		osMsg(ICON_TRASH,
			"z64rom project:  %s\n"
			"Mod objects:     %d\n"
			"Vanilla objects: %d",
			file, sLoadedModObj, sLoadedVanObj);
	}
}
