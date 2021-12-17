#include "Editor.h"

void EnSceneView_Init(void* passArg, void* instance, Split* split) {
	EditorContext* editorCtx = passArg;
	SceneView* this = instance;
	
	View_Init(&this->viewCtx, &editorCtx->inputCtx);
	
	MemFile_LoadFile(&editorCtx->objCtx.zobj, "zobj.zobj");
	SkelAnime_Init(&editorCtx->objCtx.zobj, &this->skelAnime, 0x0600E988, 0x06010808, this->jointTable);
	// MemFile_LoadFile(&editorCtx->objCtx.zobj, "object_sd.zobj");
}

void EnSceneView_Destroy(void* passArg, void* instance, Split* split) {
	EditorContext* editorCtx = passArg;
	SceneView* this = instance;
	
	OsPrintfEx("Destroy");
}

void EnSceneView_Update(void* passArg, void* instance, Split* split) {
	EditorContext* editorCtx = passArg;
	SceneView* this = instance;
	InputContext* inputCtx = &editorCtx->inputCtx;
	MouseInput* mouse = &inputCtx->mouse;
	Vec2s dim = {
		split->rect.w,
		split->rect.h
	};
	u32 extGrabDist = SPLIT_GRAB_DIST * 1.5;
	
	this->viewCtx.cameraControl = false;
	if (split->blockMouse == false) {
		if (split->mouseInSplit && !split->mouseInHeader) {
			this->viewCtx.cameraControl = true;
		}
	}
	
	// Cursor Wrapping
	if (this->viewCtx.setCamMove == true && (mouse->vel.x || mouse->vel.y)) {
		s16 rel;
		if (mouse->pos.x < split->edge[EDGE_L]->pos + extGrabDist) {
			rel = mouse->pos.x - split->edge[EDGE_L]->pos - extGrabDist;
			
			glfwSetCursorPos(editorCtx->appInfo.mainWindow, split->edge[EDGE_R]->pos - extGrabDist, editorCtx->inputCtx.mouse.pos.y);
			mouse->jumpVelComp.x =
			    (split->edge[EDGE_L]->pos + extGrabDist) -
			    (split->edge[EDGE_R]->pos - extGrabDist) + rel;
			OsPrintfEx("%d", rel);
		}
		
		if (mouse->pos.x > split->edge[EDGE_R]->pos - extGrabDist) {
			rel = mouse->pos.x - split->edge[EDGE_R]->pos + extGrabDist;
			
			glfwSetCursorPos(editorCtx->appInfo.mainWindow, split->edge[EDGE_L]->pos + extGrabDist, editorCtx->inputCtx.mouse.pos.y);
			mouse->jumpVelComp.x =
			    (split->edge[EDGE_R]->pos - extGrabDist) -
			    (split->edge[EDGE_L]->pos + extGrabDist) + rel;
			OsPrintfEx("%d", rel);
		}
		
		if (mouse->pos.y < split->edge[EDGE_T]->pos + extGrabDist) {
			rel = mouse->pos.y - split->edge[EDGE_T]->pos - extGrabDist;
			
			glfwSetCursorPos(editorCtx->appInfo.mainWindow, editorCtx->inputCtx.mouse.pos.x, split->edge[EDGE_B]->pos - extGrabDist - SPLIT_BAR_HEIGHT);
			mouse->jumpVelComp.y =
			    (split->edge[EDGE_T]->pos + extGrabDist) -
			    (split->edge[EDGE_B]->pos - extGrabDist - SPLIT_BAR_HEIGHT) + rel;
			OsPrintfEx("%d", rel);
		}
		
		if (mouse->pos.y > split->edge[EDGE_B]->pos - extGrabDist - SPLIT_BAR_HEIGHT) {
			rel = mouse->pos.y - split->edge[EDGE_B]->pos + extGrabDist + SPLIT_BAR_HEIGHT;
			
			glfwSetCursorPos(editorCtx->appInfo.mainWindow, editorCtx->inputCtx.mouse.pos.x, split->edge[EDGE_T]->pos + extGrabDist);
			mouse->jumpVelComp.y =
			    (split->edge[EDGE_B]->pos - extGrabDist - SPLIT_BAR_HEIGHT) -
			    (split->edge[EDGE_T]->pos + extGrabDist) + rel;
			OsPrintfEx("%d", rel);
		}
	}
}

void EnSceneView_Draw(void* passArg, void* instance, Split* split) {
	EditorContext* editorCtx = passArg;
	SceneView* this = instance;
	Vec2s dim = {
		split->rect.w,
		split->rect.h
	};
	Mtx mtx[800];
	
	View_SetProjectionDimensions(&this->viewCtx, &dim);
	View_Update(&this->viewCtx, &editorCtx->inputCtx);
	z64_Draw_SetScene(&editorCtx->objCtx.scene);
	z64_Draw_Room(&editorCtx->objCtx.room[0]);
	
	#if 1
	SkelAnime_Update(&this->skelAnime);
	
	Matrix_Translate(0, 0, 0, MTXMODE_NEW);
	Matrix_Scale(0.01, 0.01, 0.01, MTXMODE_APPLY);
	SkelAnime_Draw(&this->skelAnime, mtx, this->jointTable);
	// SkelAnime_Draw(&editorCtx->objCtx.zobj, 0x0600bac8, NULL);
	#endif
}
