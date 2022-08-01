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

static Editor* sEditor;

// # # # # # # # # # # # # # # # # # # # #
// # EDITOR                              #
// # # # # # # # # # # # # # # # # # # # #

Editor* GetEditor(void) {
	return sEditor;
}

void Editor_Init(Editor* editor) {
	sEditor = editor;
	editor->render.culling = true;
}

void Editor_DropCallback(GLFWwindow* window, s32 count, char* item[]) {
	Editor* editor = GetUserCtx(window);
	s32 hasRoom = false;
	s32 hasScene = false;
	s32 roomCount = 0;
	s32 n = 0;
	
	for (s32 i = 0; i < count; i++) {
		char* file = item[i];
		
		if (StrEndCase(file, ".zroom") || StrEndCase(file, ".zmap"))
			hasRoom = true;
		
		if (StrEndCase(file, ".zscene")) {
			hasScene = true;
			n = i;
		}
	}
	
	if (!hasRoom || !hasScene)
		return;
	
	printf_info("" PRNT_YELW "LOADING: " PRNT_BLUE "%s", item[n]);
	for (s32 i = 0; i < count; i++) {
		char* file = item[i];
		
		if (StrEndCase(file, ".zscene")) {
			if (editor->scene.segment)
				Scene_Free(&editor->scene);
			
			Time_Start(10);
			Scene_LoadScene(&editor->scene, file);
			printf_info("SceneLoad:  " PRNT_REDD "%.2fms", Time_Get(10) * 1000);
			break;
		}
	}
	
	Time_Start(10);
	for (s32 i = 0; i < count; i++) {
		char* file = item[i];
		
		if (StrEndCase(file, ".zroom") || StrEndCase(file, ".zmap")) {
			roomCount++;
			if (editor->scene.segment)
				Scene_LoadRoom(&editor->scene, file);
		}
	}
	printf_info("RoomLoad:   " PRNT_REDD "%.2fms" PRNT_RSET " [%d]", Time_Get(10) * 1000, roomCount);
	
	Time_Start(10);
	Scene_CacheBuild(&editor->scene);
	printf_info("CacheBuild: " PRNT_REDD "%.2fms", Time_Get(10) * 1000);
}

void Editor_Update(Editor* editor) {
	GeoGrid_Update(&editor->geo);
	Cursor_Update(&editor->cursor);
}

void Editor_Draw(Editor* editor) {
	GeoGrid_Draw(&editor->geo);
}