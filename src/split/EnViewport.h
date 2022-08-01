#ifndef __EN_3D_VIEWPORT__
#define __EN_3D_VIEWPORT__

#include <Editor.h>

typedef struct {
	View3D view;
	SkelAnime   skelAnime;
	s8 headerClick;
	
	RayLine rayLine;
	Vec3f   gizmoPos;
} EnViewport;

extern SplitTask gEnViewportTask;

#endif