#include "Editor.h"

void EnSceneView_Init(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	SceneView* this = instance;
	
	View_Init(&this->viewCtx, &editCtx->inputCtx);
	
	if (!MemFile_LoadFile(&editCtx->zobj, "zobj.zobj")) {
		SkelAnime_Init(
			&editCtx->zobj,
			&this->skelAnime,
			0x0600E988,
			0x06010808,
			this->jointTable,
			this->morphTable
		);
		this->skelAnime.playSpeed = 0.3f;
	}
	
	MemFile_LoadFile(&editCtx->scene.file, "scene.zscene");
	Scene_ExecuteCommands(&editCtx->scene, NULL);
	
	for (s32 i = 0; i < 32; i++) {
		char buffer[64];
		
		sprintf(buffer, "room_%d.zmap", i);
		if (MemFile_LoadFile(&editCtx->room[i].file, buffer)) {
			OsPrintfEx("Break");
			break;
		}
		
		Scene_ExecuteCommands(&editCtx->scene, &editCtx->room[i]);
	}
	
	editCtx->scene.lightCtx.state |= LIGHT_STATE_CHANGED;
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
	LightContext* lightCtx = &editCtx->scene.lightCtx;
	EnvLight* envLight = &lightCtx->envLight[lightCtx->curEnvId];
	Vec2s dim = {
		split->rect.w,
		split->rect.h
	};
	
	if (inputCtx->key[KEY_F].hold) {
		if (inputCtx->key[KEY_LEFT].press) {
			s16 near = ReadBE(envLight->fogNear) & 0x3FF;
			near--;
			envLight->fogNear = ReadBE(near);
			lightCtx->state |= LIGHT_STATE_CHANGED;
		}
		
		if (inputCtx->key[KEY_RIGHT].press) {
			s16 near = ReadBE(envLight->fogNear) & 0x3FF;
			near++;
			envLight->fogNear = ReadBE(near);
			lightCtx->state |= LIGHT_STATE_CHANGED;
		}
	}
	
	if (inputCtx->key[KEY_UP].press) {
		lightCtx->curEnvId = Wrap(lightCtx->curEnvId + 1, 0, lightCtx->envListNum - 1);
		lightCtx->state |= LIGHT_STATE_CHANGED;
	}
	
	if (inputCtx->key[KEY_DOWN].press) {
		lightCtx->curEnvId = Wrap(lightCtx->curEnvId - 1, 0, lightCtx->envListNum - 1);
		lightCtx->state |= LIGHT_STATE_CHANGED;
	}
	
	if (split->mouseInHeader && mouse->click.press) {
		this->headerClick = true;
	}
	
	if (this->headerClick == true && mouse->cursorAction == false) {
		this->headerClick = false;
	}
	
	if (this->viewCtx.setCamMove == false) {
		this->viewCtx.cameraControl = false;
		if (split->blockMouse == false) {
			if (split->mouseInSplit && !split->mouseInHeader && !this->headerClick) {
				this->viewCtx.cameraControl = true;
			}
		}
	}
	
	// Cursor Wrapping
	if (this->viewCtx.setCamMove && this->viewCtx.cameraControl) {
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
	}
}

void EnSceneView_Draw(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	SceneView* this = instance;
	LightContext* lightCtx = &editCtx->scene.lightCtx;
	ViewContext* viewCtx = &this->viewCtx;
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
	
	n64_ClearSegments();
	gSPSegment(0x02, editCtx->scene.file.data);
	Light_SetFog(&editCtx->scene, viewCtx);
	View_SetProjectionDimensions(&this->viewCtx, &dim);
	View_Update(&this->viewCtx, &editCtx->inputCtx);
	
	for (s32 i = 0; i < 32; i++) {
		if (editCtx->room[i].file.data != NULL) {
			Light_BindEnvLights(&editCtx->scene);
			Light_BindRoomLights(&editCtx->scene, &editCtx->room[i]);
			Room_Draw(&editCtx->room[i]);
		}
	}
	
	if (editCtx->zobj.data) {
		SkelAnime_Update(&this->skelAnime);
		Matrix_Push(); {
			Matrix_Scale(0.01, 0.01, 0.01, MTXMODE_APPLY);
			Matrix_Translate(0, 0, 0, MTXMODE_APPLY);
			gSPSegment(0x8, SEGMENTED_TO_VIRTUAL(eye[eyeId]));
			gSPSegment(0x9, SEGMENTED_TO_VIRTUAL(0x06004800));
			SkelAnime_Draw(&this->skelAnime, mtx, this->jointTable);
		} Matrix_Pop();
	}
}
