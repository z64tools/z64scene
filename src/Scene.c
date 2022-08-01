#define __SCENE_TYPES_H__
#include "Editor.h"

void (*sSceneCmdHandlers[SCENE_CMD_ID_MAX])(Scene*, Room*, SceneCmd*);
char* sSceneCmdHandlers_Name[SCENE_CMD_ID_MAX];

// # # # # # # # # # # # # # # # # # # # #
// # Scene                               #
// # # # # # # # # # # # # # # # # # # # #

void Scene_LoadScene(Scene* this, const char* file) {
	Log("Load File [%s]", file);
	MemFile_LoadFile(&this->file, file);
	this->segment = this->file.data;
	
	n64_clearCache();
	Scene_ExecuteCommands(this, NULL);
	
	Calloc(this->room, sizeof(Room*) * 255);
	
	this->useFog = true;
}

void Scene_LoadRoom(Scene* this, const char* file) {
	Room* room;
	
	Log("Load File [%s]", file);
	Calloc(room, sizeof(Room));
	MemFile_LoadFile(&room->file, file);
	room->segment = room->file.data;
	
	Scene_ExecuteCommands(this, room);
	
	this->room[this->numRoom++] = room;
}

void Scene_Free(Scene* this) {
	MemFile_Free(&this->file);
	for (s32 i = 0; i < this->numRoom; i++) {
		MemFile_Free(&this->room[i]->file);
		TriBuffer_Free(&this->room[i]->triBuf);
		Free(this->room[i]);
	}
	Free(this->room);
	this->numRoom = 0;
	memset(this, 0, sizeof(*this));
	n64_clearCache();
}

void Scene_ExecuteCommands(Scene* this, Room* room) {
	u8* segment;
	
	gSegment[2] = this->segment;
	gSegment[3] = room ? room->segment : NULL;
	
	if (!room) {
		segment = gSegment[2];
		Log("" PRNT_REDD "SCENE" PRNT_RSET ": [%s]", this->file.info.name);
	} else {
		segment = gSegment[3];
		Log("" PRNT_BLUE "ROOM" PRNT_RSET ": [%s]", room->file.info.name);
	}
	
	for (SceneCmd* cmd = (void*)segment; cmd->base.code != SCENE_CMD_ID_END ; cmd++) {
		if (cmd->base.code == SCENE_CMD_ID_END)
			break;
		
		if (cmd->base.code == SCENE_CMD_ID_ALTERNATE_HEADER_LIST && this->setupHeader != 0) {
		} else if (cmd->base.code < SCENE_CMD_ID_MAX) {
			Log("[%02X] %08X [%s]", cmd->base.code, (u8*)cmd - segment, sSceneCmdHandlers_Name[cmd->base.code]);
			
			if (sSceneCmdHandlers[cmd->base.code])
				sSceneCmdHandlers[cmd->base.code](this, room, cmd);
			
			else if (cmd->base.code != SCENE_CMD_ID_ALTERNATE_HEADER_LIST)
				printf_warning("SceneCmd %02X", cmd->base.code);
		}
	}
}

