CFLAGS          = -Wall -Wno-switch -DEXTLIB=155 -DNDEBUG
CFLAGS_MAIN     = -Wall -Wno-switch -DNDEBUG
OPT_WIN32      := -Ofast
OPT_LINUX      := -Ofast
SOURCE_C       := $(shell find src/* -type f -name '*.c')
SOURCE_O_LINUX := $(foreach f,$(SOURCE_C:.c=.o),bin/linux/$f)
SOURCE_O_WIN32 := $(foreach f,$(SOURCE_C:.c=.o),bin/win32/$f)

RELEASE_EXECUTABLE_LINUX := app_linux/z64scene
RELEASE_EXECUTABLE_WIN32 := app_win32/z64scene.exe

PRNT_DGRY := \e[90;2m
PRNT_GRAY := \e[0;90m
PRNT_REDD := \e[0;91m
PRNT_GREN := \e[0;92m
PRNT_YELW := \e[0;93m
PRNT_BLUE := \e[0;94m
PRNT_PRPL := \e[0;95m
PRNT_CYAN := \e[0;96m
PRNT_RSET := \e[m

.PHONY: default \
		win32 \
		linux

default: linux
linux: $(RELEASE_EXECUTABLE_LINUX)
win32: $(RELEASE_EXECUTABLE_WIN32)

$(shell mkdir -p bin/ $(foreach dir, \
	$(dir $(RELEASE_EXECUTABLE_LINUX)) \
	$(dir $(SOURCE_O_LINUX)) \
	\
	$(dir $(RELEASE_EXECUTABLE_WIN32)) \
	$(dir $(SOURCE_O_WIN32)) \
	, $(dir)))

include $(C_INCLUDE_PATH)/ExtLib.mk

clean:
	rm -f $(RELEASE_EXECUTABLE_LINUX)
	rm -f $(RELEASE_EXECUTABLE_WIN32)
	rm -f $(SOURCE_O_LINUX)
	rm -f $(SOURCE_O_WIN32)
	rm -f -R bin

# # # # # # # # # # # # # # # # # # # #
# LINUX BUILD                         #
# # # # # # # # # # # # # # # # # # # #
	
bin/linux/%.o: %.c %.h $(HEADER) $(ExtLib_H)
bin/linux/%.o: %.c $(HEADER) $(ExtLib_H)
	@echo "$(PRNT_RSET)[$(PRNT_PRPL)$(notdir $@)$(PRNT_RSET)]"
	@gcc -c -o $@ $< $(OPT_LINUX) $(CFLAGS)

$(RELEASE_EXECUTABLE_LINUX): $(SOURCE_O_LINUX) $(ExtLib_Linux_O) $(ExtGui_Linux_O)
	@echo "$(PRNT_RSET)[$(PRNT_PRPL)$(notdir $@)$(PRNT_RSET)] [$(PRNT_PRPL)$(notdir $^)$(PRNT_RSET)]"
	@gcc -o $@ $^ -lm -ldl -pthread $(OPT_LINUX) $(CFLAGS_MAIN) $(ExtGui_Linux_Flags)

# # # # # # # # # # # # # # # # # # # #
# WIN32 BUILD                         #
# # # # # # # # # # # # # # # # # # # #
	
bin/win32/%.o: %.c %.h $(HEADER) $(ExtLib_H)
bin/win32/%.o: %.c $(HEADER) $(ExtLib_H)
	@echo "$(PRNT_RSET)[$(PRNT_PRPL)$(notdir $@)$(PRNT_RSET)]"
	@i686-w64-mingw32.static-gcc -c -o $@ $< $(OPT_WIN32) $(CFLAGS) -D_WIN32

bin/win32/icon.o: src/icon.rc src/icon.ico
	@i686-w64-mingw32.static-windres -o $@ $<

$(RELEASE_EXECUTABLE_WIN32): bin/win32/icon.o $(SOURCE_O_WIN32) $(ExtLib_Win32_O) $(ExtGui_Win32_O)
	@echo "$(PRNT_RSET)[$(PRNT_PRPL)$(notdir $@)$(PRNT_RSET)] [$(PRNT_PRPL)$(notdir $^)$(PRNT_RSET)]"
	@i686-w64-mingw32.static-gcc -o $@ $^ -lm -pthread $(OPT_WIN32) $(CFLAGS_MAIN) -D_WIN32 $(ExtGui_Win32_Flags)