#include "Properties.h"

void Properties_Init(Editor* editor, Properties* this, Split* split);
void Properties_Destroy(Editor* editor, Properties* this, Split* split);
void Properties_Update(Editor* editor, Properties* this, Split* split);
void Properties_Draw(Editor* editor, Properties* this, Split* split);

static void SaveConfig(void* __this, Split* split, Toml* toml, const char* prefix) {
	Properties* this = __this;
	
	Toml_SetVar(toml, x_fmt("%s.properties.tab_index", prefix), "%d", this->subIndex);
}

static void LoadConfig(void* __this, Split* split, Toml* toml, const char* prefix) {
	Properties* this = __this;
	
	this->subIndex = Toml_GetInt(toml, "%s.properties.tab_index", prefix);
}

SplitTask gPropertiesTask = {
	.taskName   = "Properties",
	.init       = (void*)Properties_Init,
	.destroy    = (void*)Properties_Destroy,
	.update     = (void*)Properties_Update,
	.draw       = (void*)Properties_Draw,
	.saveConfig = SaveConfig,
	.loadConfig = LoadConfig,
	.size       = sizeof(Properties)
};

void Properties_BasicHeader(Split* split) {
	Element_Header(split->taskCombo, 92);
	Element_Combo(split->taskCombo);
}

///////////////////////////////////////////////////////////////////////////////

static void MenuDebug_Init(Editor* editor, void* __this, Split* split) {
	MenuDebug* this = __this;
	Scene* scene = &editor->scene;
	
	Element_SetName(&this->envAmbient, "Ambient");
	Element_SetName(&this->envColA, "EnvA");
	Element_SetName(&this->envColB, "EnvB");
	Element_SetName(&this->envFogColor, "Fog");
	
	Element_SetName(&this->killScene, "Unload Scene");
	Element_SetName(&this->buttonUnused, "Unused");
	Element_SetName(&this->buttonFog, "Fog");
	Element_SetName(&this->buttonCulling, "Culling");
	Element_SetName(&this->buttonColView, "Collision");
	
	Element_SetName(&this->buttonIndoor, "Indoor");
	
	Element_SetName(&this->fogFar, "Far");
	Element_SetName(&this->fogNear, "Near");
	
	Element_Slider_SetParams(&this->fogNear, 0, 1000, "int");
	Element_Slider_SetParams(&this->fogFar, 100, 32000, "int");
	
	Element_Button_SetProperties(&this->killScene, 0, 0);
	Element_Button_SetProperties(&this->buttonUnused, true, false);
	Element_Button_SetProperties(&this->buttonFog, true, scene->state & SCENE_DRAW_FOG);
	Element_Button_SetProperties(&this->buttonCulling, true, scene->state & SCENE_DRAW_CULLING);
	Element_Button_SetProperties(&this->buttonColView, true, scene->state & SCENE_DRAW_COLLISION);
	this->killScene.align =
		this->buttonUnused.align =
		this->buttonFog.align =
		this->buttonCulling.align =
		this->buttonColView.align = NVG_ALIGN_CENTER;
}

