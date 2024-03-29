#ifndef SETTINGS_H
#define SETTINGS_H

#include <Editor.h>
#include "Database.h"

typedef struct {
	ElCheckbox buttonIndoor;
	
	ElButton buttonUnused;
	ElButton buttonCulling;
	
	ElButton buttonFog;
	ElButton buttonColView;
	
	ElButton killScene;
	
	ElColor envAmbient;
	ElColor envFogColor;
	ElColor envColA;
	ElColor envColB;
	
	ElSlider fogNear;
	ElSlider fogFar;
} MenuDebug;

typedef enum {
	PE_COMBO,
	PE_TEXT,
	PE_CHECK,
} PropEntType;

typedef struct {
	union {
		Element*    element;
		ElCombo*    combo;
		ElTextbox*  textBox;
		ElCheckbox* check;
	};
	
	Arli*       list;
	DbProperty* property;
	int type;
} PropertyEntry;

typedef struct {
	ElCombo actorEntry;
	
	ElTextbox index;
	ElTextbox variable;
	ElTextbox rotX;
	ElTextbox rotY;
	ElTextbox rotZ;
	ElTextbox posX;
	ElTextbox posY;
	ElTextbox posZ;
	
	ElButton buttonSelected;
	ElButton buttonAdd;
	ElButton buttonIns;
	ElButton buttonRem;
	ElButton refreshDatabase;
	
	PropertyEntry* list;
	u32 num;
	u16 prevIndex;
	
	ElPanel panelPosRot;
	ElPanel panelProperties;
	ElPanel panelTest;
} MenuActor;

typedef struct {
	ElTextbox textboxHour;
	ElTextbox textboxMin;
	ElButton  buttonGlTime;
	ElSlider  sliderTimeSpeed;
	
	ElPanel panelTime;
	
	ElButton buttonSkybox;
	ElButton buttonSunMoon;
	ElSlider sliderEcho;
	ElCombo  comboBeha1;
	ElCombo  comboBeha2;
	
	ElPanel panelEnvironment;
} MenuRoomEnv;

typedef struct {
	ElButton    buttonIndoor;
	ElContainer containerSky;
	
	ElPanel panelLight;
	
	ElCombo  comboEnvIndex;
	ElColor  colorAmbient;
	ElColor  colorA;
	ElColor  colorB;
	ElButton buttonDirA;
	ElButton buttonDirB;
	ElSlider sliderNear;
	ElSlider sliderFar;
} MenuEnv;

typedef struct {
	int         subIndex;
	int         side;
	MenuDebug   menuDebug;
	MenuActor   menuActor;
	MenuEnv     menuEnv;
	MenuRoomEnv menuRoomEnv;
} Properties;

extern SplitTask gPropertiesTask;

void Properties_BasicHeader(Split* split);

#endif
