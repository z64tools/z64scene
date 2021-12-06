#include "z64scene.h"
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

void z64scene_Init(Z64SceneContext* z64Ctx) {
	printf_SetPrefix("");
	printf_SetSuppressLevel(PSL_DEBUG);
	MemFile_LoadFile(&z64Ctx->objCtx.scene, "scene.zscene");
	MemFile_LoadFile(&z64Ctx->objCtx.room[0], "room_0.zmap");
	
	z64Ctx->appInfo.winScale.x = 1400;
	z64Ctx->appInfo.winScale.y = 700;
	// z64Ctx->appInfo.viewportScale.x = 800;
	// z64Ctx->appInfo.viewportScale.y = 600;
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif
	
	z64Ctx->appInfo.mainWindow = glfwCreateWindow(
		z64Ctx->appInfo.winScale.x,
		z64Ctx->appInfo.winScale.y,
		"z64scene",
		NULL,
		NULL
	);
	if (z64Ctx->appInfo.mainWindow == NULL) {
		printf_error("Failed to create GLFW window.");
	}
	glfwMakeContextCurrent(z64Ctx->appInfo.mainWindow);
	
	glfwSetFramebufferSizeCallback(z64Ctx->appInfo.mainWindow, View_FramebufferCallback);
	glfwSetCursorPosCallback(z64Ctx->appInfo.mainWindow, Input_CursorCallback);
	glfwSetMouseButtonCallback(z64Ctx->appInfo.mainWindow, Input_MouseClickCallback);
	glfwSetKeyCallback(z64Ctx->appInfo.mainWindow, Input_KeyCallback);
	glfwSetScrollCallback(z64Ctx->appInfo.mainWindow, Input_ScrollCallback);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf_error("Failed to initialize GLAD.");
	}
	
	Matrix_Init();
	View_Init(&z64Ctx->viewCtx, &z64Ctx->inputCtx, &z64Ctx->appInfo);
	Input_SetInputPointer(&z64Ctx->inputCtx);
	glfwSetTime(2);
	
	z64Ctx->vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	if (z64Ctx->vg == NULL)
		printf_error("Could not init nanovg.");
	z64Ctx->fontCtx.notoSansID = nvgCreateFont(z64Ctx->vg, "sans", "NotoSans-Regular.ttf");
}

void z64scene_DrawGUI(Z64SceneContext* z64Ctx) {
	float pxRatio;
	
	pxRatio = (float)z64Ctx->appInfo.winScale.x / (float)z64Ctx->appInfo.winScale.y;
	nvgBeginFrame(z64Ctx->vg, z64Ctx->appInfo.winScale.x, z64Ctx->appInfo.winScale.y, pxRatio);
	
	Gui_SidePanel(z64Ctx);
	
	nvgEndFrame(z64Ctx->vg);
}

void z64scene_Draw3DViewport(Z64SceneContext* z64Ctx) {
	u8 setup[16] = {
		0xfb, 0, 0, 0, 0x80, 0x80, 0x80, 0x80,
		0xdf
	};
	
	n64_set_segment(0x02, z64Ctx->objCtx.scene.data);
	Light_Scene_SetLights(&z64Ctx->objCtx.scene, &z64Ctx->lightCtx);
	
	n64_draw(setup);
	zroom_draw(z64Ctx->objCtx.room[0].data);
}