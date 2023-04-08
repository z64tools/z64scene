#ifndef ROOM_LIST_H
#define ROOM_LIST_H

#include <Editor.h>

typedef struct {
    ElContainer list;
} RoomList;

extern SplitTask gRoomListTask;

#endif