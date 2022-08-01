#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <ExtLib.h>
#include <ExtGui/Interface.h>
#include <ExtGui/Collision.h>
#include "SkelAnime.h"
#include "Light.h"
#include "Scene.h"

typedef struct {
	u16   index;
	u16   param;
	Vec3s pos;
	Vec3s rot;
} ActorEntry;

typedef struct {
	u16 index;
} ObjectEntry;

typedef struct {
	PropEnum* propEndID;
} InterfaceContext;

typedef struct {
	u32 culling : 1;
} RenderState;

typedef struct Editor {
	AppInfo app;
	void*   vg;
	GeoGrid geo;
	Cursor  cursor;
	Input   input;
	Scene   scene;
	RenderState render;
	InterfaceContext interface;
} Editor;

ActorEntry* Actor_Add(Room* room, u16 id, u16 param, Vec3s pos, Vec3s rot);
void Actor_Delete(Room* room, s32 actorIndex);
ObjectEntry* Object_Add(Room* room, u16 id);
void Object_Delete(Room* room, s32 objIndex);

void* NewMtx();

Editor* GetEditor(void);
void Editor_Init(Editor* editor);
void Editor_DropCallback(GLFWwindow* window, s32 count, char* item[]);
void Editor_Update(Editor* editor);
void Editor_Draw(Editor* editor);

extern Gfx* gSetupDL;
#define gSetupDList(x) & gSetupDL[6 * x]

#endif