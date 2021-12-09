#ifndef __Z64REGION_H__
#define __Z64REGION_H__
#include <z64.h>
#include <nanovg.h>
#include "Theme.h"

struct RegionContext;
struct Region;
typedef void (* RegionFunc)(
	struct EditorContext*,
	struct Region*
);

typedef enum {
	REG_BAR_TOP,
	REG_BAR_BOT
} RegBar;

typedef enum {
	REG_POINT_NONE = 0,
	REG_POINT_TL   = 1 << 0,
	REG_POINT_TR   = 1 << 1,
	REG_POINT_BL   = 1 << 2,
	REG_POINT_BR   = 1 << 3,
	REG_SIDE_L     = 1 << 4,
	REG_SIDE_R     = 1 << 5,
	REG_SIDE_T     = 1 << 6,
	REG_SIDE_B     = 1 << 7,
} RegionPointFlag;

typedef enum {
	REG_STATE_NONE      = 0,
	
	REG_STATE_SPLIT_TL  = 1 << 0,
	REG_STATE_SPLIT_TR  = 1 << 1,
	REG_STATE_SPLIT_BL  = 1 << 2,
	REG_STATE_SPLIT_BR  = 1 << 3,
	
	REG_STATE_RESIZE_L  = 1 << 4,
	REG_STATE_RESIZE_R  = 1 << 5,
	REG_STATE_RESIZE_T  = 1 << 6,
	REG_STATE_RESIZE_B  = 1 << 7,
	
	REG_STATE_BLOCK_L   = 1 << 8,
	REG_STATE_BLOCK_R   = 1 << 9,
	REG_STATE_BLOCK_HOR = (REG_STATE_BLOCK_L | REG_STATE_BLOCK_R),
	REG_STATE_BLOCK_T   = 1 << 10,
	REG_STATE_BLOCK_B   = 1 << 11,
	REG_STATE_BLOCK_VER = (REG_STATE_BLOCK_T | REG_STATE_BLOCK_B),
	REG_STATE_BLOCK_ALL = (REG_STATE_BLOCK_HOR | REG_STATE_BLOCK_VER),
	REG_STATE_LAST      = 1 << 32,
} RegStateFlag;

typedef struct Region {
	struct Region* prev;
	struct Region* next;
	RegStateFlag   stateFlag;
	Recti rect;
	Vec2i center;
	Vec2i mousePos; // relative
	Vec2i mousePressPos;
	u8    mousePress   : 1;
	u8    mouseInRegion : 1;
	RegionFunc update;
	RegionFunc draw;
	struct {
		struct Region* l;
		struct Region* r;
		struct Region* t;
		struct Region* b;
	} sibling;
} Region;

typedef struct {
	Recti rect;
} StatusBar;

typedef struct RegionContext {
	struct Region* actionRegion;
	struct Region* nodeHead;
	StatusBar bar[2];
	Recti workRegion;
} RegionContext;

void Region_KillRegion(RegionContext* regCtx, Region** reg);
void Region_Init(struct EditorContext* editorCtx);
void Region_Update(struct EditorContext* editorCtx);
void Region_Draw(struct EditorContext* editorCtx);

#define REG_SPLIT_W 4
#define REG_ROUND_R 4

#endif