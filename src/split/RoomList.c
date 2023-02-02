#include "RoomList.h"

void RoomList_Init(Editor* editor, RoomList* this, Split* split);
void RoomList_Destroy(Editor* editor, RoomList* this, Split* split);
void RoomList_Update(Editor* editor, RoomList* this, Split* split);
void RoomList_Draw(Editor* editor, RoomList* this, Split* split);

SplitTask gRoomListTask = {
    .taskName = "Room List",
    .init     = (void*)RoomList_Init,
    .destroy  = (void*)RoomList_Destroy,
    .update   = (void*)RoomList_Update,
    .draw     = (void*)RoomList_Draw,
    .size     = sizeof(RoomList)
};

void RoomList_Init(Editor* editor, RoomList* this, Split* split) {
    Element_Container_SetPropList(&this->list, &editor->scene.ui.roomList, 32);
}

void RoomList_Destroy(Editor* editor, RoomList* this, Split* split) {
    
}

void RoomList_Update(Editor* editor, RoomList* this, Split* split) {
    this->list.element.heightAdd = (split->dispRect.h - SPLIT_ELEM_Y_PADDING * 2) - SPLIT_TEXT_H;
    split->scroll.offset = split->scroll.voffset = 0;
    
    Element_Header(split->taskCombo, 92);
    Element_Combo(split->taskCombo);
    
    Element_Row(&this->list, 1.0f);
    Element_Container(&this->list);
}

void RoomList_Draw(Editor* editor, RoomList* this, Split* split) {
    
}
