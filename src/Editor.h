#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <ext_lib.h>
#include <ext_texel.h>
#include <ext_interface.h>
#include <ext_collision.h>
#include <ext_undo.h>
#include <split/task_init.h>
#include "SkelAnime.h"
#include "Light.h"
#include "Scene.h"

extern f32 gFpsTime;
extern f32 gTotalTime;

void SlotMsg(int i, const char* fmt, ...);

typedef enum {
	TIME_SCENE_ANIM = 0x40,
} TimeSlot;

typedef enum {
	PROFILER_FPS = 40,
} ProfilerSlot;

typedef struct Editor {
	Window      window;
	void*       vg;
	NanoGrid    nano;
	CursorIcon  cursor;
	Input       input;
	Scene       scene;
	Gizmo       gizmo;
	Toml        config;
	const char* fpconfig;
	DbActorSearchMenu* searchMenu;
	void* head[2];
} Editor;

void* NewMtx();

Editor* GetEditor(void);
void Editor_LoadFiles(int num, char* items[]);
void Editor_Init(Editor* editor);
void Editor_Destroy(Editor* editor);
void Editor_Update(Editor* editor);
void Editor_Draw(Editor* editor);

void Console(const char*, ...);
void ClearConsole();

extern Gfx* gSetupDL;
#define gSetupDList(x) &gSetupDL[6 * x]

#endif
