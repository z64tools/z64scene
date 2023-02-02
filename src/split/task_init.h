#ifndef SPLIT_TASK_INIT_H
#define SPLIT_TASK_INIT_H

#include "Editor.h"
#include "Settings.h"
#include "Viewport.h"
#include "RoomList.h"

static void update_nothing(Editor* e, void* instance, Split* split) {
    Element_Header(split->taskCombo, 128);
    Element_Combo(split->taskCombo);
}

typedef enum {
    TAB_SETTINGS,
    TAB_3DVP,
    TAB_ROOMLIST,
    
    TAB_EMPTY
} SplitEnum;

SplitTask empty = {
    .taskName = "Empty",
    .init     = NULL,
    .destroy  = NULL,
    .update   = (void*)update_nothing,
    .draw     = NULL,
    .size     = 10
};

SplitTask* gTaskTable[] = {
    [TAB_SETTINGS] = &gSettingsTask,
    [TAB_3DVP] = &gViewportTask,
    [TAB_ROOMLIST] = &gRoomListTask,
    [TAB_EMPTY] = &empty
};

#endif