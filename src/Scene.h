#include <ExtLib.h>

typedef struct StructBE {
	/* 0x00 */ u8  ambientColor[3];
	/* 0x03 */ s8  light1Dir[3];
	/* 0x06 */ u8  light1Color[3];
	/* 0x09 */ s8  light2Dir[3];
	/* 0x0C */ u8  light2Color[3];
	/* 0x0F */ u8  fogColor[3];
	/* 0x12 */ s16 fogNear;
	/* 0x14 */ s16 fogFar;
} EnvLightSettings;

typedef struct {
	void*   segment;
	MemFile file;
	void*   mesh;
} Room;

typedef struct {
	void*   segment;
	MemFile file;
	Room**  room;
	u32 numRoom;
	
	EnvLightSettings* env;
	u32 numEnv;
	u8  setupEnv;
	u8  useDaylight;
	u8  useFog;
	
	u8  setupHeader;
	struct Split* split;
} Scene;

void Scene_LoadScene(Scene* this, const char* file);
void Scene_LoadRoom(Scene* this, const char* file);
void Scene_Free(Scene* this);
void Scene_ExecuteCommands(Scene* this, Room* room);
void Scene_Draw(Scene* this);

void Room_Draw(Scene* scene, Room* room);