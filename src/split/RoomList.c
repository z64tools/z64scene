#include "RoomList.h"
#include "Viewport.h"

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

static bool RoomList_OnChange(PropList* prop, PropListChange type, int value) {
    if (type != PROP_SET) return true;
    
    if (value == 0) {
        Editor* editor = prop->udata1;
        GeoGrid* geo = &editor->geo;
        ElContainer* container = prop->udata2;
        
        fornode(split, geo->splitHead) {
            if (split->id != TAB_VIEWPORT) continue;
            Viewport* vp = split->instance;
            Scene* scene = &editor->scene;
            
            Viewport_FocusRoom(vp, scene, container->contextKey);
        }
    }
    
    return false;
}

void RoomList_Init(Editor* editor, RoomList* this, Split* split) {
    Element_Container_SetPropList(&this->list, &editor->scene.ui.roomList, 32);
    
    this->list.contextMenu = new(PropList);
    *this->list.contextMenu = PropList_InitList(-1, "Focus", NULL, "Nice", NULL, NULL, "Wow", "Damn");
    PropList_SetOnChangeCallback(this->list.contextMenu, RoomList_OnChange, editor, &this->list);
    
    this->list.showHexID = true;
}

void RoomList_Destroy(Editor* editor, RoomList* this, Split* split) {
    PropList_Free(this->list.contextMenu);
    vfree(this->list.contextMenu);
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