void Scene_Draw(Scene* this) {
	static EnvLightSettings* prevEnv;
	EnvLightSettings* env = this->env + this->setupEnv;
	s8 l1n[3], l2n[3];
	u16 fogNear;
	
	Log("Begin");
	
	Assert(env != NULL);
	
	n64_set_onlyZmode(ZMODE_ALL);
	n64_set_onlyGeoLayer(GEOLAYER_ALL);
	
	memcpy(l1n, env->light1Dir, 3);
	memcpy(l2n, env->light2Dir, 3);
	
	if (this->indoorLight == false && this->setupEnv < 4) {
		u16 time;
		
		switch (this->setupEnv) {
			case 0:
				time = 0x6000;
				break;
				
			case 1:
				time = 0x8001;
				break;
				
			case 2:
				time = 0xB556;
				break;
				
			case 3:
				time = 0xFFFF;
				break;
		}
		
		l1n[0] = SinS((time - 0x8000)) * 120.0f;
		l1n[1] = CosS((time - 0x8000)) * 120.0f;
		l1n[2] = CosS((time - 0x8000)) * 20.0f;
		l2n[0] = -l1n[0];
		l2n[1] = -l1n[1];
		l2n[2] = -l1n[2];
	}
	
	if (this->useFog == false)
		fogNear = 1000;
	else
		fogNear = env->fogNear & 0x3FF;
	
	gSegment[2] = this->segment;
	gSPSegment(POLY_OPA_DISP++, 0x02, this->segment);
	gSPSegment(POLY_XLU_DISP++, 0x02, this->segment);
	
	if (prevEnv != env) {
		printf_hex("EnvLight", env, sizeof(*env), 0);
	}
	
	Light_SetAmbLight(env->ambientColor);
	Light_SetFog(fogNear, 0, env->fogColor);
	Light_SetDirLight(l1n, env->light1Color);
	Light_SetDirLight(l2n, env->light2Color);
	
	for (s32 i = 0; i < this->numRoom; i++) {
		Log("Room %d", i);
		Room_Draw(this, this->room[i]);
	}
	Log("OK");
	
	prevEnv = env;
	
	// TODO fix endianness so we don't need these
	u16 swapu16(u16 v) {
		return (v >> 8) | ((v & 0xff) << 8);
	}
	s16 swaps16(s16 v) {
		u16 v1 = v;
		return (v1 >> 8) | ((v1 & 0xff) << 8);
	}
#if 1 // Quick collision drawing test
	Matrix_Push(); if (this->colHeader) {
		CollisionHeader* colHeader = this->colHeader;
		static unsigned char simpleXlu[] = { // TODO use Gfx + macros
			0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE2, 0x00, 0x00, 0x1C,
			0x00, 0x50, 0x78, 0x48, 0xE3, 0x00, 0x12, 0x01, 0x00, 0x00, 0x20, 0x00,
			0xFC, 0xFF, 0xFE, 0x04, 0xFF, 0xFE, 0xFB, 0xF8, 0xD9, 0xF0, 0xFD, 0xFE,
			0x00, 0x20, 0x04, 0x04, 0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};
		static struct StructBE {
			Vec3s pos;
			u8 pad[10];
		} *vBuf = 0;
		static int vBufCount = 0;
		static Gfx* tri = 0;
		//static uint8_t *baked = 0;
		//static unsigned bakedOfs = 0;
		
		// Allocate three vertices per triangle
		if (vBufCount != colHeader->numPolygons * 3) {
			Vec3s* vtxList = colHeader->vtxList;
			u16 numVertices = colHeader->numVertices;
			u16 numPolygons = colHeader->numPolygons;
			Gfx* g;
			int i;
			int k;
			int u;
			
			vBufCount = numPolygons * 3;
			vBuf = realloc(vBuf, sizeof(*vBuf) * vBufCount);
			memset(vBuf, -1, sizeof(*vBuf) * vBufCount);
			
			for (i = k = 0; i < numPolygons; ++i) {
				CollisionPoly poly = colHeader->polyList[i];
				for (u = 0; u < 3; ++u, ++k)
					memcpy(&vBuf[k].pos, &vtxList[swapu16(poly.vtxData[u]) & 0x1fff], sizeof(*vtxList));
			}
			
			// Allocate triangle list
			// TODO unoptimized; 2 opcodes per triangle for now + a DF at the end
			tri = realloc(tri, sizeof(*tri) * (2 * (numPolygons + 1)));
			g = tri;
			for (i = 0; i < numPolygons; ++i)
			{
				u32 vaddr = 0x06000000 | (i * 16 * 3);
				gSPVertex(g++, vaddr, 3, 0);
				gSP1Triangle(g++, 0, 1, 2, 0);
			}
			gSPEndDisplayList(g++);
			
			// output test zobj
			if (false) {
				FILE* test = fopen("test.zobj", "wb+");
				/*for (i = 0; i < vBufCount; ++i) {
					fputc(vBuf[i].pos.x >> 8, test);
					fputc(vBuf[i].pos.x >> 0, test);
					fputc(vBuf[i].pos.y >> 8, test);
					fputc(vBuf[i].pos.y >> 0, test);
					fputc(vBuf[i].pos.z >> 8, test);
					fputc(vBuf[i].pos.z >> 0, test);
					for (k = 0; k < 10; ++k)
						fputc(0, test);
				}*/
				fwrite(vBuf, 1, sizeof(*vBuf) * vBufCount, test);
				//bakedOfs = ftell(test);
				fwrite(simpleXlu, 1, sizeof(simpleXlu) - 8, test);
				/*for (i = 0; i < sizeof(*tri) * (2 * (numPolygons + 1)); ++i)
				{
					fputc(((uint8_t*)tri)[i], test);
				}*/
				fwrite(tri, 1, sizeof(*tri) * (2 * (numPolygons + 1)), test);
				/*baked = malloc(ftell(test));
				unsigned end = ftell(test);
				fseek(test, 0, SEEK_SET);
				fread(baked, 1, end, test);
				fprintf(stderr, "%08x\n", *(u32*)(baked + 0x1A838));*/
				fclose(test);
			}
			
			// output wavefront obj
			if (false) {
				FILE* test = fopen("test.obj", "w");
				for (i = 0; i < vBufCount; ++i)
					fprintf(test, "v %d %d %d\n", swaps16(vBuf[i].pos.x), swaps16(vBuf[i].pos.y), swaps16(vBuf[i].pos.z));
				//for (int i = 0; i < numVertices; ++i)
				//	fprintf(test, "v %d %d %d\n", swap16(vtxList[i].x), swap16(vtxList[i].y), swap16(vtxList[i].z));
				for (i = 0; i < numPolygons * 3; i += 3)
					fprintf(test, "f %d %d %d\n", i + 1, i + 2, i + 3);
				fclose(test);
			}
		}
		
		// material
		gDPPipeSync(POLY_XLU_DISP++);
		gDPSetRenderMode(POLY_XLU_DISP++, AA_EN | IM_RD | CVG_DST_CLAMP | ZMODE_DEC | CVG_X_ALPHA | ALPHA_CVG_SEL | FORCE_BL | GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA), AA_EN | IM_RD | CVG_DST_CLAMP | ZMODE_DEC | CVG_X_ALPHA | ALPHA_CVG_SEL | FORCE_BL | GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA));
		gDPSetTextureFilter(POLY_XLU_DISP++, G_TF_BILERP);
		gDPExtras(POLY_XLU_DISP++, 0, GX_POLYGONOFFSET);
		gDPSetEnvColor(POLY_XLU_DISP++, 0xFF, 0xFF, 0xFF, 0x80);
		gDPSetCombineLERP(POLY_XLU_DISP++, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED);
		gSPGeometryMode(POLY_XLU_DISP++, G_CULL_FRONT | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR, G_ZBUFFER | G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH);
		
		// draw
		gSPSegment(POLY_XLU_DISP++, 6, (void*)vBuf);
		gSPDisplayList(POLY_XLU_DISP++, tri);
		
		// wireframe
		gDPPipeSync(POLY_XLU_DISP++);
		gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 255);
		gDPExtras(POLY_XLU_DISP++, 0, GX_WIREFRAME);
		gSPDisplayList(POLY_XLU_DISP++, tri);
		
		// reset drawing state
		gDPExtras(POLY_XLU_DISP++, GX_POLYGONOFFSET | GX_WIREFRAME, 0);
		
		/*gSPSegment(POLY_XLU_DISP++, 6, (void*)baked);
		Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
		gSPMatrix(POLY_XLU_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
		gSPDisplayList(POLY_XLU_DISP++, baked + bakedOfs);*/
	} Matrix_Pop();