static void MenuDebug_Update(Editor* editor, void* __this, Split* split) {
	MenuDebug* this = __this;
	Scene* scene = &editor->scene;
	SceneHeader* sceneHeader = Scene_GetCurSceneHeader(scene);
	EnvLights* envSettings = sceneHeader ? Arli_At(&sceneHeader->envList, sceneHeader->envList.cur) : NULL;
	
	Properties_BasicHeader(split);
	
	Element_Condition(&this->buttonIndoor, scene->segment != NULL);
	Element_Condition(&this->killScene, scene->segment != NULL);
	
	if (editor->scene.segment) {
		Element_Color_SetColor(&this->envAmbient, envSettings->ambientColor);
		Element_Color_SetColor(&this->envColA, envSettings->light1Color);
		Element_Color_SetColor(&this->envColB, envSettings->light2Color);
		Element_Color_SetColor(&this->envFogColor, envSettings->fogColor);
		
		Element_Slider_SetValue(&this->fogNear, envSettings->fogNear & 0x3FF);
		Element_Slider_SetValue(&this->fogFar, envSettings->fogFar);
		
	} else {
		Element_Color_SetColor(&this->envAmbient, NULL);
		Element_Color_SetColor(&this->envColA, NULL);
		Element_Color_SetColor(&this->envColB, NULL);
		Element_Color_SetColor(&this->envFogColor, NULL);
	}
	
	Element_Row(&this->envAmbient, 1.0f);
	Element_Color(&this->envAmbient);
	
	Element_Row(&this->envColA, 1.0f);
	Element_Row(&this->envColB, 1.0f);
	Element_Color(&this->envColA);
	Element_Color(&this->envColB);
	
	Element_Box(BOX_START);
	Element_Row(&this->envFogColor, 1.0f);
	Element_Color(&this->envFogColor);
	
	Element_Row(&this->fogNear, 0.5f, &this->fogFar, 0.5f);
	
	if (Element_Slider(&this->fogNear))
		if (envSettings) envSettings->fogNear = Element_Slider_GetValue(&this->fogNear);
	
	if (Element_Slider(&this->fogFar))
		if (envSettings) envSettings->fogFar = Element_Slider_GetValue(&this->fogFar);
	
	Element_Box(BOX_END);
	
	Element_Separator(false);
	
	Element_Row( &this->buttonUnused, 0.5f, &this->buttonCulling, 0.5f);
	Element_Row( &this->buttonFog, 0.5f, &this->buttonColView, 0.5f);
	
	Element_Button_SetProperties(&this->buttonFog, true, scene->state & SCENE_DRAW_FOG);
	Element_Button_SetProperties(&this->buttonCulling, true, scene->state & SCENE_DRAW_CULLING);
	Element_Button_SetProperties(&this->buttonColView, true, scene->state & SCENE_DRAW_COLLISION);
	
	Element_Button(&this->buttonUnused);
	if (Element_Button(&this->buttonFog))
		Scene_SetState(scene, SCENE_DRAW_FOG, this->buttonFog.state);
	if (Element_Button(&this->buttonCulling))
		Scene_SetState(scene, SCENE_DRAW_CULLING, this->buttonCulling.state);
	if (Element_Button(&this->buttonColView))
		Scene_SetState(scene, SCENE_DRAW_COLLISION, this->buttonColView.state);
	
	Element_Separator(false);
	
	Element_Row(&this->killScene, 1.0f);
	if (Element_Button(&this->killScene)) {
		Gizmo_UnselectAll(&editor->gizmo);
		Actor_UnselectAll(&editor->scene, NULL);
		Scene_Kill(&editor->scene);
		
		Element_Color_SetColor(&this->envAmbient, NULL);
		Element_Color_SetColor(&this->envColA, NULL);
		Element_Color_SetColor(&this->envColB, NULL);
		Element_Color_SetColor(&this->envFogColor, NULL);
	}
}

///////////////////////////////////////////////////////////////////////////////

void MenuActor_Init(Editor* editor, void* __this, Split* split);
void MenuActor_Update(Editor* editor, void* __this, Split* split);
void MenuEnv_Init(Editor* editor, void* __this, Split* split);
void MenuEnv_Update(Editor* editor, void* __this, Split* split);
void MenuRoomEnv_Init(Editor* editor, void* __this, Split* split);
void MenuRoomEnv_Update(Editor* editor, void* __this, Split* split);

///////////////////////////////////////////////////////////////////////////////

static struct {
	void     (*init)(Editor*, void*, Split*);
	void     (*update)(Editor*, void*, Split*);
	off_t    offset;
	int      icon;
	NVGcolor color;
} sSubMenuParam[] = {
	//crustify
	{ MenuActor_Init,   MenuActor_Update,   offsetof(Properties, menuActor),   ICON_GHOST,   { 230.0 / 360, 0.5, 0.5, 255 } },
	{ MenuEnv_Init,     MenuEnv_Update,     offsetof(Properties, menuEnv),     ICON_LIGHT, { 50.0 / 360, 0.5, 0.5, 255 } },
	{ MenuRoomEnv_Init, MenuRoomEnv_Update, offsetof(Properties, menuRoomEnv), ICON_WIND, { 170.0 / 360, 0.5, 0.5, 255 } },
	
	
	{ MenuDebug_Init,   MenuDebug_Update,   offsetof(Properties, menuDebug),   ICON_TERMINAL, { 350.0 / 360, 0.7, 0.5, 255 } },
	//uncrustify
};

