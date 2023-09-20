#include "Properties.h"
#include "split/viewport/Viewport.h"
#include "split/task_init.h"

void MenuEnv_Init(Editor* editor, void* __this, Split* split) {
	MenuEnv* this = __this;
	
	const struct {
		ElButton*   ptr;
		const char* name;
		f32 lerp;
	} name[] = {
		{ (void*)&this->buttonIndoor,  "Indoor",         0.5f            },
		
		{ (void*)&this->comboEnvIndex, "Setting",        0.25f           },
		{ (void*)&this->colorAmbient,  "Ambient",        0.25f           },
		{ (void*)&this->colorA,        "Color A",        0.5f            },
		{ (void*)&this->colorB,        "Color B",        0.5f            },
		{ (void*)&this->buttonDirA,    "Normal A",       0.5f            },
		{ (void*)&this->buttonDirB,    "Normal B",       0.5f            },
		{ (void*)&this->sliderNear,    "Near",           0.5f            },
		{ (void*)&this->sliderFar,     "Far",            0.5f            },
	};
	
	for (int i = 0; i < ArrCount(name); i++) {
		Element_SetName(name[i].ptr, name[i].name);
		Element_SetNameLerp(name[i].ptr, name[i].lerp);
	}
	
	Element_SetIcon(&this->buttonDirA, ICON_EYE);
	Element_SetIcon(&this->buttonDirB, ICON_EYE);
	
	Element_Slider_SetParams(&this->sliderNear, 0, 1000, "int");
	Element_Slider_SetParams(&this->sliderFar, 100, 32000, "int");
}

static View3D* GetView(NanoGrid* nano) {
	Split* view = NULL;
	f64 d = 0;
	
	for (Split* split = nano->splitHead; split; split = split->next) {
		if (split->id != TAB_VIEWPORT) continue;
		
		if (split->rect.w * split->rect.h > d) {
			view = split;
			d = split->rect.w * split->rect.h;
		}
	}
	
	if (view) {
		Viewport* vp = view->instance;
		
		return &vp->view;
	}
	
	return NULL;
}

static void SetLightNormal(View3D* view, s8* normal) {
	if (!view) return;
	
	Vec3f n = Math_Vec3f_LineSegDir(view->currentCamera->eye, view->currentCamera->at);
	
	n = Math_Vec3f_Invert(n);
	
	for (int i = 0; i < 3; i++)
		normal[i] = remapf(n.axis[i], -1.0f, 1.0f, -128, 127);
}

void MenuEnv_Update(Editor* editor, void* __this, Split* split) {
	MenuEnv* this = __this;
	Scene* mainScene = &editor->scene;
	SceneHeader* scene = Scene_GetCurSceneHeader(mainScene);
	EnvLights* light = scene ? Arli_At(&scene->envList, scene->envList.cur) : NULL;
	
	Element_Condition(&this->buttonIndoor, light != NULL);
	Element_Condition(&this->containerSky, light != NULL);
	Element_Condition(&this->comboEnvIndex, light != NULL);
	Element_Condition(&this->colorAmbient, light != NULL);
	Element_Condition(&this->colorA, light != NULL);
	Element_Condition(&this->colorB, light != NULL);
	Element_Condition(&this->buttonDirA, light != NULL);
	Element_Condition(&this->buttonDirB, light != NULL);
	Element_Condition(&this->sliderNear, light != NULL);
	Element_Condition(&this->sliderFar, light != NULL);
	this->comboEnvIndex.list = scene ? &scene->envList : NULL;
	
	Properties_BasicHeader(split);
	
	if (Element_Box(BOX_START, &this->panelLight, DatabaseScene_GetName(SCENE_TITLE_LIGHTING))) {
		Element_Color_SetColor(&this->colorAmbient, light ? light->ambientColor : NULL);
		Element_Color_SetColor(&this->colorA, light ? light->light1Color : NULL);
		Element_Color_SetColor(&this->colorB, light ? light->light2Color : NULL);
		
		Element_Row(&this->comboEnvIndex, 1.0f);
		
		Element_Combo(&this->comboEnvIndex);
		
		Element_Row(&this->colorAmbient, 1.0f);
		Element_Row(&this->colorA, 0.5f, &this->colorB, 0.5f);
		Element_Color(&this->colorAmbient);
		Element_Color(&this->colorA);
		Element_Color(&this->colorB);
		
		Element_Row(&this->buttonDirA, 0.5f, &this->buttonDirB, 0.5f);
		
		if (Element_Button(&this->buttonDirA))
			SetLightNormal(GetView(&editor->nano), light->light1Dir);
		if (Element_Button(&this->buttonDirB))
			SetLightNormal(GetView(&editor->nano), light->light2Dir);
		
		Element_Row(&this->sliderNear, 0.5f, &this->sliderFar, 0.5f);
		
		if (Element_Slider(&this->sliderNear))
			light->fogNear = Element_Slider_GetValue(&this->sliderNear);
		else if (light) Element_Slider_SetValue(&this->sliderNear, light->fogNear & 0x3FF);
		
		if (Element_Slider(&this->sliderFar))
			light->fogFar = Element_Slider_GetValue(&this->sliderFar);
		else if (light) Element_Slider_SetValue(&this->sliderFar, light->fogFar);
		
		Element_Separator(true);
		
		Element_Row(&this->buttonIndoor, 0.5f, &this->containerSky, 0.5f);
		
		if (Element_Button(&this->buttonIndoor))
			scene->envState ^= 1;
		else if (scene)
			Element_Button_SetProperties(&this->buttonIndoor, true, scene->envState);
		
		Element_Container(&this->containerSky);
	}
	
	Element_Box(BOX_END, &this->panelLight);
}
