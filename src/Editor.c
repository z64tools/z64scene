#include "Editor.h"

void Editor_DropCallback(GLFWwindow* window, s32 count, char* item[]) {
}

void Editor_Update(EditorContext* editor) {
	GeoGrid_Update(&editor->geoGrid);
	Cursor_Update(&editor->cursor);
}

void Editor_Draw(EditorContext* editor) {
	GeoGrid_Draw(&editor->geoGrid);
}