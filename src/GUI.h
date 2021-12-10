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
	VTX_BOT_R
} SVtx;

typedef enum {
	DIR_T = 1,
	DIR_B,
	DIR_L,
	DIR_R,
} SplitDir;

typedef enum {
	SPLIT_POINT_NONE = 0,
	SPLIT_POINT_TL   = (1 << 0),
	SPLIT_POINT_TR   = (1 << 1),
	SPLIT_POINT_BL   = (1 << 2),
	SPLIT_POINT_BR   = (1 << 3),
	
	SPLIT_POINTS     = (SPLIT_POINT_TL | SPLIT_POINT_TR | SPLIT_POINT_BL | SPLIT_POINT_BR),
	
	SPLIT_SIDE_T     = (1 << 4),
	SPLIT_SIDE_B     = (1 << 5),
	SPLIT_SIDE_L     = (1 << 6),
	SPLIT_SIDE_R     = (1 << 7),
	
	SPLIT_SIDE_H     = (SPLIT_SIDE_L | SPLIT_SIDE_R),
	SPLIT_SIDE_V     = (SPLIT_SIDE_T | SPLIT_SIDE_B),
	SPLIT_SIDES      = (SPLIT_SIDE_H | SPLIT_SIDE_V),
} SplitPointIndex;

typedef enum {
	SPLIT_STATE_NONE        = 0,
	
	SPLIT_STATE_DRAG_TL     = (1 << 0),
	SPLIT_STATE_DRAG_TR     = (1 << 1),
	SPLIT_STATE_DRAG_BL     = (1 << 2),
	SPLIT_STATE_DRAG_BR     = (1 << 3),
	
	SPLIT_STATE_DRAG_CORNER = (SPLIT_STATE_DRAG_TL | SPLIT_STATE_DRAG_TR | SPLIT_STATE_DRAG_BL | SPLIT_STATE_DRAG_BR),
	
	SPLIT_STATE_DRAG_T      = (1 << 4),
	SPLIT_STATE_DRAG_B      = (1 << 5),
	SPLIT_STATE_DRAG_L      = (1 << 6),
	SPLIT_STATE_DRAG_R      = (1 << 7),
	
	SPLIT_STATE_DRAG_SIDE   = (SPLIT_STATE_DRAG_T | SPLIT_STATE_DRAG_B | SPLIT_STATE_DRAG_L | SPLIT_STATE_DRAG_R),
	
	SPLIT_STATE_SPLIT_T     = (1 << 8),
	SPLIT_STATE_SPLIT_B     = (1 << 9),
	SPLIT_STATE_SPLIT_L     = (1 << 10),
	SPLIT_STATE_SPLIT_R     = (1 << 11),
	
	SPLIT_STATE_SPLIT_V     = (SPLIT_STATE_SPLIT_T | SPLIT_STATE_SPLIT_B),
	SPLIT_STATE_SPLIT_H     = (SPLIT_STATE_SPLIT_L | SPLIT_STATE_SPLIT_R),
} SplitState;

typedef struct {
	Rect rect;
} StatusBar;

typedef enum {
	VTX_STICK_T = (1 << 0),
	VTX_STICK_B = (1 << 1),
	VTX_STICK_L = (1 << 2),
	VTX_STICK_R = (1 << 3),
	
	VTX_STICK   = (VTX_STICK_T | VTX_STICK_B | VTX_STICK_L | VTX_STICK_R),
} VtxState;

typedef struct SplitVtx {
	struct SplitVtx* prev;
	struct SplitVtx* next;
	Vec2d    pos;
	VtxState state;
} SplitVtx;

typedef enum {
	EDGE_STATE_NONE = 0,
	EDGE_HORIZONTAL = (1 << 0),
	EDGE_VERTICAL   = (1 << 1),
	
	EDGE_ALIGN      = (EDGE_HORIZONTAL | EDGE_VERTICAL),
	
	EDGE_HEADER_T   = (1 << 2),
	EDGE_HEADER_B   = (1 << 3),
	EDGE_HEADER_L   = (1 << 4),
	EDGE_HEADER_R   = (1 << 5),
} EdgeState;

typedef struct SplitEdge {
	struct SplitEdge* prev;
	struct SplitEdge* next;
	SplitVtx* vtx[2];
	EdgeState state;
} SplitEdge;

typedef struct Split {
	struct Split* prev;
	struct Split* next;
	SplitState    stateFlag;
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

typedef struct GuiContext {
	Split*     actionSplit;
	Split*     splitHead;
	SplitVtx*  vtxHead;
	SplitEdge* edgeHead;
	StatusBar  bar[2];
	Rect prevWorkRect;
	Rect workRect;
	SplitEdge* resizeEdge;
} GuiContext;

void Gui_Init(struct EditorContext* editorCtx);
void Gui_Update(struct EditorContext* editorCtx);
void Gui_Draw(struct EditorContext* editorCtx);

#define SPLIT_SPLIT_W 4
#define SPLIT_ROUND_R 4

#endif