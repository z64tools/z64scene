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

static void Editor_TriCallback(s32 flag, const Vec3f* v0, const Vec3f* v1, const Vec3f* v2, const Vec3f* n0, const Vec3f* n1, const Vec3f* n2) {
	Editor* editor = GetEditor();
	Triangle* tri;
	
	if (flag == 0) {
		editor->triCount = 0;
		
		return;
	}
	Log("Copy Triangle %d / 4096", editor->triCount);
	
	tri = &editor->triHead[editor->triCount++];
	memcpy(&tri->v[0], v0, sizeof(Vec3f));
	memcpy(&tri->v[1], v1, sizeof(Vec3f));
	memcpy(&tri->v[2], v2, sizeof(Vec3f));
	memcpy(&tri->n[0], n0, sizeof(Vec3f));
	memcpy(&tri->n[1], n1, sizeof(Vec3f));
	memcpy(&tri->n[2], n2, sizeof(Vec3f));
}

void Editor_Init(Editor* editor) {
	sEditor = editor;
	// n64_set_triangle_buffer_callback((void*)Editor_TriCallback);
	// editor->triHead = SysAlloc(sizeof(Triangle) * 4096);
	editor->render.culling = true;
}

void Editor_DropCallback(GLFWwindow* window, s32 count, char* item[]) {
	Editor* editor = GetUserCtx(window);
	s32 getRoom = false;
	
	for (s32 i = 0; i < count; i++) {
		char* file = item[i];
		
		if (StrEndCase(file, ".zscene")) {
			if (editor->scene.segment)
				Scene_Free(&editor->scene);
			
			Time_Start(10);
			Scene_LoadScene(&editor->scene, file);
			printf_info("SceneLoad: %.2fms", Time_Get(10) * 1000);
			getRoom = true;
			break;
		}
	}
	
	if (!getRoom)
		return;
	s32 roomCount = 0;
	
	Time_Start(10);
	for (s32 i = 0; i < count; i++) {
		char* file = item[i];
		
		if (StrEndCase(file, ".zroom") || StrEndCase(file, ".zmap")) {
			roomCount++;
			if (editor->scene.segment)
				Scene_LoadRoom(&editor->scene, file);
		}
	}
	printf_info("RoomLoad: [%d] %.2fms", roomCount, Time_Get(10) * 1000);
}

void Editor_Update(Editor* editor) {
	GeoGrid_Update(&editor->geo);
	Cursor_Update(&editor->cursor);
}

void Editor_Draw(Editor* editor) {
	GeoGrid_Draw(&editor->geo);
}