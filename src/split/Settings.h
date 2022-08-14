#ifndef __EN_ROOM__
#define __EN_ROOM__

#include <Editor.h>

typedef struct {
	ElButton buttonIndoor;
	ElCombo  comboBox;
	
	ElButton buttonFPS;
	ElButton buttonCulling;
	
	ElButton buttonFog;
	ElButton buttonColView;
	
	ElButton killScene;
	
	ElContainer cont;
} Settings;

extern SplitTask gSettingsTask;

#endif