#include "z64scene.h"

static Z64SceneContext* z64Ctx;

int main(void) {
	z64Ctx = Lib_Malloc(0, sizeof(Z64SceneContext));
	bzero(z64Ctx, sizeof(Z64SceneContext));
	z64scene_Init(z64Ctx);
	
	while (!glfwWindowShouldClose(z64Ctx->appInfo.mainWindow)) {
		AppInfo* appInfo = &z64Ctx->appInfo;
		InputContext* inputCtx = &z64Ctx->inputCtx;
		LightContext* lightCtx = &z64Ctx->lightCtx;
		ViewContext* viewCtx = &z64Ctx->viewCtx;
		
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
		
		z64scene_Draw3DViewport(z64Ctx);
		z64scene_DrawGUI(z64Ctx);
		
		glfwSwapBuffers(appInfo->mainWindow);
	}
	
	glfwTerminate();
}
