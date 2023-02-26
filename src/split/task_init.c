#include "RoomList.h"
#include "Settings.h"
#include "Viewport.h"
#include "task_init.h"

static void update_nothing(Editor* e, void* instance, Split* split) {
    Element_Header(split->taskCombo, 128);
    Element_Combo(split->taskCombo);
}

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
    [TAB_VIEWPORT] = &gViewportTask,
    [TAB_ROOMLIST] = &gRoomListTask,
    [TAB_EMPTY] = &empty
};