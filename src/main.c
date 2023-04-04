#include "Editor.h"

extern DataFile gFileIcon;

void Main_Install(Editor* this) {
    const char* appdata = sys_appdata();
    
    this->fpconfig = fmt("%s/config.toml", appdata);
    
    if (sys_stat(this->fpconfig))
        Toml_Load(&this->config, this->fpconfig);
    else
        this->config = Toml_New();
    
    if (!sys_stat(appdata)) {
        Memfile mem = Memfile_New();
        char reg[1024] = {};
        wchar wreg[1024] = {};
        
        sys_mkdir("%s/", appdata);
        Memfile_LoadMem(&mem, gFileIcon.data, gFileIcon.size);
        Memfile_SaveBin(&mem, x_fmt("%s/file_icon.ico", appdata));
        
        mem = Memfile_New();
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
        
        strto16(wreg, reg);
        Memfile_Write(&mem, "\xFF\xFE", 2);
        Memfile_Write(&mem, wreg, strwlen(wreg) * 2);
        
        Memfile_SaveBin(&mem, x_fmt("%s/file_icon.reg", appdata));
        Memfile_Free(&mem);
        
        cli_show();
        info("Setting file association. This will require admin privileges.");
        info("Press enter to continue.");
        cli_getc();
        
        system(x_fmt("%s/file_icon.reg", appdata));
        cli_hide();
    }
}

int uni_main(argc, argv) {
    Editor* editor = new(*editor);
    
    sys_setworkdir(sys_appdir());
    
    Main_Install(editor);
    
    Editor_Init(editor);
    Interface_Main(&editor->app);
    Editor_Destroy(editor);
    
    return 0;
}
