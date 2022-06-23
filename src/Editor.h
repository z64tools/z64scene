#include <ExtLib.h>
#include <ExtGui/Global.h>

typedef struct EditorContext {
	AppInfo app;
	void*   vg;
	GeoGrid geoGrid;
	CursorContext cursor;
	InputContext  input;
} EditorContext;

void Editor_DropCallback(GLFWwindow* window, s32 count, char* item[]);
void Editor_Update(EditorContext* editor);
void Editor_Draw(EditorContext* editor);

void EnSceneView_Init(void* passArg, void* instance, Split* split);
void EnSceneView_Destroy(void* passArg, void* instance, Split* split);
void EnSceneView_Update(void* passArg, void* instance, Split* split);
void EnSceneView_Draw(void* passArg, void* instance, Split* split);

typedef struct {
	ViewContext view;
} EnSceneView;

void EnRoom_Init(void* passArg, void* instance, Split* split);
void EnRoom_Destroy(void* passArg, void* instance, Split* split);
void EnRoom_Update(void* passArg, void* instance, Split* split);
void EnRoom_Draw(void* passArg, void* instance, Split* split);

typedef struct EnRoom {
	ElSlider   slider;
	ElButton   leButton;
	ElButton   saveLayout;
	ElTextbox  sceneName;
	ElCheckbox checkBox;
} EnRoom;