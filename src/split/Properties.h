#ifndef SETTINGS_H
#define SETTINGS_H

#include <Editor.h>
#include "Database.h"

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

typedef enum {
    PE_COMBO,
    PE_TEXT,
} PropEntType;

typedef struct {
    union {
        Element*   element;
        ElCombo*   combo;
        ElTextbox* textBox;
        ElButton*  el;
    };
    
    Arli*       list;
    DbProperty* property;
    int type;
} PropertyEntry;

typedef struct {
    ElCombo     actorEntry;
    
    ElTextbox index;
    ElTextbox variable;
    ElTextbox rotX;
    ElTextbox rotY;
    ElTextbox rotZ;
    ElTextbox posX;
    ElTextbox posY;
    ElTextbox posZ;
    
    ElButton buttonAdd;
    ElButton buttonRem;
    
    PropertyEntry* list;
    u32 num;
    u16 prevIndex;
} MenuActor;

typedef struct {
    int       subIndex;
    MenuDebug menuDebug;
    MenuActor menuActor;
} Properties;

extern SplitTask gPropertiesTask;

#endif