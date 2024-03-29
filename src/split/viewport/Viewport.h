#ifndef VIEWPORT_TASK_H
#define VIEWPORT_TASK_H

#include <Editor.h>
#include <Gizmo.h>

typedef struct {
	View3D    view;
	SkelAnime skelAnime;
	Memfile   object;
	bool      holdBlockCamUpdate;

	Vec3f cubePos;

	ElSlider sliderCamFov;
	ElButton buttonResetCam;
	ElButton buttonViewSmooth;

	s32   selMode;
	Vec2f selPos[512];
	s32   selID;
} Viewport;

extern SplitTask gViewportTask;

void Viewport_FocusRoom(Viewport* this, Scene* scene, int id);

#endif
