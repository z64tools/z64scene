#ifndef __EN_ROOM__
#define __EN_ROOM__

#include <Editor.h>

typedef struct {
	ElCombo  envID;
	ElButton buttonIndoor;
	ElCombo  comboBox;
	
	ElButton buttonFPS;
	ElButton buttonCulling;
	
	ElButton buttonFog;
	ElButton buttonColView;
	
	ElButton killScene;
} Settings;

extern SplitTask gSettingsTask;

#endif