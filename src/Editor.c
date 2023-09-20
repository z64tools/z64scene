#include "Editor.h"
#include "Database.h"
#include "Export.h"

extern DataFile gAppIcon16;
extern DataFile gAppIcon32;
extern DataFile gAppIcon64;
extern DataFile gAppIcon128;
extern DataFile gAppIcon256;
extern DataFile gCursor_ArrowUp;
extern DataFile gCursor_ArrowDown;
extern DataFile gCursor_ArrowLeft;
extern DataFile gCursor_ArrowRight;
extern DataFile gCursor_ArrowHorizontal;
extern DataFile gCursor_ArrowVertical;
extern DataFile gCursor_Crosshair;
extern DataFile gCursor_Empty;

Image sTexelFile[5];
const DataFile* sIconData[5] = {
	&gAppIcon16,
	&gAppIcon32,
	&gAppIcon64,
	&gAppIcon128,
	&gAppIcon256,
};
GLFWimage sIconImage[5] = {
	{ 16,  16  },
	{ 32,  32  },
	{ 64,  64  },
	{ 128, 128 },
	{ 256, 256 },
};

////////////////////////////////////////////////////////////////////////////////

void* NewMtx() {
	return Matrix_ToMtx(n64_graph_alloc(sizeof(Mtx)));
}

static Editor* sEditor;

////////////////////////////////////////////////////////////////////////////////

