#include "Editor.h"

// # # # # # # # # # # # # # # # # # # # #
// # EDITING                             #
// # # # # # # # # # # # # # # # # # # # #

ActorEntry* Actor_Add(Room* room, u16 id, u16 param, Vec3s pos, Vec3s rot) {
	ActorEntry* newActor = &room->actor[room->actorNum++];
	
	newActor->index = id;
	newActor->param = param;
	newActor->pos = pos;
	newActor->rot = rot;
	
	return newActor;
}

void Actor_Delete(Room* room, s32 actorIndex) {
	for (s32 i = actorIndex; i < room->actorNum; i++) {
		room->actor[i] = room->actor[1 + i];
	}
	
	room->actorNum--;
}

ObjectEntry* Object_Add(Room* room, u16 id) {
	ObjectEntry* newObj = &room->object[room->objectNum++];
	
	newObj->index = id;
	
	return newObj;
}

void Object_Delete(Room* room, s32 objIndex) {
	for (s32 i = objIndex; i < room->objectNum; i++) {
		room->object[i] = room->object[1 + i];
	}
	
	room->objectNum--;
}

// # # # # # # # # # # # # # # # # # # # #
// # EDITOR                              #
// # # # # # # # # # # # # # # # # # # # #

void Editor_DropCallback(GLFWwindow* window, s32 count, char* item[]) {
}

void Editor_Update(EditorContext* editor) {
	GeoGrid_Update(&editor->geoGrid);
	Cursor_Update(&editor->cursor);
}

void Editor_Draw(EditorContext* editor) {
	GeoGrid_Draw(&editor->geoGrid);
}