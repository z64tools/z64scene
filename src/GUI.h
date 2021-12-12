#ifndef __Z64REGION_H__
#define __Z64REGION_H__
#include <z64.h>
#include <nanovg.h>
#include "Theme.h"

struct GuiContext;
struct Split;
typedef void (* SplitFunc)(
	struct EditorContext*,
	struct Split*
);

typedef enum {
	GUI_BAR_TOP,
	GUI_BAR_BOT
} GuiBarIndex;

typedef enum {
	VTX_BOT_L = 0,
	VTX_TOP_L,
	VTX_TOP_R,
	VTX_BOT_R,
	EDGE_L    = 0,
	EDGE_T,
	EDGE_R,
	EDGE_B
} SplitPos;

typedef enum {
	DIR_L = 0,
	DIR_T,
	DIR_R,
	DIR_B,
} SplitDir;

typedef enum {
	SPLIT_POINT_NONE = 0,
	SPLIT_POINT_BL   = (1 << 0),
	SPLIT_POINT_TL   = (1 << 1),
	SPLIT_POINT_TR   = (1 << 2),
	SPLIT_POINT_BR   = (1 << 3),
	
	SPLIT_POINTS     = (SPLIT_POINT_TL | SPLIT_POINT_TR | SPLIT_POINT_BL | SPLIT_POINT_BR),
	
	SPLIT_SIDE_L     = (1 << 4),
	SPLIT_SIDE_T     = (1 << 5),
	SPLIT_SIDE_R     = (1 << 6),
	SPLIT_SIDE_B     = (1 << 7),
	
	SPLIT_SIDE_H     = (SPLIT_SIDE_L | SPLIT_SIDE_R),
	SPLIT_SIDE_V     = (SPLIT_SIDE_T | SPLIT_SIDE_B),
	SPLIT_SIDES      = (SPLIT_SIDE_H | SPLIT_SIDE_V),
} SplitState;

typedef enum {
	EDGE_STATE_NONE = 0,
	EDGE_HORIZONTAL = (1 << 0),
	EDGE_VERTICAL   = (1 << 1),
	
	EDGE_ALIGN      = (EDGE_HORIZONTAL | EDGE_VERTICAL),
	
	EDGE_STICK_L    = (1 << 2),
	EDGE_STICK_T    = (1 << 3),
	EDGE_STICK_R    = (1 << 4),
	EDGE_STICK_B    = (1 << 5),
	
	EDGE_STICK      = (EDGE_STICK_L | EDGE_STICK_T | EDGE_STICK_R | EDGE_STICK_B),
} EdgeState;

typedef struct SplitVtx {
	struct SplitVtx* prev;
	struct SplitVtx* next;
	Vec2d pos;
} SplitVtx;

typedef struct SplitEdge {
	struct SplitEdge* prev;
	struct SplitEdge* next;
	SplitVtx* vtx[2];
	f64 pos;
	EdgeState state;
	u8 killFlag;
} SplitEdge;

typedef struct Split {
	struct Split* prev;
	struct Split* next;
	SplitState    stateFlag;
	SplitEdge*    edge[4];
	SplitVtx* vtx[4];
	Rect  rect; // Absolute XY, relative WH
	Vec2s center;
	Vec2s mousePos; // relative
	Vec2s mousePressPos;
	u8    mousePress    : 1;
	u8    mouseInRegion : 1;
	SplitFunc update;
	SplitFunc draw;
} Split;

typedef struct {
	Rect rect;
} StatusBar;

typedef struct GuiContext {
	Split*     actionSplit;
	Split*     splitHead;
	SplitVtx*  vtxHead;
	SplitEdge* edgeHead;
	StatusBar  bar[2];
	Rect prevWorkRect;
	Rect workRect;
	SplitEdge* actionEdge;
	struct {
		f64 clampMax;
		f64 clampMin;
	} edgeMovement;
} GuiContext;

void Gui_Init(struct EditorContext* editorCtx);
void Gui_Update(struct EditorContext* editorCtx);
void Gui_Draw(struct EditorContext* editorCtx);

#define SPLIT_SPLIT_W 4
#define SPLIT_ROUND_R 4

#endif