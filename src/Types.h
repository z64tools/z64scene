#ifndef __Z64_TYPES_H__
#define __Z64_TYPES_H__

#include <ExtLib.h>

typedef struct StructBE {
	s16 x, y, z;
} Vec3s_BE;

typedef struct StructBE {
	Vec3s_BE pos;
	s16      pad;
	s16      u;
	s16      v;
	union {
		struct StructBE {
			u8 r;
			u8 g;
			u8 b;
			u8 a;
		} color;
		struct StructBE {
			s8 x;
			s8 y;
			s8 z;
			u8 alpha;
		} normal;
	} ext;
} VtxS_BE;

typedef struct DataNode {
	struct DataNode* next;
	void32 segment;
	void*  pointer;
} DataNode;

typedef struct {
	DataNode* head[255];
} DataContext;

#endif