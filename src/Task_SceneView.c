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
		s16 xMin = split->edge[EDGE_L]->pos + extGrabDist;
		s16 xMax = split->edge[EDGE_R]->pos - extGrabDist;
		s16 yMin = split->edge[EDGE_T]->pos + extGrabDist;
		s16 yMax = split->edge[EDGE_B]->pos - extGrabDist - SPLIT_BAR_HEIGHT;
		
		if (mouse->pos.x < xMin || mouse->pos.x > xMax) {
			s16 oldPos = mouse->pos.x;
			s16 newPos = Wrap(mouse->pos.x, xMin, xMax);
			
			mouse->jumpVelComp.x = oldPos - newPos;
			
			glfwSetCursorPos(
				editCtx->appInfo.mainWindow,
				newPos,
				editCtx->inputCtx.mouse.pos.y
			);
		}
		
		if (mouse->pos.y < yMin || mouse->pos.y > yMax) {
			s16 oldPos = mouse->pos.y;
			s16 newPos = Wrap(mouse->pos.y, yMin, yMax);
			
			mouse->jumpVelComp.y = oldPos - newPos;
			
			glfwSetCursorPos(
				editCtx->appInfo.mainWindow,
				editCtx->inputCtx.mouse.pos.x,
				newPos
			);
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
	
	Matrix_Scale(0.01, 0.01, 0.01, MTXMODE_NEW);
	Matrix_Translate(0, 0, 0, MTXMODE_APPLY);
	gSPSegment(0x8, SEGMENTED_TO_VIRTUAL(0x06000000));
	gSPSegment(0x9, SEGMENTED_TO_VIRTUAL(0x06004800));
	SkelAnime_Draw(&this->skelAnime, mtx, this->jointTable);
	// SkelAnime_Draw(&editCtx->objCtx.zobj, 0x0600bac8, NULL);
	#endif
	
	Matrix_Pop();
}
