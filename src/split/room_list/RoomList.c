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
    Element_Container_SetArli(&this->list, &editor->scene.ui.roomNameList, 32);
    this->list.showHexID = true;
}

void RoomList_Update(Editor* editor, RoomList* this, Split* split) {
    Scene* scene = &editor->scene;
    
    this->list.element.heightAdd = (split->dispRect.h - SPLIT_ELEM_Y_PADDING * 2) - SPLIT_TEXT_H;
    split->scroll.offset = split->scroll.voffset = 0;
    
    Element_Header(split->taskCombo, 92);
    Element_Combo(split->taskCombo);
    Element_Row(&this->list, 1.0f);
    
    Element_Condition(&this->list, scene->segment != NULL);
    
    int index;
    
    if ( (index = Element_Container(&this->list)) > -1) {
        info("next: %d", index);
        editor->scene.next.roomId = index;
    }
}
