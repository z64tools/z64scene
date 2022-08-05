#include "Editor.h"

// # # # # # # # # # # # # # # # # # # # #
// # EDITING                             #
// # # # # # # # # # # # # # # # # # # # #

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

// # # # # # # # # # # # # # # # # # # # #
// # DataNode                            #
// # # # # # # # # # # # # # # # # # # # #

typedef union {
	ActorList actor;
	RoomMesh  mesh;
	LightList lightList;
} Data;

void* DataNode_Copy(DataContext* ctx, SceneCmd* cmd) {
	u32 size = 0;
	u8 code = cmd->base.code;
	u32 segment = cmd->base.data2;
	DataNode* node = ctx->head[code];
	Data* this;
	void* ptr = gSegment[segment >> 24];
	
	switch (cmd->base.code) {
		case SCENE_CMD_ID_ACTOR_LIST:
			size = sizeof(ActorList);
			break;
			
		case SCENE_CMD_ID_MESH_HEADER:
			size = sizeof(RoomMesh);
			break;
			
		case SCENE_CMD_ID_LIGHT_SETTINGS_LIST:
			size = sizeof(LightList);
			break;
			
		default:
			printf_warning("DataNode_Copy: No support for code [%02X]", cmd->base.code);
			
			return NULL;
	}
	
	while (node) {
		if (node->segment == segment && node->pointer == ptr)
			return node;
		
		node = node->next;
	}
	
	Assert(segment != 0);
	
	Calloc(node, size);
	Node_Add(ctx->head[code], node);
	node->segment = segment;
	node->pointer = ptr;
	this = (void*)node;
	
	Actor* actor;
	ActorEntry* entry;
	
	switch (code) {
		case SCENE_CMD_ID_ACTOR_LIST:
			actor = this->actor.head = SysCalloc(sizeof(Actor) * 0xFF);
			this->actor.num = cmd->actorList.num;
			entry = SEGMENTED_TO_VIRTUAL(cmd->actorList.segment);
			
			for (s32 i = 0; i < cmd->actorList.num; i++, actor++, entry++) {
				actor->id = entry->id;
				actor->param = entry->param;
				
				actor->pos.x = entry->pos.x;
				actor->pos.y = entry->pos.y;
				actor->pos.z = entry->pos.z;
				
				actor->rot.x = entry->rot.x;
				actor->rot.y = entry->rot.y;
				actor->rot.z = entry->rot.z;
			}
			
			break;
			
		case SCENE_CMD_ID_MESH_HEADER:
			this->mesh.header = SEGMENTED_TO_VIRTUAL(segment);
			this->mesh.roomFile = gSegment[3];
			break;
			
		case SCENE_CMD_ID_LIGHT_SETTINGS_LIST:
			this->lightList.env = SysCalloc(sizeof(EnvLightSettings) * 0xFF);
			this->lightList.num = cmd->lightSettingList.num;
			memcpy(
				this->lightList.env,
				SEGMENTED_TO_VIRTUAL(cmd->lightSettingList.segment),
				sizeof(EnvLightSettings) * this->lightList.num
			);
			this->lightList.enumProp = PropEnum_Init(0);
			
			for (s32 i = 0; i < this->lightList.num; i++)
				PropEnum_Add(this->lightList.enumProp, xFmt("ENV %d", i));
			
			break;
	}
	
	return node;
}

void DataNode_Free(DataContext* ctx, u8 code) {
	DataNode** head = &ctx->head[code];
	
	while (*head) {
		DataNode* node = *head;
		Data* this = (void*)node;
		
		switch (code) {
			case SCENE_CMD_ID_MESH_HEADER:
				TriBuffer_Free(&this->mesh.triBuf);
				break;
			case SCENE_CMD_ID_ACTOR_LIST:
				Free(this->actor.head);
				break;
			case SCENE_CMD_ID_LIGHT_SETTINGS_LIST:
				PropEnum_Free(this->lightList.enumProp);
				Free(this->lightList.env);
				break;
		}
		
		Node_Kill(*head, node);
	}
}