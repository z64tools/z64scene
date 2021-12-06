#include "Editor.h"
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

void Editor_Init(EditorContext* editorCtx) {
	printf_SetPrefix("");
	printf_SetSuppressLevel(PSL_DEBUG);
	MemFile_LoadFile(&editorCtx->objCtx.scene, "scene.zscene");
	MemFile_LoadFile(&editorCtx->objCtx.room[0], "room_0.zmap");
	
	editorCtx->appInfo.mainCtx = editorCtx;
	editorCtx->appInfo.drawCall = (CallDraw)Editor_Draw;
	
	editorCtx->appInfo.winScale.x = 1520;
	editorCtx->appInfo.winScale.y = 740;
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif
	
	editorCtx->appInfo.mainWindow = glfwCreateWindow(
		editorCtx->appInfo.winScale.x,
		editorCtx->appInfo.winScale.y,
		"z64scene",
		NULL,
		NULL
	);
	if (editorCtx->appInfo.mainWindow == NULL) {
		printf_error("Failed to create GLFW window.");
	}
	glfwMakeContextCurrent(editorCtx->appInfo.mainWindow);
	
	glfwSetFramebufferSizeCallback(editorCtx->appInfo.mainWindow, View_FramebufferCallback);
	glfwSetCursorPosCallback(editorCtx->appInfo.mainWindow, Input_CursorCallback);
	glfwSetMouseButtonCallback(editorCtx->appInfo.mainWindow, Input_MouseClickCallback);
	glfwSetKeyCallback(editorCtx->appInfo.mainWindow, Input_KeyCallback);
	glfwSetScrollCallback(editorCtx->appInfo.mainWindow, Input_ScrollCallback);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf_error("Failed to initialize GLAD.");
	}
	
	Matrix_Init();
	View_Init(&editorCtx->viewCtx, &editorCtx->inputCtx, &editorCtx->appInfo);
	Input_SetInputPointer(&editorCtx->inputCtx);
	glfwSetTime(2);
	
	editorCtx->vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	if (editorCtx->vg == NULL)
		printf_error("Could not init nanovg.");
	editorCtx->fontCtx.notoSansID = nvgCreateFont(editorCtx->vg, "sans", "NotoSans-Regular.ttf");
	
	Element_Spawn(editorCtx, 0, 12, (Vec2f) { 0 }, "[Side Panel]");
}

void Editor_Draw_2DElements(EditorContext* editorCtx) {
	// float pxRatio;
	
	// pxRatio = (float)editorCtx->appInfo.winScale.x / (float)editorCtx->appInfo.winScale.y;
	nvgBeginFrame(editorCtx->vg, editorCtx->appInfo.winScale.x, editorCtx->appInfo.winScale.y, 1.0f);
	
	Element_UpdateElements(editorCtx, &editorCtx->elemCtx);
	Element_DrawElements(editorCtx, &editorCtx->elemCtx);
	
	nvgFillColor(editorCtx->vg, Element_GetColor(GUICOL_BASE_WHITE));
	nvgFontSize(editorCtx->vg, 17.5f);
	nvgFontFace(editorCtx->vg, "sans");
	nvgTextAlign(editorCtx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	
	char txt[2048];
	
	sprintf(txt, "M: x%4d y%4d", editorCtx->inputCtx.mouse.pos.x, editorCtx->inputCtx.mouse.pos.y);
	nvgTextBox(editorCtx->vg, 10.0f, 10.0f, 150, txt, NULL);
	sprintf(txt, "W: x%4d y%4d", (s32)editorCtx->appInfo.winScale.x, (s32)editorCtx->appInfo.winScale.y);
	nvgTextBox(editorCtx->vg, 10.0f, 10.0f + 17.5f, 150, txt, NULL);
	
	nvgEndFrame(editorCtx->vg);
}

void Editor_Draw_3DViewport(EditorContext* editorCtx) {
	u8 setup[16] = {
		0xfb, 0, 0, 0, 0x80, 0x80, 0x80, 0x80,
		0xdf
	};
	
	n64_set_segment(0x02, editorCtx->objCtx.scene.data);
	Light_Scene_SetLights(&editorCtx->objCtx.scene, &editorCtx->lightCtx);
	
	n64_draw(setup);
	zroom_draw(editorCtx->objCtx.room[0].data);
}

void Editor_Draw(EditorContext* editorCtx) {
	AppInfo* appInfo = &editorCtx->appInfo;
	InputContext* inputCtx = &editorCtx->inputCtx;
	LightContext* lightCtx = &editorCtx->lightCtx;
	ViewContext* viewCtx = &editorCtx->viewCtx;
	
	Input_Update(inputCtx, appInfo);
	View_Update(viewCtx, inputCtx, &appInfo->winScale);
	Input_End(inputCtx);
	
	glClearColor(
		lightCtx->ambient.r,
		lightCtx->ambient.g,
		lightCtx->ambient.b,
		1.0f
	);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	Editor_Draw_3DViewport(editorCtx);
	Editor_Draw_2DElements(editorCtx);
	
	glfwSwapBuffers(appInfo->mainWindow);
}