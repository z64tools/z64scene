#include "Editor.h"

void EnSceneView_Init(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	SceneView* this = instance;
	
	View_Init(&this->viewCtx, &editCtx->inputCtx);
	
	MemFile_LoadFile(&editCtx->objCtx.zobj, "zobj.zobj");
	SkelAnime_Init(
		&editCtx->objCtx.zobj,
		&this->skelAnime,
		0x0600E988,
		0x06010808,
		this->jointTable,
		this->morphTable
	);
	this->skelAnime.playSpeed = 0.3f;
	
	MemFile_LoadFile(&editCtx->objCtx.scene, "scene.zscene");
	for (s32 i = 0; i < 32; i++) {
		char buffer[64];
		
		sprintf(buffer, "room_%d.zmap", i);
		MemFile_LoadFile(&editCtx->objCtx.room[i], buffer);
	}
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
	
	if (split->mouseInHeader && mouse->click.press) {
		this->headerClick = true;
	}
	
	if (this->headerClick == true && mouse->cursorAction == false) {
		this->headerClick = false;
	}
	
	if (this->headerClick == true) {
		return;
	}
	
	// Cursor Wrapping
	if (this->viewCtx.setCamMove == true) {
		s16 xMin = split->edge[EDGE_L]->pos;
		s16 xMax = split->edge[EDGE_R]->pos;
		s16 yMin = split->edge[EDGE_T]->pos;
		s16 yMax = split->edge[EDGE_B]->pos - SPLIT_BAR_HEIGHT;
		
		Cursor_ForceCursor(CURSOR_DEFAULT);
		
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
	} else {
		this->viewCtx.cameraControl = false;
		if (split->blockMouse == false) {
			if (split->mouseInSplit && !split->mouseInHeader) {
				this->viewCtx.cameraControl = true;
			}
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
	static s16 frame;
	static s8 eyeId;
	u32 eye[] = {
		0x06000000,
		0x06000800,
		0x06001000,
	};
	
	if (Zelda64_20fpsLimiter())
		eyeId = Zelda64_EyeBlink(&frame);
	
	View_SetProjectionDimensions(&this->viewCtx, &dim);
	View_Update(&this->viewCtx, &editCtx->inputCtx);
	
	for (int i = 0x8; i < 0x10; ++i)
		gSPSegment(i, 0);
	z64_Draw_SetScene(&editCtx->objCtx.scene);
	for (s32 i = 0; i < 32; i++) {
		if (editCtx->objCtx.room[i].data != NULL)
			z64_Draw_Room(&editCtx->objCtx.room[i]);
	}
	
	SkelAnime_Update(&this->skelAnime);
	
	Matrix_Push(); {
		Matrix_Scale(0.01, 0.01, 0.01, MTXMODE_APPLY);
		Matrix_Translate(0, 0, 0, MTXMODE_APPLY);
		gSPSegment(0x8, SEGMENTED_TO_VIRTUAL(eye[eyeId]));
		gSPSegment(0x9, SEGMENTED_TO_VIRTUAL(0x06004800));
		SkelAnime_Draw(&this->skelAnime, mtx, this->jointTable);
	} Matrix_Pop();
}
