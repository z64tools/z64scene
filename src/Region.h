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
	REG_HEAD      = 1 << 0,
	REG_ALIGN_L   = 1 << 1,
	REG_ALIGN_R   = 1 << 2,
	REG_ALIGN_TOP = 1 << 3,
	REG_ALIGN_BOT = 1 << 4
} RegionAlignFlag;

typedef enum {
	REG_CORNER_TL = 1 << 0,
	REG_CORNER_TR = 1 << 1,
	REG_CORNER_BL = 1 << 2,
	REG_CORNER_BR = 1 << 3,
	REG_SIDE_T    = 1 << 0,
	REG_SIDE_L    = 1 << 1,
	REG_SIDE_B    = 1 << 2,
	REG_SIDE_R    = 1 << 3,
} RegionPointFlag;

typedef struct Region {
	struct Region*  prev;
	struct Region*  next;
	RegionAlignFlag align;
	Recti rect;
	Vec2i mousePos; // relative
	RegionFunc update;
	RegionFunc draw;
} Region;

typedef struct {
	Recti rect;
} StatusBar;

typedef struct RegionContext {
	struct Region* nodeHead;
	StatusBar bar[2];
	Recti workRegion;
} RegionContext;

void Region_Init(struct EditorContext* editorCtx);
void Region_Update(struct EditorContext* editorCtx);
void Region_Draw(struct EditorContext* editorCtx);

#define REG_SPLIT_W 4
#define REG_ROUND_R 4

#endif