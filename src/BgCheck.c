#include "BgCheck.h"

void CollisionMesh_Generate(CollisionHeader* colHeader, CollisionMesh* mesh) {
	const u8 simpleXlu[] = { // TODO use Gfx + macros
		0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE2, 0x00, 0x00, 0x1C,
		0x00, 0x50, 0x78, 0x48, 0xE3, 0x00, 0x12, 0x01, 0x00, 0x00, 0x20, 0x00,
		0xFC, 0xFF, 0xFE, 0x04, 0xFF, 0xFE, 0xFB, 0xF8, 0xD9, 0xF0, 0xFD, 0xFE,
		0x00, 0x20, 0x04, 0x04, 0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	Vec3s_BE* vtxList = colHeader->vtxList;
	u16 numPolygons = colHeader->numPolygons;
	s32 vBufCount = 0;
	Gfx* g;
	s32 i;
	s32 k;
	s32 u;
	s32 l = 0;
	
	Log("%d", l++);
	vBufCount = numPolygons * 3;
	mesh->vtxBuf = malloc(sizeof(*mesh->vtxBuf) * vBufCount);
	memset(mesh->vtxBuf, -1, sizeof(*mesh->vtxBuf) * vBufCount);
	
	Log("%d", l++);
	for (i = k = 0; i < numPolygons; ++i) {
		CollisionPoly poly = colHeader->polyList[i];
		for (u = 0; u < 3; ++u, ++k)
			memcpy(&mesh->vtxBuf[k].pos, &vtxList[poly.vtxData[u] & 0x1fff], sizeof(*vtxList));
	}
	
	Log("%d", l++);
	// Allocate triangle list
	// TODO unoptimized; 2 opcodes per triangle for now + a DF at the end
	mesh->tri = malloc(sizeof(*mesh->tri) * (2 * (numPolygons + 1)));
	g = mesh->tri;
	
	Log("%d", l++);
	for (i = 0; i < numPolygons; ++i) {
		u32 vaddr = 0x06000000 | (i * 16 * 3);
		gSPVertex(g++, vaddr, 3, 0);
		gSP1Triangle(g++, 0, 1, 2, 0);
	}
	
	Log("%d", l++);
	gSPEndDisplayList(g++);
	
	// output test zobj
	if (false) {
		FILE* test = fopen("test.zobj", "wb+");
		/*for (i = 0; i < vBufCount; ++i) {
		        fputc(mesh->vtxBuf[i].pos.x >> 8, test);
		        fputc(mesh->vtxBuf[i].pos.x >> 0, test);
		        fputc(mesh->vtxBuf[i].pos.y >> 8, test);
		        fputc(mesh->vtxBuf[i].pos.y >> 0, test);
		        fputc(mesh->vtxBuf[i].pos.z >> 8, test);
		        fputc(mesh->vtxBuf[i].pos.z >> 0, test);
		        for (k = 0; k < 10; ++k)
		                fputc(0, test);
		   }*/
		fwrite(mesh->vtxBuf, 1, sizeof(*mesh->vtxBuf) * vBufCount, test);
		//bakedOfs = ftell(test);
		fwrite(simpleXlu, 1, sizeof(simpleXlu) - 8, test);
		/*for (i = 0; i < sizeof(*mesh->tri) * (2 * (numPolygons + 1)); ++i)
		   {
		        fputc(((uint8_t*)mesh->tri)[i], test);
		   }*/
		fwrite(mesh->tri, 1, sizeof(*mesh->tri) * (2 * (numPolygons + 1)), test);
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
			fprintf(test, "v %d %d %d\n", mesh->vtxBuf[i].pos.x, mesh->vtxBuf[i].pos.y, mesh->vtxBuf[i].pos.z);
		//for (int i = 0; i < numVertices; ++i)
		//	fprintf(test, "v %d %d %d\n", swap16(vtxList[i].x), swap16(vtxList[i].y), swap16(vtxList[i].z));
		for (i = 0; i < numPolygons * 3; i += 3)
			fprintf(test, "f %d %d %d\n", i + 1, i + 2, i + 3);
		fclose(test);
	}
}

void CollisionMesh_Free(CollisionMesh* mesh) {
	Free(mesh->vtxBuf);
	Free(mesh->tri);
	memset(mesh, 0, sizeof(*mesh));
}

void CollisionMesh_Draw(CollisionMesh* mesh) {
	// material
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetRenderMode(POLY_OPA_DISP++, AA_EN | IM_RD | CVG_DST_CLAMP | ZMODE_DEC | CVG_X_ALPHA | ALPHA_CVG_SEL | FORCE_BL | GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA), AA_EN | IM_RD | CVG_DST_CLAMP | ZMODE_DEC | CVG_X_ALPHA | ALPHA_CVG_SEL | FORCE_BL | GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA));
	gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_BILERP);
	gXPMode(POLY_OPA_DISP++, 0, GX_POLYGONOFFSET);
	gDPSetEnvColor(POLY_OPA_DISP++, 0xFF, 0xFF, 0xFF, 0x40);
	gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED);
	gSPGeometryMode(POLY_OPA_DISP++, G_CULL_FRONT | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR, G_ZBUFFER | G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH);
	
	// draw
	gSPSegment(POLY_OPA_DISP++, 6, (void*)mesh->vtxBuf);
	gSPDisplayList(POLY_OPA_DISP++, mesh->tri);
	
	// wireframe
	gDPPipeSync(POLY_OPA_DISP++);
	gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
	gXPMode(POLY_OPA_DISP++, 0, GX_WIREFRAME);
	gSPDisplayList(POLY_OPA_DISP++, mesh->tri);
	
	// reset drawing state
	gXPMode(POLY_OPA_DISP++, GX_POLYGONOFFSET | GX_WIREFRAME, 0);
}
