#include "ObjectList.h"

void ObjectList_Init(Editor* editor, ObjectList* this, Split* split);
void ObjectList_Update(Editor* editor, ObjectList* this, Split* split);

SplitTask gObjectListTask = {
    .taskName = "Object List",
    .init     = (void*)ObjectList_Init,
    .update   = (void*)ObjectList_Update,
    .size     = sizeof(ObjectList)
};

void ObjectList_Init(Editor* editor, ObjectList* this, Split* split) {
    this->list.showHexID = true;
    this->list.mutable = true;
    this->list.textBox.size = 4;
    this->list.controller = true;
    Element_Container_SetArli(&this->list, NULL, 8);
}

void ObjectList_Update(Editor* editor, ObjectList* this, Split* split) {
    Scene* scene = &editor->scene;
    RoomHeader* room = NULL;
    
    if (editor->scene.segment) {
        room = Scene_GetRoomHeader(scene, scene->curRoom);
        
        Element_Container_SetArli(&this->list, &room->objectList, 8);
    }
    
    Element_Condition(&this->list, room != NULL);
    
    split->scroll.offset = split->scroll.voffset = 0;
    
    Element_Header(split->taskCombo, 92);
    Element_Combo(split->taskCombo);
    
    this->list.element.rect =
        Rect_SubPos(
        Rect_Scale(split->rect, -SPLIT_ELEM_X_PADDING * 2, -SPLIT_ELEM_X_PADDING * 2),
        split->rect);
    
    switch (Element_Container(&this->list)) {
        case -2: {
            RoomHeader* room = Scene_GetRoomHeader(scene, scene->curRoom);
            u16* obj = Arli_At(&room->objectList, room->objectList.cur);
            
            if (obj)
                *obj = shex(this->list.textBox.txt);
        } break;
    }
}
