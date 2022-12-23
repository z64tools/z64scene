#include "Editor.h"

extern DataFile gFileIcon;

void Main_Install(void) {
    const char* appdata = Sys_ThisAppData();
    
    if (!Sys_Stat(appdata)) {
        MemFile mem = MemFile_Initialize();
        char reg[1024] = {};
        wchar wreg[1024] = {};
        
        Sys_MakeDir("%s/", appdata);
        MemFile_LoadMem(&mem, gFileIcon.data, gFileIcon.size);
        MemFile_SaveFile(&mem, x_fmt("%s/file_icon.ico", appdata));
        
        mem = MemFile_Initialize();
        snprintf(reg, 1024,
            "Windows Registry Editor Version 5.00"      "\x0D\n"
            ""                                          "\x0D\n"
            "[HKEY_CLASSES_ROOT\\.zsp]"                 "\x0D\n"
            "@=\"Zelda Scene Package\""                 "\x0D\n"
            ""                                          "\x0D\n"
            "[HKEY_CLASSES_ROOT\\.zsp\\DefaultIcon]"    "\x0D\n"
            "@=\"%s\\\\z64scene\\\\file_icon.ico\""     "\x0D\n",
            "%AppData%"
        );
        
        StrU16(wreg, reg);
        MemFile_Write(&mem, "\xFF\xFE", 2);
        MemFile_Write(&mem, wreg, strwlen(wreg) * 2);
        
        MemFile_SaveFile(&mem, x_fmt("%s/file_icon.reg", appdata));
        MemFile_Free(&mem);
        
        Terminal_Show();
        printf_info("Setting file association. This will require admin privileges.");
        printf_info("Press enter to continue.");
        Terminal_GetChar();
        
        system(x_fmt("%s/file_icon.reg", appdata));
        Terminal_Hide();
    }
    
    Free(appdata);
}

int UnicodeMain(argc, argv) {
    Editor* editor = Calloc(sizeof(*editor));
    
    Main_Install();
    
    Editor_Init(editor);
    Interface_Main(&editor->app);
    Editor_Destroy(editor);
    
    return 0;
}
