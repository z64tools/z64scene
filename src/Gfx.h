#ifndef DISP_COPY_H
#define DISP_COPY_H
#include <ext_lib.h>

struct Scene;

typedef struct {
	void*  data;
	size_t size;
	off_t  segment;
	off_t  new;
	
	u8  palette;
	u8  fmt;
	u8  bitsiz;
	int x, y;
} DispRef;

typedef struct {
	DispRef* ref;
	int      num;
	int      max;
} DispRefList;

DispRefList DispRefList_GatherRef(struct Scene*);
void DisplayRefList_UpdateRef(DispRefList* list, void* disp);
void DispRefList_Print(DispRefList* list);
void DispRefList_Free(DispRefList* list);

#endif
