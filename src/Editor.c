#include "Editor.h"
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

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
		"3DViewport",
		DefineTask(EnSceneView)
	},
	{
		"Room",
		DefineTask(EnRoom)
	}
};

EditorContext* gEditCtx;

void Editor_DropCallback(GLFWwindow* window, s32 count, char* file[]) {
	s8 loadFlag[ROOM_MAX] = { 0 };
	s32 loadRoom = 0;
	
	printf_SetSuppressLevel(PSL_NONE);
	
	for (s32 i = 0; i < count; i++) {
		printf_info("Drop File: [%s]", file[i]);
		
		if (StrStr(file[i], "conf.txt")) {
			MemFile memFile = MemFile_Initialize();
			char* shaderPtr;
			printf_info("Loading Conf [%s]", file[i]);
			
			MemFile_LoadFile_String(&memFile, file[i]);
			shaderPtr = StrStr(memFile.data, "shader");
			Assert(shaderPtr != NULL);
			printf_info("ShaderID [%s]", String_GetWord(shaderPtr, 1));
			gSceneConfIndex = String_GetInt(String_GetWord(shaderPtr, 1));
			MemFile_Free(&memFile);
		}
		
		if (StrStr(file[i], ".zscene")) {
			printf_info("Loading Scene [%s]", file[i]);
			MemFile_LoadFile(&gEditCtx->scene.file, file[i]);
			Scene_ExecuteCommands(&gEditCtx->scene, NULL);
		}
		
		if (StrStr(file[i], ".zmap")) {
			loadRoom++;
			for (s32 j = 0; j < ROOM_MAX; j++) {
				char roomNum[128] = { 0 };
				sprintf(roomNum, "_%d.zmap", j);
				if (MemMem(file[i], strlen(file[i]), roomNum, strlen(roomNum))) {
					printf_info("Loading Room [%s]", file[i]);
					
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
				printf_info("Clearing Room [%d]", i);
				MemFile_Free(&gEditCtx->room[i].file);
			}
		}
	}
	
	printf_SetSuppressLevel(PSL_DEBUG);
}

/* / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / */

void Editor_Draw(EditorContext* editCtx) {
	if (glfwGetWindowAttrib(editCtx->appInfo.mainWindow, GLFW_ICONIFIED))
		return;
	
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
	
	if (editCtx->vg == NULL)
		printf_error("Could not init nanovg.");
	editCtx->fontCtx.notoSansID = nvgCreateFont(editCtx->vg, "sans", "NotoSans-Regular.ttf");
	if (editCtx->fontCtx.notoSansID < 0) {
		printf_debugExt("Could not load Font");
	}
	
	Rcp_Init();
	Theme_Init(0);
	GeoGrid_Init(
		&editCtx->geoCtx,
		&editCtx->appInfo.winDim,
		&editCtx->inputCtx.mouse,
		editCtx->vg
	);
	Cursor_Init(&editCtx->cursorCtx);
	
	glfwSetWindowSizeLimits(
		editCtx->appInfo.mainWindow,
		400,
		200,
		GLFW_DONT_CARE,
		GLFW_DONT_CARE
	);
}