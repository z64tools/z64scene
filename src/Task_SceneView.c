#include "Editor.h"

void EnSceneView_Init(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	SceneView* this = instance;
	
	View_Init(&this->viewCtx, &editCtx->inputCtx);
	
	MemFile_LoadFile(&editCtx->objCtx.zobj, "zobj.zobj");
	SkelAnime_Init(&editCtx->objCtx.zobj, &this->skelAnime, 0x0600E988, 0x06010808, this->jointTable);
	// MemFile_LoadFile(&editCtx->objCtx.zobj, "object_sd.zobj");
}

void EnSceneView_Destroy(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	SceneView* this = instance;
	
	OsPrintfEx("Destroy");
}

void EnSceneView_Update(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	SceneView* this = instance;
	InputContext* inputCtx = &editCtx->inputCtx;
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
			
			glfwSetCursorPos(editCtx->appInfo.mainWindow, split->edge[EDGE_R]->pos - extGrabDist, editCtx->inputCtx.mouse.pos.y);
			mouse->jumpVelComp.x =
			    (split->edge[EDGE_L]->pos + extGrabDist) -
			    (split->edge[EDGE_R]->pos - extGrabDist) + rel;
			OsPrintfEx("%d", rel);
		}
		
		if (mouse->pos.x > split->edge[EDGE_R]->pos - extGrabDist) {
			rel = mouse->pos.x - split->edge[EDGE_R]->pos + extGrabDist;
			
			glfwSetCursorPos(editCtx->appInfo.mainWindow, split->edge[EDGE_L]->pos + extGrabDist, editCtx->inputCtx.mouse.pos.y);
			mouse->jumpVelComp.x =
			    (split->edge[EDGE_R]->pos - extGrabDist) -
			    (split->edge[EDGE_L]->pos + extGrabDist) + rel;
			OsPrintfEx("%d", rel);
		}
		
		if (mouse->pos.y < split->edge[EDGE_T]->pos + extGrabDist) {
			rel = mouse->pos.y - split->edge[EDGE_T]->pos - extGrabDist;
			
			glfwSetCursorPos(editCtx->appInfo.mainWindow, editCtx->inputCtx.mouse.pos.x, split->edge[EDGE_B]->pos - extGrabDist - SPLIT_BAR_HEIGHT);
			mouse->jumpVelComp.y =
			    (split->edge[EDGE_T]->pos + extGrabDist) -
			    (split->edge[EDGE_B]->pos - extGrabDist - SPLIT_BAR_HEIGHT) + rel;
			OsPrintfEx("%d", rel);
		}
		
		if (mouse->pos.y > split->edge[EDGE_B]->pos - extGrabDist - SPLIT_BAR_HEIGHT) {
			rel = mouse->pos.y - split->edge[EDGE_B]->pos + extGrabDist + SPLIT_BAR_HEIGHT;
			
			glfwSetCursorPos(editCtx->appInfo.mainWindow, editCtx->inputCtx.mouse.pos.x, split->edge[EDGE_T]->pos + extGrabDist);
			mouse->jumpVelComp.y =
			    (split->edge[EDGE_B]->pos - extGrabDist - SPLIT_BAR_HEIGHT) -
			    (split->edge[EDGE_T]->pos + extGrabDist) + rel;
			OsPrintfEx("%d", rel);
		}
	}
}

void EnSceneView_Draw(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	SceneView* this = instance;
	Vec2s dim = {
		split->rect.w,
		split->rect.h
	};
	Mtx mtx[800];
	
	View_SetProjectionDimensions(&this->viewCtx, &dim);
	View_Update(&this->viewCtx, &editCtx->inputCtx);
	Matrix_Translate(0, 0, 0, MTXMODE_NEW);
	Matrix_Push();
	z64_Draw_SetScene(&editCtx->objCtx.scene);
	z64_Draw_Room(&editCtx->objCtx.room[0]);
	
	#if 1
	SkelAnime_Update(&this->skelAnime);
	
	Matrix_Translate(0, 0, 0, MTXMODE_NEW);
	Matrix_Scale(0.01, 0.01, 0.01, MTXMODE_APPLY);
	gSPSegment(0x8, SEGMENTED_TO_VIRTUAL(0x06000000));
	gSPSegment(0x9, SEGMENTED_TO_VIRTUAL(0x06004800));
	SkelAnime_Draw(&this->skelAnime, mtx, this->jointTable);
	// SkelAnime_Draw(&editCtx->objCtx.zobj, 0x0600bac8, NULL);
	#endif
	
	Matrix_Pop();
	
	char buffer[1024];
	
	sprintf(
		buffer,
		"%.2f %.2f %.2f",
		this->viewCtx.currentCamera->eye.x,
		this->viewCtx.currentCamera->eye.y,
		this->viewCtx.currentCamera->eye.z
	);
	nvgFontSize(editCtx->vg, SPLIT_TEXT_SCALE);
	nvgFontFace(editCtx->vg, "sans");
	nvgTextAlign(editCtx->vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	
	nvgFillColor(editCtx->vg, Theme_GetColor(THEME_TEXT, 200));
	nvgFontBlur(editCtx->vg, 0.0f);
	nvgText(
		editCtx->vg,
		20,
		20,
		buffer,
		NULL
	);
}
