#ifndef DISP_COPY_H
#define DISP_COPY_H
#include <ext_lib.h>

struct Scene;

typedef struct RefNode {
    struct RefNode* next;
    void* data;
    u32   segment;
    u32   new_segment;
    u32   size;
} RefNode;

void DisplayList_GatherReferences(struct Scene* scene);

#endif