#define SIDE_BUTTON_SIZE (24 * gPixelScale)

static void* GetArg(void* ptr, int index) {
	return ((u8*)ptr) + sSubMenuParam[index].offset;
}

static Rect GetSubRect(Split* split, int index, int side) {
	Rect r;
	
	r.x = SPLIT_ELEM_X_PADDING / 2;
	r.y = SPLIT_ELEM_X_PADDING + (SIDE_BUTTON_SIZE + SPLIT_ELEM_X_PADDING) * index;
	r.w = SIDE_BUTTON_SIZE;
	r.h = SIDE_BUTTON_SIZE;
	
	if (side < 0)
		return Rect_FlipHori(r, split->dispRect);
	
	return r;
}

///////////////////////////////////////////////////////////////////////////////

void Properties_Init(Editor* editor, Properties* this, Split* split) {
	for (int i = 0; i < ArrCount(sSubMenuParam); i++)
		if (sSubMenuParam[i].init)
			sSubMenuParam[i].init(editor, GetArg(this, i), split);
}

void Properties_Destroy(Editor* editor, Properties* this, Split* split) {
}

void Properties_Update(Editor* editor, Properties* this, Split* split) {
	int xSplit = RectW(split->rect) - split->rect.w / 2;
	
	this->side = (xSplit < (editor->window.dim.x / 2) ? -1 : 1);
	
	Element_RowY(SPLIT_ELEM_X_PADDING * 2);
	Element_ShiftX(SIDE_BUTTON_SIZE * this->side);
	
	if (!split->blockCursor && !editor->nano.state.blockElemInput) {
		Input* input = &editor->input;
		
		for (int i = 0; i < ArrCount(sSubMenuParam); i++) {
			Rect r = GetSubRect(split, i, this->side);
			
			if (Input_SelectClick(input, CLICK_L))
				if (Split_CursorInRect(split, &r))
					this->subIndex = i;
		}
	}
	
	sSubMenuParam[this->subIndex].update(editor, GetArg(this, this->subIndex), split);
}

void Properties_Draw(Editor* editor, Properties* this, Split* split) {
	void* vg = editor->vg;
	Rect r = {
		0, 0, SIDE_BUTTON_SIZE + SPLIT_ELEM_X_PADDING, split->dispRect.h
	};
	
	if (this->side < 0)
		r = Rect_FlipHori(r, split->dispRect);
	Rect scissor = r;
	
	Gfx_DrawRounderRect(vg, r, Theme_GetColor(THEME_ELEMENT_DARK, 255, 0.8f));
	r.x += r.w - SPLIT_PIXEL;
	r.w = SPLIT_PIXEL;
	Gfx_DrawRounderRect(vg, r, Theme_GetColor(THEME_ELEMENT_LIGHT, 25, 1.25f));
	
	for (int i = 0; i < ArrCount(sSubMenuParam); i++) {
		Rect tr;
		Rect r;
		NVGcolor col = nvgHSLA(UnfoldRGBA(sSubMenuParam[i].color));
		
		tr = r = GetSubRect(split, i, this->side);
		tr.x += SPLIT_ELEM_X_PADDING / 2;
		
		if (this->subIndex == i) {
			r = Rect_ExpandX(r, (SPLIT_CTXM_DIST / 2) * this->side);
			nvgScissor(vg, UnfoldRect(scissor));
			r = Rect_ExpandX(r, SPLIT_CTXM_DIST * this->side);
			Gfx_DrawRounderOutline(vg, r, Theme_GetColor(THEME_ELEMENT_LIGHT, 25, 1.0f));
			Gfx_DrawRounderRect(vg, r, Theme_GetColor(THEME_BASE, 255, 1.0f));
			nvgResetScissor(vg);
			
			col = Theme_Mix(0.15f, col, Theme_GetColor(THEME_TEXT, 255, 1.0f));
		}
		
		if (sSubMenuParam[i].icon)
			Gfx_Icon(vg, tr, col, sSubMenuParam[i].icon);
	}
}
