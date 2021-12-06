#include "z64scene.h"

static EditorContext* editorCtx;

int main(void) {
	editorCtx = Lib_Malloc(0, sizeof(EditorContext));
	bzero(editorCtx, sizeof(EditorContext));
	z64scene_Init(editorCtx);
	OsPrintfEx("Initialization OK");
	
	while (!glfwWindowShouldClose(editorCtx->appInfo.mainWindow)) {
		AppInfo* appInfo = &editorCtx->appInfo;
		InputContext* inputCtx = &editorCtx->inputCtx;
		LightContext* lightCtx = &editorCtx->lightCtx;
		ViewContext* viewCtx = &editorCtx->viewCtx;
		
		Input_Update(inputCtx, appInfo);
		View_Update(viewCtx, inputCtx, &appInfo->winScale);
		Input_End(inputCtx);
		glfwPollEvents();
		
		glClearColor(
			lightCtx->ambient.r,
			lightCtx->ambient.g,
			lightCtx->ambient.b,
			1.0f
		);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		z64scene_Draw3DViewport(editorCtx);
		z64scene_DrawGUI(editorCtx);
		
		glfwSwapBuffers(appInfo->mainWindow);
	}
	
	glfwTerminate();
}
