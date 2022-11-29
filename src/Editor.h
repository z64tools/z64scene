#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <ext_lib.h>
#include <ext_interface.h>
#include <ext_collision.h>
#include "SkelAnime.h"
#include "Light.h"
#include "Scene.h"

extern VectorGfx gVecGfx_EyeOpen;

typedef enum {
	TIME_SCENE_ANIM = 0x40,
} TimeSlot;

typedef enum {
	PROFILER_FPS = 4,
} ProfilerSlot;

typedef struct Editor {
	AppInfo app;
	void*   vg;
	GeoGrid geo;
	Cursor  cursor;
	Input   input;
	Scene   scene;
} Editor;

void* NewMtx();

Editor* GetEditor(void);
void Editor_Init(Editor* editor);
void Editor_Destroy(Editor* editor);
void Editor_DropCallback(GLFWwindow* window, s32 count, char* item[]);
void Editor_Update(Editor* editor);
void Editor_Draw(Editor* editor);

void* DataNode_Copy(DataContext* ctx, SceneCmd* cmd);
void DataNode_Free(DataContext* ctx, u8 code);

extern Gfx* gSetupDL;
#define gSetupDList(x) & gSetupDL[6 * x]

#endif