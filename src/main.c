#include <z64viewer.h>
#include <nanovg.h>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

static NVGcontext* vg = NULL;
static int gFont = -1;

typedef struct drawParams
{
	MemFile zScene;
	MemFile zRoom;
} DrawParams;

static void gui(void *udata)
{
	int windowWidth;
	int windowHeight;
	float pxRatio;
	
	DrawParams *params = udata;
	
	z64viewer_get_windowDimensions(&windowWidth, &windowHeight);
	pxRatio = (float)windowWidth / (float)windowHeight;
	nvgBeginFrame(vg, windowWidth, windowHeight, pxRatio);
	
	nvgBeginPath(vg);
	nvgRect(vg, 100,100, 150,30);
	nvgFillColor(vg, nvgRGBA(255,192,0,255));
	nvgFill(vg);
	if (gFont != -1)
	{
		nvgFillColor(vg, nvgRGBA(0,0,0,220));
		nvgFontSize(vg, 15.0f);
		nvgFontFace(vg, "sans");
		nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
		nvgTextBox(vg, 100,100, 150, "wow, hermosauhuts everywhere!", NULL);
	}

	nvgEndFrame(vg);
}

static void draw(void *udata)
{
	DrawParams *params = udata;
	
	z64viewer_scene(params->zScene.data);
	z64viewer_room(params->zRoom.data);
	
	gui(udata);
}

int main(void)
{
	DrawParams params = {0};
	// void* zobj = loadfile("object_link_boy.zobj", 0);
	// void* gameplay_keep = loadfile("gameplay_keep.zobj", 0);
	params.zScene = MemFile_Initialize();
	params.zRoom = MemFile_Initialize();
	
	MemFile_LoadFile(&params.zScene, "scene.zscene");
	MemFile_LoadFile(&params.zRoom, "room_0.zmap");
	
	if (z64viewer_init("z64scene"))
		return 0;
	
#ifdef DEMO_MSAA
	vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
#else
	vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#endif
	if (vg == NULL) {
		printf("Could not init nanovg.\n");
		return -1;
	}
	gFont = nvgCreateFont(vg, "sans", "NotoSans-Regular.ttf");
	
	// render loop
	// -----------
	while (!z64viewer_shouldWindowClose())
	{
		z64viewer_update();
		z64viewer_draw(draw, &params);
	}
	
	z64viewer_terminate();
}

