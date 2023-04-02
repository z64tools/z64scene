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
} MenuDebug;

typedef struct {
    ElTextbox   textBox;
    ElContainer actorContainerList;
} MenuActor;

typedef struct {
    int       subIndex;
    MenuDebug menuDebug;
    MenuActor menuActor;
} Settings;

extern SplitTask gSettingsTask;

#endif