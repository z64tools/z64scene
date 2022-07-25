#ifndef __EN_ROOM__
#define __EN_ROOM__

#include <Editor.h>

typedef struct {
	ElCombo  envID;
	ElButton buttonDayLight;
	ElButton buttonFog;
	ElCombo  comboBox;
} EnRoom;

extern SplitTask gEnRoomTask;

#endif