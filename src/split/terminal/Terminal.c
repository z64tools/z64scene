#include "Terminal.h"

void Terminal_Init(Editor*, Terminal*, Split*);
void Terminal_Update(Editor*, Terminal*, Split*);
void Terminal_Draw(Editor*, Terminal*, Split*);

SplitTask gTerminalTask = {
	.taskName = "Terminal",
	.init     = (void*)Terminal_Init,
	.update   = (void*)Terminal_Update,
	.draw     = (void*)Terminal_Draw,
	.size     = sizeof(Terminal)
};

typedef struct {
	char** msg;
} TerminalContext;

static TerminalContext sTerminal;

////////////////////////////////////////////////////////////////////////////////

onlaunch_func_t TerminalContext_Init() {
	sTerminal.msg = new(char*[128]);
	
	char** p = sTerminal.msg;
	char** e = sTerminal.msg + 128;
	
	for (; p < e; p++)
		*p = new(char[128]);
}

onexit_func_t TerminalContext_Dest() {
	char** p = sTerminal.msg;
	char** e = sTerminal.msg + 128;
	
	for (; p < e; p++)
		delete(*p);
	delete(sTerminal.msg);
}

////////////////////////////////////////////////////////////////////////////////

void Console(const char* fmt, ...) {
	va_list va;
	
	va_start(va, fmt);
	
	char** p = sTerminal.msg;
	char** e = sTerminal.msg + 128;
	
	for (; p < e; p++)
		if (!**p)
			xl_vsnprintf(*p, 128, fmt, va);
	
	va_end(va);
}

void ClearConsole() {
	char** p = sTerminal.msg;
	char** e = sTerminal.msg + 128;
	
	for (; p < e; p++)
		**p = '\0';
}

////////////////////////////////////////////////////////////////////////////////

void Terminal_Init(Editor* editor, Terminal* this, Split* split) {
}

void Terminal_Update(Editor* editor, Terminal* this, Split* split) {
	Element_Header(split->taskCombo, 92);
	Element_Combo(split->taskCombo);
}

void Terminal_Draw(Editor* editor, Terminal* this, Split* split) {
	char** p = sTerminal.msg;
	char** e = sTerminal.msg + 128;
	void* vg = editor->vg;
	Rect r = split->rect;
	
	Rect_Scale(r, SPLIT_ELEM_X_PADDING, 0);
	r.h = SPLIT_TEXT_H;
	
	for (; p < e; p++) {
		if (!**p) continue;
		
		Gfx_DrawRounderRect(vg, r, Theme_GetColor(THEME_ELEMENT_LIGHT, 255, 1.0f));
		
		r.y += SPLIT_TEXT_H + SPLIT_ELEM_X_PADDING;
		
	}
}
