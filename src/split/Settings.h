#ifndef SETTINGS_H
#define SETTINGS_H

#include <Editor.h>

typedef struct {
    ElCheckbox buttonIndoor;
    
    ElButton buttonFPS;
    ElButton buttonCulling;
    
    ElButton buttonFog;
    ElButton buttonColView;
    
    ElButton killScene;
    
    ElColor envAmbient;
    ElColor envFogColor;
    ElColor envColA;
    ElColor envColB;
    
    ElSlider fogNear;
    ElSlider fogFar;
    
    ElContainer cont;
} Settings;

extern SplitTask gSettingsTask;

#endif