#endif
}

static void Room_BuildTriBuf(void* userData, const n64_triangleCallbackData* triData) {
	Room* this = userData;
	Triangle* tri = &this->triBuf.head[this->triBuf.num];
	
	memcpy(tri->v, triData, sizeof(float) * 3 * 3);
	tri->cullBackface = triData->cullBackface;
	tri->cullFrontface = triData->cullFrontface;
	this->triBuf.num++;
	
	if (this->triBuf.num == this->triBuf.max)
		TriBuffer_Realloc(&this->triBuf);
}

void Scene_CacheBuild(Scene* this) {
	MtxF mtx;
	
	Matrix_Push();
	Matrix_Scale(1.0, 1.0, 1.0, MTXMODE_NEW);
	Matrix_Get(&mtx);
	Matrix_Pop();
	
	n64_setMatrix_model(&mtx);
	n64_setMatrix_view(&mtx);
	n64_setMatrix_projection(&mtx);
	n64_set_culling(false);
	
	for (s32 i = 0; i < this->numRoom; i++) {
		Room* room = this->room[i];
		TriBuffer_Alloc(&room->triBuf, 256);
		
		n64_graph_init();
		gSegment[2] = this->segment;
		gSPSegment(POLY_OPA_DISP++, 0x02, this->segment);
		gSPSegment(POLY_XLU_DISP++, 0x02, this->segment);
		
		Log("Room %d", i);
		Room_Draw(this, room);
		
		n64_set_triangleCallbackFunc(room, Room_BuildTriBuf);
		n64_draw_buffers();
		
		printf_info("Room%d TriCount: %d", i, room->triBuf.num);
	}
	
	n64_set_triangleCallbackFunc(0, 0);
}

