#ifndef SPLIT_TASK_INIT_H
#define SPLIT_TASK_INIT_H

#include "Editor.h"

#include <split/Settings.h>
#include <split/Viewport.h>

static void nothing() {
}

static void draw_nothing(Editor* e, void* instance, Split* split) {
    Element_Header(split, split->taskCombo, 128);
    Element_Combo(split->taskCombo);
}

typedef enum {
    TAB_ROOM,
    TAB_3DVP,
    
    TAB_EMPTY
} SplitEnum;

SplitTask empty = {
    .taskName = "Empty",
    .init     = nothing,
    .destroy  = nothing,
    .update   = nothing,
    .draw     = (void*)draw_nothing,
    .size     = 30
};

SplitTask* gTaskTable[] = {
    [TAB_ROOM] = &gSettingsTask,
    [TAB_3DVP] = &gViewportTask,
    
    [TAB_EMPTY] = &empty
};

#endif