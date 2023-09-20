#ifndef SPLIT_TASK_INIT_H
#define SPLIT_TASK_INIT_H

#include "nano_grid.h"

typedef enum {
	TAB_SETTINGS,
	TAB_VIEWPORT,
	TAB_ROOMLIST,
	TAB_OBJECT_LIST,
	TAB_TERMINAL,
	
	TAB_EMPTY,
	TAB_MAX,
} SplitEnum;

extern SplitTask* gTaskTable[TAB_MAX];

#endif
