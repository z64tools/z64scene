#ifndef __EN_3D_VIEWPORT__
#define __EN_3D_VIEWPORT__

#include <Editor.h>
#include <Gizmo.h>

typedef struct {
    View3D    view;
    SkelAnime skelAnime;
    bool      lockCameraAccess;
    
    Vec3f cubePos;
    
    ElButton resetCam;
    
    s32   selMode;
    Vec2f selPos[512];
    s32   selID;
} Viewport;

extern SplitTask gViewportTask;

#endif