#include "Editor.h"

s32 gSceneConfIndex = 0;
s64 sFirstInit = 0;

void EnSceneView_Init(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnSceneView* this = instance;
	
	View_Init(&this->viewCtx, &editCtx->inputCtx);
	
	// if (!MemFile_LoadFile(&editCtx->zobj, "zobj.zobj")) {
	// 	gSegment[6] = editCtx->zobj.data;
	// 	SkelAnime_Init(
	// 		&editCtx->zobj,
	// 		&this->skelAnime,
	// 		0x0600E988,
	// 		0x06010808,
	// 		this->jointTable,
	// 		this->morphTable
	// 	);
	// 	this->skelAnime.playSpeed = 0.3f;
	// }
	
	// MemFile_LoadFile(&editCtx->scene.file, "scene.zscene");
	// Scene_ExecuteCommands(&editCtx->scene, NULL);
	//
	// for (s32 i = 0; i < 32; i++) {
	// 	char buffer[64];
	//
	// 	sprintf(buffer, "room_%d.zmap", i);
	// 	if (MemFile_LoadFile(&editCtx->room[i].file, buffer)) {
	// 		printf_debugExt("Break");
	// 		break;
	// 	}
	//
	// 	Scene_ExecuteCommands(&editCtx->scene, &editCtx->room[i]);
	// }
	
	// editCtx->scene.lightCtx.state |= LIGHT_STATE_CHANGED;
	split->bg.useCustomBG = true;
	this->viewCtx.matchDrawDist = true;
}

void EnSceneView_Destroy(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnSceneView* this = instance;
	
	split->bg.useCustomBG = false;
}

void EnSceneView_Update(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnSceneView* this = instance;
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
			
			Input_SetMousePos(newPos, MOUSE_KEEP_AXIS);
		}
		
		if (mouse->pos.y < yMin || mouse->pos.y > yMax) {
			s16 oldPos = mouse->pos.y;
			s16 newPos = Wrap(mouse->pos.y, yMin, yMax);
			
			Input_SetMousePos(MOUSE_KEEP_AXIS, newPos);
		}
	}
	
	if (editCtx->scene.file.data != NULL) {
		n64_set_segment(0x02, editCtx->scene.file.data);
		Light_SetFog(&editCtx->scene, &this->viewCtx);
		envLight = &lightCtx->envLight[lightCtx->curEnvId];
		split->bg.color = (RGB8) {
			envLight->fogColor.r,
			envLight->fogColor.g,
			envLight->fogColor.b
		};
	}
}

void EnSceneView_Draw(void* passArg, void* instance, Split* split) {
	EditorContext* editCtx = passArg;
	EnSceneView* this = instance;
	LightContext* lightCtx = &editCtx->scene.lightCtx;
	ViewContext* viewCtx = &this->viewCtx;
	Vec2s dim = {
		split->rect.w,
		split->rect.h
	};
	static s16 frame;
	static s8 eyeId;
	u32 eye[] = {
		0x06000000,
		0x06000800,
		0x06001000,
	};
	
	if (Zelda64_20fpsLimiter())
		gGameplayFrames++;
	
	Graph_Alloc(GRAPH_INIT);
	n64_assign_triangle(TRI_INIT);
	n64_ClearSegments();
	gPolyOpaDisp = gPolyOpaHead;
	
	View_SetProjectionDimensions(&this->viewCtx, &dim);
	View_Update(&this->viewCtx, &editCtx->inputCtx);
	
	if (editCtx->scene.file.data != NULL) {
		gxSPSegment(POLY_OPA_DISP++, 0x2, editCtx->scene.file.data);
		gSceneDrawConf[gSceneConfIndex](&editCtx->scene);
		for (s32 i = 0; i < ROOM_MAX; i++) {
			if (editCtx->room[i].file.data != NULL) {
				Light_BindEnvLights(&editCtx->scene, &editCtx->room[0]);
				Light_BindRoomLights(&editCtx->scene, &editCtx->room[i]);
				Room_Draw(&editCtx->room[i]);
			}
		}
	}
	
	if (editCtx->zobj.data) {
		if (Zelda64_20fpsLimiter())
			eyeId = Zelda64_EyeBlink(&frame);
		gxSPSegment(POLY_OPA_DISP++, 0x6, editCtx->zobj.data);
		gSPSegment(POLY_OPA_DISP++, 0x8, SEGMENTED_TO_VIRTUAL(eye[eyeId]));
		gSPSegment(POLY_OPA_DISP++, 0x9, SEGMENTED_TO_VIRTUAL(0x06004800));
		SkelAnime_Update(&this->skelAnime);
		Matrix_Push(); {
			gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
			gDPSetEnvColor(POLY_OPA_DISP++, 0xFF, 0xFF, 0xFF, 0xFF);
			Matrix_Scale(0.01, 0.01, 0.01, MTXMODE_APPLY);
			Matrix_Translate(0, 0, 0, MTXMODE_APPLY);
			SkelAnime_Draw(&this->skelAnime, SKELANIME_FLEX, this->jointTable);
		} Matrix_Pop();
	}
	
	gSPEndDisplayList(POLY_OPA_DISP++);
	n64_draw(gPolyOpaHead);
	
	if (editCtx->scene.file.data == NULL && editCtx->zobj.data == NULL) {
		void* vg = editCtx->vg;
		const char* txt = "z64scene";
		static f32 fontSize = 1;
		f32 boundr[4];
		
		nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgFontBlur(vg, 0.0);
		
		nvgFontFace(vg, "font-bold");
		nvgFontSize(vg, 35);
		nvgTextLetterSpacing(vg, 2.0f);
		nvgFillColor(vg, Theme_GetColor(THEME_PRIM, 355));
		nvgText(
			vg,
			split->cect.w * 0.5,
			split->cect.h * 0.5,
			txt,
			NULL
		);
		
		nvgFontFace(vg, "font-basic");
		nvgFontSize(vg, 15);
		nvgTextLetterSpacing(vg, 0.0f);
		nvgFillColor(vg, Theme_GetColor(THEME_LINE, 755));
		nvgText(
			vg,
			split->cect.w * 0.5,
			split->cect.h * 0.5 + 35 * 0.75f,
			"drop files here",
			NULL
		);
	}
}
