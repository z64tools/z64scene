#include "Editor.h"

// # # # # # # # # # # # # # # # # # # # #
// # EDITING                             #
// # # # # # # # # # # # # # # # # # # # #

#if 0
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
#endif

void* NewMtx() {
	return Matrix_ToMtx(n64_graph_alloc(sizeof(Mtx)));
}

// # # # # # # # # # # # # # # # # # # # #
// # EDITOR                              #
// # # # # # # # # # # # # # # # # # # # #

static EditorContext* sEditCtx;

EditorContext* Editor_GetCtx(void) {
	return sEditCtx;
}

void Editor_DropCallback(GLFWwindow* window, s32 count, char* item[]) {
	EditorContext* editor = Editor_GetCtx();
	s32 getRoom = false;
	
	printf_info("DragNDrop: %d", count);
	editor->state.drawBlock = true;
	
	for (s32 i = 0; i < count; i++) {
		char* file = item[i];
		
		if (StrEndCase(file, ".zscene")) {
			if (editor->scene.segment)
				Scene_Free(&editor->scene);
			
			printf_info_align("DragLoad", file);
			Scene_LoadScene(&editor->scene, file);
			getRoom = true;
			break;
		}
	}
	
	if (!getRoom)
		return;
	
	for (s32 i = 0; i < count; i++) {
		char* file = item[i];
		
		if (StrEndCase(file, ".zroom") || StrEndCase(file, ".zmap")) {
			if (editor->scene.segment) {
				printf_info_align("DragLoad", file);
				Scene_LoadRoom(&editor->scene, file);
			}
		}
	}
	
	editor->state.drawBlock = false;
}

void Editor_Update(EditorContext* editor) {
	sEditCtx = editor;
	GeoGrid_Update(&editor->geoGrid);
	Cursor_Update(&editor->cursor);
}

void Editor_Draw(EditorContext* editor) {
	GeoGrid_Draw(&editor->geoGrid);
}