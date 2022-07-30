#ifndef __EN_ROOM__
#define __EN_ROOM__

#include <Editor.h>

typedef struct {
	ElCombo  envID;
	ElButton buttonIndoor;
	ElButton buttonFog;
	ElCombo  comboBox;
	ElButton buttonFPS;
	ElSlider slider;
} EnRoom;

extern SplitTask gEnRoomTask;

#endif