// # # # # # # # # # # # # # # # # # # # #
// # Room                                #
// # # # # # # # # # # # # # # # # # # # #

void Room_Draw(Scene* scene, Room* room) {
	MeshHeader* header = room->mesh;
	
	Assert(header != NULL);
	
	n64_set_onlyZmode(ZMODE_ALL);
	n64_set_onlyGeoLayer(GEOLAYER_ALL);
	
	gSPDisplayList(POLY_OPA_DISP++, gSetupDList(0x19));
	gDPSetEnvColor(POLY_OPA_DISP++, 0x80, 0x80, 0x80, 0x80);
	gSPDisplayList(POLY_XLU_DISP++, gSetupDList(0x19));
	gDPSetEnvColor(POLY_XLU_DISP++, 0x80, 0x80, 0x80, 0x80);
	
	Matrix_Push();
	Matrix_Translate(0, 0, 0, MTXMODE_NEW);
	
	gSegment[3] = room->segment;
	gSPSegment(POLY_OPA_DISP++, 0x03, room->segment);
	gSPSegment(POLY_XLU_DISP++, 0x03, room->segment);
	gSPMatrix(POLY_OPA_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
	gSPMatrix(POLY_XLU_DISP++, NewMtx(), G_MTX_MODELVIEW | G_MTX_LOAD);
	
	Matrix_Pop();
	
	Log("Type: %d", header->base.type);
	if (header->base.type == 0) {
		PolygonType0* polygon = &header->polygon0;
		PolygonDlist* polygonDlist = SEGMENTED_TO_VIRTUAL(polygon->start);
		
		Log("PolyNum %d", polygon->num);
		for (s32 i = 0; i < polygon->num; i++, polygonDlist++) {
			if (polygonDlist->opa != 0)
				gSPDisplayList(POLY_OPA_DISP++, polygonDlist->opa);
			
			if (polygonDlist->xlu != 0)
				gSPDisplayList(POLY_XLU_DISP++, polygonDlist->xlu);
		}
	} else if (header->base.type == 2) {
		PolygonType2* polygon = &header->polygon2;
		PolygonDlist2* polygonDlist = SEGMENTED_TO_VIRTUAL(polygon->start);
		
		Log("PolyNum %d", polygon->num);
		for (s32 i = 0; i < polygon->num; i++, polygonDlist++) {
			if (polygonDlist->opa != 0)
				gSPDisplayList(POLY_OPA_DISP++, polygonDlist->opa);
			
			if (polygonDlist->xlu != 0)
				gSPDisplayList(POLY_XLU_DISP++, polygonDlist->xlu);
		}
	}
}

// # # # # # # # # # # # # # # # # # # # #
// # Commands                            #
// # # # # # # # # # # # # # # # # # # # #

static void Scene_CommandSpawnList(Scene* scene, Room* room, SceneCmd* cmd) {
	// ActorEntry* linkEntry = play->linkActorEntry =
	// 	(ActorEntry*)SEGMENTED_TO_VIRTUAL(cmd->spawnList.segment) + play->setupEntranceList[play->curSpawn].spawn;
	// s16 linkObjectId;
	//
	// play->linkAgeOnLoad = ((void)0, gSaveContext.linkAge);
	//
	// linkObjectId = gLinkObjectIds[((void)0, gSaveContext.linkAge)];
	//
	// gActorOverlayTable[linkEntry->id].initInfo->objectId = linkObjectId;
	// Object_Spawn(&play->objectCtx, linkObjectId);
}

static void Scene_CommandActorList(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->numSetupActors = cmd->actorList.num;
	// play->setupActorList = SEGMENTED_TO_VIRTUAL(cmd->actorList.segment);
}

static void Scene_CommandUnused2(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->unk_11DFC = SEGMENTED_TO_VIRTUAL(cmd->unused02.segment);
}

static void Scene_CommandCollisionHeader(Scene* scene, Room* room, SceneCmd* cmd) {
	CollisionHeader* colHeader = SEGMENTED_TO_VIRTUAL(cmd->colHeader.segment);
	
	scene->colHeader = malloc(sizeof(*colHeader));
	memcpy(scene->colHeader, colHeader, 0x2C);
	colHeader = scene->colHeader;
	colHeader->vtxList = SEGMENTED_TO_VIRTUAL(colHeader->vtxList32);
	colHeader->polyList = SEGMENTED_TO_VIRTUAL(colHeader->polyList32);
	colHeader->surfaceTypeList = SEGMENTED_TO_VIRTUAL(colHeader->surfaceTypeList32);
	// colHeader->cameraDataList = SEGMENTED_TO_VIRTUAL(colHeader->cameraDataList);
	// colHeader->waterBoxes = SEGMENTED_TO_VIRTUAL(colHeader->waterBoxes);
	//
	// BgCheck_Allocate(&play->colCtx, play, colHeader);
}

static void Scene_CommandRoomList(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->numRooms = cmd->roomList.num;
	// play->roomList = SEGMENTED_TO_VIRTUAL(cmd->roomList.segment);
}

static void Scene_CommandEntranceList(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->setupEntranceList = SEGMENTED_TO_VIRTUAL(cmd->entranceList.segment);
}

static void Scene_CommandSpecialFiles(Scene* scene, Room* room, SceneCmd* cmd) {
	// if (cmd->specialFiles.keepObjectId != OBJECT_INVALID) {
	// 	play->objectCtx.subKeepIndex = Object_Spawn(&play->objectCtx, cmd->specialFiles.keepObjectId);
	// 	gSegments[5] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[play->objectCtx.subKeepIndex].segment);
	// }
	//
	// if (cmd->specialFiles.cUpElfMsgNum != 0) {
	// 	play->cUpElfMsgs = Play_LoadFile(play, &sNaviMsgFiles[cmd->specialFiles.cUpElfMsgNum - 1]);
	// }
}

static void Scene_CommandRoomBehavior(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->roomCtx.curRoom.behaviorType1 = cmd->roomBehavior.gpFlag1;
	// play->roomCtx.curRoom.behaviorType2 = cmd->roomBehavior.gpFlag2 & 0xFF;
	// play->roomCtx.curRoom.lensMode = (cmd->roomBehavior.gpFlag2 >> 8) & 1;
	// play->msgCtx.disableWarpSongs = (cmd->roomBehavior.gpFlag2 >> 0xA) & 1;
}

static void Scene_CommandMeshHeader(Scene* scene, Room* room, SceneCmd* cmd) {
	if (room == NULL)
		printf_warning("Scene_CommandMeshHeader: No Room Provided...");
	
	else
		room->mesh = SEGMENTED_TO_VIRTUAL(cmd->mesh.segment);
}

static void Scene_CommandObjectList(Scene* scene, Room* room, SceneCmd* cmd) {
	// s32 i;
	// s32 j;
	// s32 k;
	// ObjectStatus* status;
	// ObjectStatus* status2;
	// ObjectStatus* firstStatus;
	// s16* objectEntry = SEGMENTED_TO_VIRTUAL(cmd->objectList.segment);
	// void* nextPtr;
	//
	// k = 0;
	// i = play->objectCtx.unk_09;
	// firstStatus = &play->objectCtx.status[0];
	// status = &play->objectCtx.status[i];
	//
	// while (i < play->objectCtx.num) {
	// 	if (status->id != *objectEntry) {
	// 		status2 = &play->objectCtx.status[i];
	// 		for (j = i; j < play->objectCtx.num; j++) {
	// 			status2->id = OBJECT_INVALID;
	// 			status2++;
	// 		}
	// 		play->objectCtx.num = i;
	// 		func_80031A28(play, &play->actorCtx);
	//
	// 		continue;
	// 	}
	//
	// 	i++;
	// 	k++;
	// 	objectEntry++;
	// 	status++;
	// }
	//
	// ASSERT(
	// 	cmd->objectList.num <= OBJECT_EXCHANGE_BANK_MAX,
	// 	"scene_info->object_bank.num <= OBJECT_EXCHANGE_BANK_MAX",
	// 	"../z_scene.c",
	// 	705
	// );
	//
	// while (k < cmd->objectList.num) {
	// 	nextPtr = func_800982FC(&play->objectCtx, i, *objectEntry);
	// 	if (i < OBJECT_EXCHANGE_BANK_MAX - 1) {
	// 		firstStatus[i + 1].segment = nextPtr;
	// 	}
	// 	i++;
	// 	k++;
	// 	objectEntry++;
	// }
	//
	// play->objectCtx.num = i;
}

static void Scene_CommandLightList(Scene* scene, Room* room, SceneCmd* cmd) {
	// s32 i;
	// LightInfo* lightInfo = SEGMENTED_TO_VIRTUAL(cmd->lightList.segment);
	//
	// for (i = 0; i < cmd->lightList.num; i++) {
	// 	LightContext_InsertLight(play, &play->lightCtx, lightInfo);
	// 	lightInfo++;
	// }
}

static void Scene_CommandPathList(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->setupPathList = SEGMENTED_TO_VIRTUAL(cmd->pathList.segment);
}

static void Scene_CommandTransitionActorList(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->transiActorCtx.numActors = cmd->transiActorList.num;
	// play->transiActorCtx.list = SEGMENTED_TO_VIRTUAL(cmd->transiActorList.segment);
}

static void Scene_CommandLightSettingsList(Scene* scene, Room* room, SceneCmd* cmd) {
	Editor* editor = GetEditor();
	InterfaceContext* interface = &editor->interface;
	
	scene->numEnv = cmd->lightSettingList.num;
	scene->env = SEGMENTED_TO_VIRTUAL(cmd->lightSettingList.segment);
	Log("Light List: %08X", (u8*)scene->env - (u8*)scene->segment);
	
	if (interface->propEndID)
		PropEnum_Free(interface->propEndID);
	
	interface->propEndID = PropEnum_Init(0);
	for (s32 i = 0; i < editor->scene.numEnv; i++)
		PropEnum_Add(interface->propEndID, xFmt("Env %d", i));
}

static void Scene_CommandSkyboxSettings(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->skyboxId = cmd->skyboxSettings.skyboxId;
	// play->envCtx.skyboxConfig = play->envCtx.changeSkyboxNextConfig = cmd->skyboxSettings.unk_05;
	// play->envCtx.lightMode = cmd->skyboxSettings.unk_06;
	
	scene->indoorLight = cmd->skyboxSettings.unk_06;
}

static void Scene_CommandSkyboxDisables(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->envCtx.skyboxDisabled = cmd->skyboxDisables.unk_04;
// play->envCtx.sunMoonDisabled = cmd->skyboxDisables.unk_05;
}

static void Scene_CommandTimeSettings(Scene* scene, Room* room, SceneCmd* cmd) {
	// if ((cmd->timeSettings.hour != 0xFF) && (cmd->timeSettings.min != 0xFF)) {
	// 	gSaveContext.skyboxTime = gSaveContext.dayTime =
	// 		((cmd->timeSettings.hour + (cmd->timeSettings.min / 60.0f)) * 60.0f) / ((f32)(24 * 60) / 0x10000);
	// }
	//
	// if (cmd->timeSettings.unk_06 != 0xFF) {
	// 	play->envCtx.sceneTimeSpeed = cmd->timeSettings.unk_06;
	// } else {
	// 	play->envCtx.sceneTimeSpeed = 0;
	// }
	//
	// if (gSaveContext.sunsSongState == SUNSSONG_INACTIVE) {
	// 	gTimeSpeed = play->envCtx.sceneTimeSpeed;
	// }
	//
	// play->envCtx.sunPos.x = -(Math_SinS(((void)0, gSaveContext.dayTime) - CLOCK_TIME(12, 0)) * 120.0f) * 25.0f;
	// play->envCtx.sunPos.y = (Math_CosS(((void)0, gSaveContext.dayTime) - CLOCK_TIME(12, 0)) * 120.0f) * 25.0f;
	// play->envCtx.sunPos.z = (Math_CosS(((void)0, gSaveContext.dayTime) - CLOCK_TIME(12, 0)) * 20.0f) * 25.0f;
	//
	// if (((play->envCtx.sceneTimeSpeed == 0) && (gSaveContext.cutsceneIndex < 0xFFF0)) ||
	// 	(gSaveContext.entranceIndex == ENTR_SPOT06_8)) {
	// 	gSaveContext.skyboxTime = ((void)0, gSaveContext.dayTime);
	//
	// 	if ((gSaveContext.skyboxTime > CLOCK_TIME(4, 0)) && (gSaveContext.skyboxTime < CLOCK_TIME(6, 30))) {
	// 		gSaveContext.skyboxTime = CLOCK_TIME(5, 0) + 1;
	// 	} else if ((gSaveContext.skyboxTime >= CLOCK_TIME(6, 30)) && (gSaveContext.skyboxTime <= CLOCK_TIME(8, 0))) {
	// 		gSaveContext.skyboxTime = CLOCK_TIME(8, 0) + 1;
	// 	} else if ((gSaveContext.skyboxTime >= CLOCK_TIME(16, 0)) && (gSaveContext.skyboxTime <= CLOCK_TIME(17, 0))) {
	// 		gSaveContext.skyboxTime = CLOCK_TIME(17, 0) + 1;
	// 	} else if ((gSaveContext.skyboxTime >= CLOCK_TIME(18, 0) + 1) &&
	// 		(gSaveContext.skyboxTime <= CLOCK_TIME(19, 0))) {
	// 		gSaveContext.skyboxTime = CLOCK_TIME(19, 0) + 1;
	// 	}
	// }
}

static void Scene_CommandWindSettings(Scene* scene, Room* room, SceneCmd* cmd) {
	// s8 x = cmd->windSettings.x;
	// s8 y = cmd->windSettings.y;
	// s8 z = cmd->windSettings.z;
	//
	// play->envCtx.windDirection.x = x;
	// play->envCtx.windDirection.y = y;
	// play->envCtx.windDirection.z = z;
	//
	// play->envCtx.windSpeed = cmd->windSettings.unk_07;
}

static void Scene_CommandExitList(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->setupExitList = SEGMENTED_TO_VIRTUAL(cmd->exitList.segment);
}

static void Scene_CommandUndefined9(Scene* scene, Room* room, SceneCmd* cmd) {
}

static void Scene_CommandSoundSettings(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->sequenceCtx.seqId = cmd->soundSettings.seqId;
	// play->sequenceCtx.natureAmbienceId = cmd->soundSettings.natureAmbienceId;
	//
	// if (gSaveContext.seqId == (u8)NA_BGM_DISABLED) {
	// 	Audio_QueueSeqCmd(cmd->soundSettings.specId | 0xF0000000);
	// }
}

static void Scene_CommandEchoSettings(Scene* scene, Room* room, SceneCmd* cmd) {
	// play->roomCtx.curRoom.echo = cmd->echoSettings.echo;
}

static void Scene_CommandCutsceneData(Scene* scene, Room* room, SceneCmd* cmd) {
	// osSyncPrintf("\ngame_play->demo_play.data=[%x]", play->csCtx.segment);
	// play->csCtx.segment = SEGMENTED_TO_VIRTUAL(cmd->cutsceneData.segment);
}

static void Scene_CommandMiscSettings(Scene* scene, Room* room, SceneCmd* cmd) {
	// YREG(15) = cmd->miscSettings.cameraMovement;
	// gSaveContext.worldMapArea = cmd->miscSettings.area;
	//
	// if ((play->sceneNum == SCENE_SHOP1) || (play->sceneNum == SCENE_SYATEKIJYOU)) {
	// 	if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
	// 		gSaveContext.worldMapArea = 1;
	// 	}
	// }
	//
	// if (((play->sceneNum >= SCENE_SPOT00) && (play->sceneNum <= SCENE_GANON_TOU)) ||
	// 	((play->sceneNum >= SCENE_ENTRA) && (play->sceneNum <= SCENE_SHRINE_R))) {
	// 	if (gSaveContext.cutsceneIndex < 0xFFF0) {
	// 		gSaveContext.worldMapAreaData |= gBitFlags[gSaveContext.worldMapArea];
	// 		osSyncPrintf(
	// 			"０００  ａｒｅａ＿ａｒｒｉｖａｌ＝%x (%d)\n",
	// 			gSaveContext.worldMapAreaData,
	// 			gSaveContext.worldMapArea
	// 		);
	// 	}
	// }
}

void (*sSceneCmdHandlers[SCENE_CMD_ID_MAX])(Scene*, Room*, SceneCmd*) = {
	Scene_CommandSpawnList, // SCENE_CMD_ID_SPAWN_LIST
	Scene_CommandActorList, // SCENE_CMD_ID_ACTOR_LIST
	Scene_CommandUnused2,  // SCENE_CMD_ID_UNUSED_2
	Scene_CommandCollisionHeader, // SCENE_CMD_ID_COLLISION_HEADER
	Scene_CommandRoomList, // SCENE_CMD_ID_ROOM_LIST
	Scene_CommandWindSettings, // SCENE_CMD_ID_WIND_SETTINGS
	Scene_CommandEntranceList, // SCENE_CMD_ID_ENTRANCE_LIST
	Scene_CommandSpecialFiles, // SCENE_CMD_ID_SPECIAL_FILES
	Scene_CommandRoomBehavior, // SCENE_CMD_ID_ROOM_BEHAVIOR
	Scene_CommandUndefined9, // SCENE_CMD_ID_UNDEFINED_9
	Scene_CommandMeshHeader, // SCENE_CMD_ID_MESH_HEADER
	Scene_CommandObjectList, // SCENE_CMD_ID_OBJECT_LIST
	Scene_CommandLightList, // SCENE_CMD_ID_LIGHT_LIST
	Scene_CommandPathList, // SCENE_CMD_ID_PATH_LIST
	Scene_CommandTransitionActorList, // SCENE_CMD_ID_TRANSITION_ACTOR_LIST
	Scene_CommandLightSettingsList, // SCENE_CMD_ID_LIGHT_SETTINGS_LIST
	Scene_CommandTimeSettings, // SCENE_CMD_ID_TIME_SETTINGS
	Scene_CommandSkyboxSettings, // SCENE_CMD_ID_SKYBOX_SETTINGS
	Scene_CommandSkyboxDisables, // SCENE_CMD_ID_SKYBOX_DISABLES
	Scene_CommandExitList, // SCENE_CMD_ID_EXIT_LIST
	NULL,                  // SCENE_CMD_ID_END
	Scene_CommandSoundSettings, // SCENE_CMD_ID_SOUND_SETTINGS
	Scene_CommandEchoSettings, // SCENE_CMD_ID_ECHO_SETTINGS
	Scene_CommandCutsceneData, // SCENE_CMD_ID_CUTSCENE_DATA
	NULL, // SCENE_CMD_ID_ALTERNATE_HEADER_LIST
	Scene_CommandMiscSettings, // SCENE_CMD_ID_MISC_SETTINGS
};

char* sSceneCmdHandlers_Name[SCENE_CMD_ID_MAX] = {
	"Scene_CommandSpawnList", // SCENE_CMD_ID_SPAWN_LIST
	"Scene_CommandActorList", // SCENE_CMD_ID_ACTOR_LIST
	"Scene_CommandUnused2", // SCENE_CMD_ID_UNUSED_2
	"Scene_CommandCollisionHeader", // SCENE_CMD_ID_COLLISION_HEADER
	"Scene_CommandRoomList", // SCENE_CMD_ID_ROOM_LIST
	"Scene_CommandWindSettings", // SCENE_CMD_ID_WIND_SETTINGS
	"Scene_CommandEntranceList", // SCENE_CMD_ID_ENTRANCE_LIST
	"Scene_CommandSpecialFiles", // SCENE_CMD_ID_SPECIAL_FILES
	"Scene_CommandRoomBehavior", // SCENE_CMD_ID_ROOM_BEHAVIOR
	"Scene_CommandUndefined9", // SCENE_CMD_ID_UNDEFINED_9
	"Scene_CommandMeshHeader", // SCENE_CMD_ID_MESH_HEADER
	"Scene_CommandObjectList", // SCENE_CMD_ID_OBJECT_LIST
	"Scene_CommandLightList", // SCENE_CMD_ID_LIGHT_LIST
	"Scene_CommandPathList", // SCENE_CMD_ID_PATH_LIST
	"Scene_CommandTransitionActorList", // SCENE_CMD_ID_TRANSITION_ACTOR_LIST
	"Scene_CommandLightSettingsList", // SCENE_CMD_ID_LIGHT_SETTINGS_LIST
	"Scene_CommandTimeSettings", // SCENE_CMD_ID_TIME_SETTINGS
	"Scene_CommandSkyboxSettings", // SCENE_CMD_ID_SKYBOX_SETTINGS
	"Scene_CommandSkyboxDisables", // SCENE_CMD_ID_SKYBOX_DISABLES
	"Scene_CommandExitList", // SCENE_CMD_ID_EXIT_LIST
	"NULL",                // SCENE_CMD_ID_END
	"Scene_CommandSoundSettings", // SCENE_CMD_ID_SOUND_SETTINGS
	"Scene_CommandEchoSettings", // SCENE_CMD_ID_ECHO_SETTINGS
	"Scene_CommandCutsceneData", // SCENE_CMD_ID_CUTSCENE_DATA
	"SetupIndex", // SCENE_CMD_ID_ALTERNATE_HEADER_LIST
	"Scene_CommandMiscSettings", // SCENE_CMD_ID_MISC_SETTINGS
};