#include "room_list/RoomList.h"
#include "properties/Properties.h"
#include "viewport/Viewport.h"
#include "object_list/ObjectList.h"
#include "terminal/Terminal.h"
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
	[TAB_SETTINGS] = &gPropertiesTask,
	[TAB_VIEWPORT] = &gViewportTask,
	[TAB_ROOMLIST] = &gRoomListTask,
	[TAB_OBJECT_LIST] = &gObjectListTask,
	[TAB_TERMINAL] = &gTerminalTask,
	[TAB_EMPTY] = &empty
};
