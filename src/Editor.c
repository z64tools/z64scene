#include "Editor.h"
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

#define INCBIN_PREFIX
#include "../incbin/incbin.h"

INCBIN(gFont_CascadiaCode, "assets/CascadiaCode-SemiBold.ttf");
INCBIN(gFont_NotoSand, "assets/NotoSans-Bold.ttf");
INCBIN(gGizmo3D, "assets/gizmo_arrow.zobj");

INCBIN(gAppIcon_16, "assets/icon/16.data");
INCBIN(gAppIcon_32, "assets/icon/32.data");
INCBIN(gAppIcon_64, "assets/icon/64.data");
INCBIN(gAppIcon_128, "assets/icon/128.data");

static GLFWimage gAppIcon[4] = {
	{
		16, 16,
		(void*)gAppIcon_16Data
	},
	{
		32, 32,
		(void*)gAppIcon_32Data
	},
	{
		64, 64,
		(void*)gAppIcon_64Data
	},
	{
		128, 128,
		(void*)gAppIcon_128Data
	},
};

char* gBuild = {
	"z64scene alpha commit[ "
	#ifndef __COMFLAG__
		GIT_COMMIT_MSG
	#endif
	" ]"
};
char* gHash = {
	""
	#ifndef __COMFLAG__
		GIT_COMMIT_HASH
	#endif
};

SplitTask sTaskTable[] = {
	{
		"None"
	},
	{
		NULL
	},
	{
		"3D-Viewport",
		DefineTask(EnSceneView)
	},
	{
		"Scene",
		DefineTask(EnRoom)
	}
};

EditorContext* gEditCtx;

void Editor_DropCallback(GLFWwindow* window, s32 count, char* file[]) {
	s8 loadFlag[ROOM_MAX] = { 0 };
	s32 loadRoom = 0;
	
	for (s32 i = 0; i < count; i++) {
		printf_debug("Drop File: [%s]", file[i]);
		
		if (StrStr(file[i], "conf.txt")) {
			MemFile memFile = MemFile_Initialize();
			char* shaderPtr;
			printf_debug("Loading Config [%s]", file[i]);
			
			MemFile_LoadFile_String(&memFile, file[i]);
			shaderPtr = StrStr(memFile.data, "shader");
			Assert(shaderPtr != NULL);
			printf_debug("ShaderID [%s]", String_GetWord(shaderPtr, 1));
			gSceneConfIndex = String_GetInt(String_GetWord(shaderPtr, 1));
			MemFile_Free(&memFile);
		}
		
		if (StrStr(file[i], "config.cfg")) {
			MemFile memFile = MemFile_Initialize();
			char* shaderPtr;
			printf_debug("Loading Config [%s]", file[i]);
			
			if (!MemFile_LoadFile_String(&memFile, file[i]))
				gSceneConfIndex = Config_GetInt(&memFile, "scene_func_id");
			MemFile_Free(&memFile);
		}
		
		if (StrStr(file[i], ".zscene")) {
			printf_debug("Loading Scene [%s]", file[i]);
			MemFile_LoadFile(&gEditCtx->scene.file, file[i]);
			Scene_ExecuteCommands(&gEditCtx->scene, NULL);
		}
		
		if (StrStr(file[i], ".zmap") || StrStr(file[i], ".zroom")) {
			loadRoom++;
			for (s32 j = 0; j < ROOM_MAX; j++) {
				char roomNum[128] = { 0 };
				if (StrStr(file[i], ".zmap"))
					sprintf(roomNum, "_%d.zmap", j);
				else
					sprintf(roomNum, "_%d.zroom", j);
				if (MemMem(file[i], strlen(file[i]), roomNum, strlen(roomNum))) {
					printf_debug("Loading Room [%s]", file[i]);
					
					gEditCtx->scene.lightCtx.room[j].lightNum = 0;
					MemFile_LoadFile(&gEditCtx->room[j].file, file[i]);
					Scene_ExecuteCommands(&gEditCtx->scene, &gEditCtx->room[j]);
					loadFlag[j] = true;
				}
			}
		}
	}
	
	if (loadRoom) {
		n64_clearShaderCache();
		for (s32 i = 0; i < ROOM_MAX; i++) {
			if (loadFlag[i] == false && gEditCtx->room[i].file.data != NULL) {
				gEditCtx->scene.lightCtx.room[i].lightNum = 0;
				printf_debug("Clearing Room [%d]", i);
				MemFile_Free(&gEditCtx->room[i].file);
			}
		}
	}
}

void Editor_NewScene(EditorContext* editCtx) {
	strcpy(editCtx->project.sceneName, "Untitled_Scene");
}

/* / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Editor_Draw(EditorContext* editCtx) {
	s32 winWidth, winHeight;
	s32 fbWidth, fbHeight;
	
	if (glfwGetWindowAttrib(editCtx->appInfo.mainWindow, GLFW_ICONIFIED))
		return;
	
	glfwGetWindowSize(editCtx->appInfo.mainWindow, &winWidth, &winHeight);
	glfwGetFramebufferSize(editCtx->appInfo.mainWindow, &fbWidth, &fbHeight);
	gPixelRatio = (float)fbWidth / (float)winWidth;
	GeoGrid_Draw(&editCtx->geoCtx);
}

void Editor_Update(EditorContext* editCtx) {
	if (glfwGetWindowAttrib(editCtx->appInfo.mainWindow, GLFW_ICONIFIED))
		return;
	
	GeoGrid_Update(&editCtx->geoCtx);
	Cursor_Update(&editCtx->cursorCtx);
}

void Editor_Init(EditorContext* editCtx) {
	editCtx->geoCtx.passArg = editCtx;
	editCtx->geoCtx.taskTable = sTaskTable;
	editCtx->geoCtx.taskTableNum = ArrayCount(sTaskTable);
	editCtx->vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
	
	gEditCtx = editCtx;
	
	editCtx->zobj = MemFile_Initialize();
	editCtx->scene.file = MemFile_Initialize();
	for (s32 i = 0; i < ROOM_MAX; i++) {
		editCtx->room[i].file = MemFile_Initialize();
	}
	
	if (editCtx->vg == NULL)
		printf_error("Could not init nanovg.");
	nvgCreateFontMem(editCtx->vg, "font-basic", (void*)gFont_CascadiaCodeData, gFont_CascadiaCodeSize, 0);
	nvgCreateFontMem(editCtx->vg, "font-bold", (void*)gFont_NotoSandData, gFont_NotoSandSize, 0);
	
	editCtx->gizmo = (void*)gGizmo3DData;
	
	Rcp_Init();
	Theme_Init(0);
	GeoGrid_Init(
		&editCtx->geoCtx,
		&editCtx->appInfo.winDim,
		&editCtx->inputCtx,
		editCtx->vg
	);
	Cursor_Init(&editCtx->cursorCtx);
	Editor_NewScene(editCtx);
	
	glfwSetWindowIcon(editCtx->appInfo.mainWindow, 4, gAppIcon);
	glfwSetWindowSizeLimits(
		editCtx->appInfo.mainWindow,
		400,
		200,
		GLFW_DONT_CARE,
		GLFW_DONT_CARE
	);
}