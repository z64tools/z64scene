#include "Gfx.h"
#include "Scene.h"

#define qu102_I(x) \
		((x) >> 2)

#define GG_SIZ_BITS(siz) (4 << (uint32_t)(siz))
#define G_SIZ_BYTES(siz) (GG_SIZ_BITS(siz) / 8.0f)

////////////////////////////////////////////////////////////////////////////////

typedef struct StructBE {
	uint32_t hi;
	uint32_t lo;
} GfxBE;

struct GbiTimg;
struct GbiTile;

////////////////////////////////////////////////////////////////////////////////

static DispRef* DispRefList_NewEntry(DispRefList* this) {
	if (this->num >= this->max) {
		this->max = this->max ? (this->max * 2) : 32;
		this->ref = realloc(this->ref, sizeof(DispRef[this->max]));
	}
	
	memset(this->ref + this->num, 0, sizeof(DispRef));
	
	return &this->ref[this->num++];
}

static DispRef* DispRefList_Find(DispRefList* this, off_t segment) {
	DispRef* ref = this->ref;
	DispRef* end = this->ref + this->num;
	
	for (; ref < end; ref++)
		if (segment == ref->segment)
			return ref;
	
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////

typedef struct GbiTimg {
	int fmt;
	int siz;
	u32 width;
	u32 seg;
} GbiTimg;

typedef struct GbiTile {
	int      fmt;
	int      siz;
	int      line;
	uint16_t tmem;
	int      pal;
	int      cms;
	int      cmt;
	int      masks;
	int      maskt;
	int      shifts;
	int      shiftt;
	u16      uls;
	u16      ult;
	u16      lrs;
	u16      lrt;
} GbiTile;

static int GatherDispCmd(DispRefList* list, GfxBE* gfx, u8* history, GbiTimg* gbitimg, GbiTile* gbitile) {
	int cmd = rmask(gfx->hi, 0xFF000000);
	
	switch (cmd) {
		case G_SETTIMG: {
			int bin =        rmask(gfx->hi, 0x00FF0000);
			
			gbitimg->fmt =   rmask(bin, 0b11100000);
			gbitimg->siz =   rmask(bin, 0b00011000);
			gbitimg->width = rmask(gfx->hi, 0x00000FFF);
			gbitimg->seg = gfx->lo;
		} break;
		
		case G_SETTILE: {
			int tile =             rmask(gfx->lo, 0x07000000);
			
			gbitile[tile].fmt =    rmask(gfx->hi, 0b111000000000000000000000);
			gbitile[tile].siz =    rmask(gfx->hi, 0b000110000000000000000000);
			gbitile[tile].line =   rmask(gfx->hi, 0b000000111111111000000000);
			gbitile[tile].tmem =   rmask(gfx->hi, 0b000000000000000111111111);
			gbitile[tile].pal =    rmask(gfx->lo, 0b111100000000000000000000);
			gbitile[tile].cms =    rmask(gfx->lo, 0b000011000000000000000000);
			gbitile[tile].cmt =    rmask(gfx->lo, 0b000000111100000000000000);
			gbitile[tile].masks =  rmask(gfx->lo, 0b000000000011110000000000);
			gbitile[tile].maskt =  rmask(gfx->lo, 0b000000000000001100000000);
			gbitile[tile].shifts = rmask(gfx->lo, 0b000000000000000011110000);
			gbitile[tile].shiftt = rmask(gfx->lo, 0b000000000000000000001111);
		} break;
		
		case G_SETTILESIZE: {
			int tile =          rmask(gfx->lo, 0x07000000);
			
			gbitile[tile].uls = rmask(gfx->hi, 0xFFF000);
			gbitile[tile].ult = rmask(gfx->hi, 0x000FFF);
			gbitile[tile].lrs = rmask(gfx->lo, 0xFFF000);
			gbitile[tile].lrt = rmask(gfx->lo, 0x000FFF);
			
			if (history[0] == G_SETTILE &&
				history[1] == G_RDPPIPESYNC &&
				history[2] == G_LOADBLOCK &&
				history[3] == G_RDPLOADSYNC &&
				history[4] == G_SETTILE &&
				history[5] == G_SETTIMG) {
				int segid = rmask(gbitimg->seg, 0xFF000000);
				int segof = rmask(gbitimg->seg, 0x00FFFFFF);
				
				if (segid == 0x02 && !DispRefList_Find(list, gbitimg->seg)) {
					DispRef* ref = DispRefList_NewEntry(list);
					
					ref->data = SegmentToVirtual(segid, segof);
					ref->segment = gbitimg->seg;
					ref->x = qu102_I(gbitile[tile].lrs) + 1;
					ref->y = qu102_I(gbitile[tile].lrt) + 1;
					ref->size = G_SIZ_BYTES(gbitile[tile].siz) * (ref->x * ref->y);
					ref->fmt = gbitile[tile].fmt;
					ref->bitsiz = gbitile[tile].siz;
				}
			}
		} break;
		
		case G_LOADTLUT: {
			int tile = rmask(gfx->lo, 0x0F000000);
			int count = (rmask(gfx->lo, 0x00FFF000) >> 2) + 1;
			
			if (history[0] == G_RDPLOADSYNC &&
				history[1] == G_SETTILE &&
				history[2] == G_RDPTILESYNC &&
				history[3] == G_SETTIMG &&
				(gbitimg->fmt == G_IM_FMT_RGBA || gbitimg->fmt == G_IM_FMT_IA) &&
				gbitimg->siz == G_IM_SIZ_16b) {
				int segid = rmask(gbitimg->seg, 0xFF000000);
				int segof = rmask(gbitimg->seg, 0x00FFFFFF);
				
				if (segid == 0x02 && !DispRefList_Find(list, gbitimg->seg)) {
					DispRef* ref = DispRefList_NewEntry(list);
					
					ref->data = SegmentToVirtual(segid, segof);
					ref->segment = gbitimg->seg;
					ref->size = G_SIZ_BYTES(G_IM_SIZ_16b) * count;
					ref->palette = count;
					ref->fmt = gbitile[tile].fmt;
					ref->bitsiz = G_IM_SIZ_16b;
				}
			}
		} break;
		
		case G_DL: {
			int seg = rmask(gfx->lo, 0xFF000000);
			int off = rmask(gfx->lo, 0x00FFFFFF);
			
			switch (seg) {
				case 0x03: {
					GfxBE* ngfx = SegmentToVirtual(seg, off);
					
					while (!GatherDispCmd(list, ngfx, history, gbitimg, gbitile)) {
						memmove(&history[1], history, sizeof(u8[31]));
						history[0] = rmask(gfx->hi, 0xFF000000);
						ngfx++;
					}
				} break;
				
				default:
					warn("Display Call to segment 0x%02X", seg);
					break;
			}
			
		} break;
		
		case G_ENDDL:
			return 1;
			break;
			
		default:
			break;
	}
	
	return 0;
}

static int UpdateDispCmd(DispRefList* list, GfxBE* gfx, u8* history, GbiTimg* gbitimg, GbiTile* gbitile) {
	int cmd = rmask(gfx->hi, 0xFF000000);
	DispRef* ref;
	
	switch (cmd) {
		case G_SETTIMG: {
			if ((gfx->lo & 0xFF000000) == 0x02000000)
				if ((ref = DispRefList_Find(list, gfx->lo)))
					gfx->lo = ref->new;
		} break;
		
		case G_DL: {
			int seg = rmask(gfx->lo, 0xFF000000);
			int off = rmask(gfx->lo, 0x00FFFFFF);
			
			switch (seg) {
				case 0x03: {
					GfxBE* ngfx = SegmentToVirtual(seg, off);
					
					while (!UpdateDispCmd(list, ngfx, history, gbitimg, gbitile)) {
						memmove(&history[1], history, sizeof(u8[31]));
						history[0] = rmask(gfx->hi, 0xFF000000);
						ngfx++;
					}
				} break;
				
				default:
					warn("Display Call to segment 0x%02X", seg);
					break;
			}
			
		} break;
		
		case G_ENDDL:
			return 1;
			break;
			
		default:
			break;
	}
	
	return 0;
}

static void DisplayList(DispRefList* list, GfxBE* gfx, int func(DispRefList*, GfxBE*, u8*, GbiTimg*, GbiTile*)) {
	u8 history[32] = {};
	GbiTile tile[8] = {};
	GbiTimg timg = {};
	
	while (!func(list, gfx, history, &timg, tile)) {
		memmove(&history[1], history, sizeof(u8[31]));
		history[0] = rmask(gfx->hi, 0xFF000000);
		gfx++;
	}
}

DispRefList DispRefList_GatherRef(Scene* scene) {
	DispRefList list = {};
	
	for (int mesh = 0; mesh < scene->mesh.num; mesh++) {
		RoomMesh* m = &scene->mesh.entry[mesh];
		off_t addr;
		
		SegmentSet(2, scene->segment);
		SegmentSet(3, m->segment);
		
		for (int gfx = 0; gfx < m->disp.num; gfx++) {
			if (m->disp.opa[gfx]) {
				addr = m->disp.opa[gfx];
				DisplayList(&list, SegmentToVirtual(3, addr & 0xFFFFFF), GatherDispCmd);
			}
			
			if (m->disp.xlu[gfx]) {
				addr = m->disp.xlu[gfx];
				DisplayList(&list, SegmentToVirtual(3, addr & 0xFFFFFF), GatherDispCmd);
			}
		}
	}
	
	return list;
}

////////////////////////////////////////////////////////////////////////////////

void DisplayRefList_UpdateRef(DispRefList* list, void* disp) {
	DisplayList(list, disp, UpdateDispCmd);
}

////////////////////////////////////////////////////////////////////////////////

void DispRefList_Print(DispRefList* list) {
	const char* fmtName[0xFF] = {
		PRNT_BLUE "RGBA",
		PRNT_REDD "YUV",
		PRNT_GREN "CI",
		PRNT_YELW "IA",
		PRNT_PRPL "I"
	};
	const char* sizName[0xFF] = {
		"4b", "8b", "16b", "32b",
	};
	
	for (int i = 0; i < list->num; i++) {
		DispRef* ref = &list->ref[i];
		
		_log("%d / %d: 0x%X", i + 1, list->num, ref->size);
		
		if (ref->palette)
			info("%*d: %08X -> %08X %-12s%-4s 0x%X " PRNT_REDD "PalNum:%d" PRNT_RSET, digint(list->num), i, ref->segment, ref->new, fmtName[ref->fmt], sizName[ref->bitsiz], ref->size, ref->palette);
		else
			info("%*d: %08X -> %08X %-12s%-4s 0x%X %d x %d" PRNT_RSET, digint(list->num), i, ref->segment, ref->new, fmtName[ref->fmt], sizName[ref->bitsiz], ref->size, UnfoldVec2(*ref));
	}
}

void DispRefList_Free(DispRefList* list) {
	delete(list->ref);
}