Editor* GetEditor(void) {
	return sEditor;
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

typedef struct {
	ElCombo     fileMenu;
	ElCombo     optionsMenu;
	FileDialog* dialog;
	char mode;
} Header;

static void Header_Init(Header* this, Editor* editor) {
	Element_Combo_SetArli(&this->fileMenu, DatabaseScene_GetList(SCENE_FILE));
	this->fileMenu.align = NVG_ALIGN_CENTER;
	
	Element_Combo_SetArli(&this->optionsMenu, DatabaseScene_GetList(SCENE_OPTIONS));
	this->optionsMenu.align = NVG_ALIGN_CENTER;
	
}

static void Header_Update(void* pass, void* ____null____, Split* split) {
	Editor* editor = pass;
	Header* this = editor->head[0];
	int mode;
	
	this->fileMenu.title = DatabaseScene_GetName(SCENE_FILE);
	this->optionsMenu.title = DatabaseScene_GetName(SCENE_OPTIONS);
	
	Element_Header(&this->fileMenu, 64, &this->optionsMenu, 64);
	
	switch ((mode = Element_Combo(&this->fileMenu))) {
		case 0:
			this->dialog = FileDialog_Open(&editor->nano, 'o', NULL, ".zsp");
			this->mode = mode;
			break;
		case 1:
			this->dialog = FileDialog_Open(&editor->nano, 's', "project.zsp", ".zsp");
			this->mode = mode;
			break;
		case 2:
			this->dialog = FileDialog_Open(&editor->nano, 's', "project.zsp", ".zsp");
			this->mode = mode;
			break;
		case 3:
			this->dialog = FileDialog_Open(&editor->nano, 'm', NULL, ".zscene,.zroom");
			this->mode = mode;
			break;
		case 4:
			this->dialog = FileDialog_Open(&editor->nano, 's', NULL, ".zscene");
			this->mode = mode;
			break;
		case 5:
			this->dialog = FileDialog_Open(&editor->nano, 'o', NULL, "z64project.toml");
			this->mode = mode;
			break;
	}
	
	if (Element_Combo(&this->optionsMenu) > -1) {
	}
	
	if (FileDialog_Poll(this->dialog)) {
		List* output = FileDialog_GetResult(&this->dialog);
		
		if (output) {
			switch (this->mode) {
				case 3:
					Editor_LoadFiles(output->num, output->item);
					break;
					
				case 4: {
					Export* out = Export_New(&editor->scene, output->item[0]);
					
					Export_Scene(out);
					for (int i = 0; i < editor->scene.numRoom; i++)
						Export_Room(out, i);
					
					Export_Free(out);
				}
				
				case 5:
					LoadRomProject(output->item[0]);
					break;
			}
			
			List_Print(output);
			List_Free(output);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

static void Editor_DropCallback(GLFWwindow* __unused__, s32 count, char* item[]) {
	Editor* editor = GetEditor();
	var_t hasRoom = false;
	var_t hasScene = false;
	var_t n = -1;
	List roomList = List_New();
	
	time_start(11);
	
	for (var_t i = 0; i < count; i++) {
		char* file = item[i];
		
		if (striend(file, ".zroom") || striend(file, ".zmap"))
			hasRoom = true;
		
		if (striend(file, ".zscene")) {
			hasScene = true;
			n = i;
		}
	}
	
	if (!hasRoom || !hasScene)
		return;
	
	info("Drop:: " PRNT_YELW "LOADING: " PRNT_BLUE "%s", item[n]);
	if (editor->scene.segment) {
		Gizmo_UnselectAll(&editor->gizmo);
		Actor_UnselectAll(&editor->scene, NULL);
		Scene_Free(&editor->scene);
		Scene_Init(&editor->scene);
	}
	time_start(10);
	Scene_LoadScene(&editor->scene, item[n]);
	info("Drop:: SceneLoad:  " PRNT_REDD "%.2fms", time_get(10) * 1000);
	
	if (editor->scene.segment) {
		List_Alloc(&roomList, count);
		
		for (var_t i = 0; i < count; i++) {
			char* file = item[i];
			
			if (striend(file, ".zroom") || striend(file, ".zmap"))
				List_Add(&roomList, file);
		}
		
		List_Sort(&roomList);
		
		for (int i = 0; i < roomList.num; i++) {
			char* file = roomList.item[i];
			
			time_start(10);
			Scene_LoadRoom(&editor->scene, file);
			info("Drop:: RoomLoad:   " PRNT_REDD "%.2fms" PRNT_RSET " [%s]", time_get(10) * 1000, file);
		}
		
		List_Free(&roomList);
	}
	
	time_start(10);
	
	Scene_CacheBuild(&editor->scene);
	info("Drop:: CacheBuild: " PRNT_REDD "%.2fms", time_get(10) * 1000);
	
	editor->scene.animOoT.index = 0;
	
	for (s32 i = 0; i < count; i++) {
		if (striend(item[i], ".toml")) {
			Toml toml = Toml_New();
			
			Toml_Load(&toml, item[i]);
			editor->scene.animOoT.index = Toml_GetInt(&toml, "draw_func_index");
			Toml_Free(&toml);
		}
	}
	
	if (editor->scene.animOoT.index)
		info("Drop:: SceneAnim: %d", editor->scene.animOoT.index);
	info("Drop:: Total: " PRNT_REDD "%.2fms", time_get(11) * 1000);
}

void Editor_LoadFiles(int num, char* items[]) {
	Editor_DropCallback(NULL, num, items);
}

void Editor_Init(Editor* editor) {
	sEditor = editor;
	
	int x = 980;
	int y = 480;
	
	if (Toml_Var(&editor->config, "z64scene.win_res[0]"))
		x = Toml_GetInt(&editor->config, "z64scene.win_res[0]");
	if (Toml_Var(&editor->config, "z64scene.win_res[1]"))
		y = Toml_GetInt(&editor->config, "z64scene.win_res[1]");
	
#if 1
	Theme_Init(0);
	Undo_Init(512);
	editor->vg =
		Window_Init(
		"z64scene",
		&editor->window, &editor->input,
		editor,
		(void*)Editor_Update, (void*)Editor_Draw,
		Editor_DropCallback,
		x, y, 2);
	
	NanoGrid_Init(&editor->nano, &editor->window, editor);
	NanoGrid_TaskTable(&editor->nano, gTaskTable, ArrCount(gTaskTable));
	glfwSetWindowSizeLimits(editor->window.glfw, 980, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);
	
	Cursor_Init(&editor->cursor, &editor->window);
	Cursor_CreateCursor(CURSOR_ARROW_U, gCursor_ArrowUp.data, 24, 12, 12);
	Cursor_CreateCursor(CURSOR_ARROW_D, gCursor_ArrowDown.data, 24, 12, 12);
	Cursor_CreateCursor(CURSOR_ARROW_L, gCursor_ArrowLeft.data, 24, 12, 12);
	Cursor_CreateCursor(CURSOR_ARROW_R, gCursor_ArrowRight.data, 24, 12, 12);
	Cursor_CreateCursor(CURSOR_ARROW_H, gCursor_ArrowHorizontal.data, 32, 16, 16);
	Cursor_CreateCursor(CURSOR_ARROW_V, gCursor_ArrowVertical.data, 32, 16, 16);
	Cursor_CreateCursor(CURSOR_CROSSHAIR, gCursor_Crosshair.data, 40, 19, 20);
	Cursor_CreateCursor(CURSOR_EMPTY, gCursor_Empty.data, 16, 0, 0);
	
	if (!NanoGrid_LoadLayout(&editor->nano, &editor->config)) {
		Rectf32 size = {
			editor->nano.workRect.x,
			editor->nano.workRect.y,
			editor->nano.workRect.w * 0.70,
			editor->nano.workRect.h
		};
		
		NanoGrid_AddSplit(&editor->nano, &size, TAB_VIEWPORT);
		
		size = (Rectf32) {
			size.w,
			editor->nano.workRect.y,
			editor->nano.workRect.w - size.w,
			editor->nano.workRect.h * 0.35
		};
		
		NanoGrid_AddSplit(&editor->nano, &size, TAB_ROOMLIST);
		
		size = (Rectf32) {
			size.x,
			editor->nano.workRect.y + size.h,
			size.w,
			editor->nano.workRect.h - size.h
		};
		
		NanoGrid_AddSplit(&editor->nano, &size, TAB_SETTINGS);
	}
	
	for (var_t i = 0; i < 5; i++) {
		Image_LoadMem(&sTexelFile[i], sIconData[i]->data, sIconData[i]->size);
		sIconImage[i].pixels = sTexelFile[i].data;
	}
	
	glfwSetWindowIcon(editor->window.glfw, 5, sIconImage);
#endif
	
	Database_Init();
	Scene_Init(&editor->scene);
	Gizmo_Init(&editor->gizmo, &editor->input, editor->vg);
	
	editor->head[0] = new(Header);
	editor->nano.bar[0].headerFunc = Header_Update;
	Header_Init(editor->head[0], editor);
	
#if 0
	char* items[] = {
		"../room_0.zroom",
		// "../room_1.zroom",
		"../scene.zscene",
	};
	
	Editor_DropCallback(editor->app.window, ArrCount(items), items);
	
	Export* exporCtx = Export_New(&editor->scene, "Lake Hylia");
	
	Export_Scene(exporCtx);
	Export_Room(exporCtx, 0);
	Export_Free(exporCtx);
	
	Scene_Free(&editor->scene);
	Database_Free();
	info("" PRNT_BLUE "A L L   O K   A N D   G O O D");
	
	exit(0);
#endif
}

void Editor_Destroy(Editor* editor) {
	Toml_SetVar(&editor->config, "z64scene.win_res[0]", "%d", editor->window.dim.x);
	Toml_SetVar(&editor->config, "z64scene.win_res[1]", "%d", editor->window.dim.y);
	
	NanoGrid_SaveLayout(&editor->nano, &editor->config, editor->fpconfig);
	NanoGrid_Destroy(&editor->nano);
	Cursor_Free(&editor->cursor);
	Undo_Destroy();
	Scene_Free(&editor->scene);
	Gizmo_Free(&editor->gizmo);
	Database_Free();
	
	for (var_t i = 0; i < 5; i++)
		Image_Free(&sTexelFile[i]);
	
	delete(editor->head[0], editor->head[1], editor);
}

f32 gFpsTime;
f32 gTotalTime;

void Editor_Update(Editor* editor) {
	profi_start(0);
	if (editor->scene.kill) {
		Scene_Free(&editor->scene);
		Scene_Init(&editor->scene);
	} else
		Scene_Update(&editor->scene);
	
	Undo_Update(&editor->input);
	profi_start(1); NanoGrid_Update(&editor->nano); profi_stop(1);
	profi_start(2); Cursor_Update(&editor->cursor); profi_stop(2);
	profi_start(3); Gizmo_Update(&editor->gizmo); profi_stop(3);
	
	profi_stop(PROFILER_FPS);
	gFpsTime = profi_get(PROFILER_FPS);
	profi_start(PROFILER_FPS);
}

void Editor_Draw(Editor* editor) {
	profi_start(4); NanoGrid_Draw(&editor->nano); profi_stop(4);
	profi_stop(0);
	gTotalTime = profi_get(0);
}
