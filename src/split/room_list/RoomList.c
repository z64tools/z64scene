#include "RoomList.h"

void RoomList_Init(Editor* editor, RoomList* this, Split* split);
void RoomList_Update(Editor* editor, RoomList* this, Split* split);

SplitTask gRoomListTask = {
	.taskName = "Room List",
	.init     = (void*)RoomList_Init,
	.update   = (void*)RoomList_Update,
	.size     = sizeof(RoomList)
};

void RoomList_Init(Editor* editor, RoomList* this, Split* split) {
	Element_Container_SetArli(&this->list, &editor->scene.ui.roomNameList, 2);
	this->list.showHexID = true;
}

void RoomList_Update(Editor* editor, RoomList* this, Split* split) {
	Scene* scene = &editor->scene;

	split->scroll.offset = split->scroll.voffset = 0;

	Element_Header(split->taskCombo, 92);
	Element_Combo(split->taskCombo);

	Element_Condition(&this->list, scene->segment != NULL);

	this->list.element.rect =
	    Rect_SubPos(
		Rect_Scale(split->rect, -SPLIT_ELEM_X_PADDING * 2, -SPLIT_ELEM_X_PADDING * 2),
		split->rect);

	switch (Element_Container(&this->list)->state) {
		case SET_CHANGE:
			info("changed");
			editor->scene.next.roomId = this->list.set.index;
			break;
	}
}
