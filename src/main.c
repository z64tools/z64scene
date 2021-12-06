#include "Editor.h"

static EditorContext* editorCtx;

int main(void) {
	editorCtx = Lib_Malloc(0, sizeof(EditorContext));
	bzero(editorCtx, sizeof(EditorContext));
	Editor_Init(editorCtx);
	OsPrintfEx("Initialization OK");
	
	while (!glfwWindowShouldClose(editorCtx->appInfo.mainWindow)) {
		glfwPollEvents();
		Editor_Draw(editorCtx);
	}
	
	glfwTerminate();
}
