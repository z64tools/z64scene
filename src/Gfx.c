#include "Gfx.h"
#include "Scene.h"

////////////////////////////////////////////////////////////////////////////////

typedef struct StructBE {
    uint32_t hi;
    uint32_t lo;
} GfxBE;

static int DisplayCmd(GfxBE* gfx);
static void DisplayList(GfxBE* gfx);

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

static RefNode* sRefNodeHead;

static int DisplayCmd(GfxBE* gfx) {
    int cmd = gfx->hi >> (32 - 8);
    int work;
    RefNode* n = NULL;
    
    switch (cmd) {
        case G_NOOP:
        case G_MODIFYVTX:
        case G_CULLDL:
        case G_BRANCH_Z:
        case G_TRI1:
        case G_TRI2:
        case G_QUAD:
        case G_DMA_IO:
        case G_TEXTURE:
        case G_POPMTX:
        case G_GEOMETRYMODE:
        case G_MTX:
        case G_MOVEWORD:
        case G_MOVEMEM:
        case G_LOAD_UCODE:
        case G_SPNOOP:
        case G_RDPHALF_1:
        case G_SETOTHERMODE_L:
        case G_SETOTHERMODE_H:
        case G_SETPTRHI:
        case G_TEXRECT:
        case G_TEXRECTFLIP:
        case G_RDPLOADSYNC:
        case G_RDPPIPESYNC:
        case G_RDPTILESYNC:
        case G_RDPFULLSYNC:
        case G_SETKEYGB:
        case G_SETKEYR:
        case G_SETCONVERT:
        case G_SETSCISSOR:
        case G_SETPRIMDEPTH:
        case G_RDPSETOTHERMODE:
        case G_LOADTLUT:
        case G_RDPHALF_2:
        case G_SETTILESIZE:
        case G_LOADBLOCK:
        case G_LOADTILE:
        case G_FILLRECT:
        case G_SETFILLCOLOR:
        case G_SETFOGCOLOR:
        case G_SETBLENDCOLOR:
        case G_SETPRIMCOLOR:
        case G_SETENVCOLOR:
        case G_SETCOMBINE:
        case G_VTX:
        case G_SETZIMG:
        case G_SETCIMG:
            break;
            
        case G_SETTIMG:
            if ((gfx->lo >> 24) == 2) {
                u32 segment = gfx->lo;
                bool found = false;
                n = sRefNodeHead;
                
                for (RefNode* i = sRefNodeHead; i; i = i->next) {
                    if (i->segment == segment) {
                        found = true;
                        break;
                    }
                }
                
                if (!found) {
                    info("%02X: %08X", cmd, gfx->lo);
                    n = new(RefNode);
                    n->data = SegmentToVirtual(gfx->lo >> 24, gfx->lo & 0xFFFFFF);
                    n->size = 0x4000;
                    n->segment = gfx->lo;
                    
                    Node_Add(sRefNodeHead, n);
                }
            }
            
            break;
            
        case G_DL:
            work = gfx->lo >> 24;
            if (work >= 2 && work <= 3)
                DisplayList(SegmentToVirtual(gfx->lo >> 24, gfx->lo & 0xFFFFFF));
            else
                warn("Display Call to segment %X", work);
            
            break;
            
        case G_ENDDL:
            return 1;
            
        default:
            break;
    }
    
    return 0;
}

static void DisplayList(GfxBE* gfx) {
    while (!DisplayCmd(gfx++));
}

////////////////////////////////////////////////////////////////////////////////

void DisplayList_GatherReferences(Scene* scene) {
    sRefNodeHead = NULL;
    
    for (int mesh = 0; mesh < scene->mesh.num; mesh++) {
        RoomMesh* m = &scene->mesh.entry[mesh];
        
        SegmentSet(2, scene->segment);
        SegmentSet(3, m->segment);
        
        for (int gfx = 0; gfx < m->disp.num; gfx++) {
            if (m->disp.opa[gfx])
                DisplayList(SegmentToVirtual(3, m->disp.opa[gfx] & 0xFFFFFF));
            if (m->disp.xlu[gfx])
                DisplayList(SegmentToVirtual(3, m->disp.xlu[gfx] & 0xFFFFFF));
        }
    }
}
