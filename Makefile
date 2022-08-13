include setup.mk

CFLAGS          = -Wall -Wno-switch -Wno-unused-function -DEXTLIB=200 -DNDEBUG -I z64viewer/include/ -I src/
CFLAGS_MAIN     = -Wall -Wno-switch -Wno-unused-function -DNDEBUG
OPT_WIN32      := -Ofast
OPT_LINUX      := -Ofast
SOURCE_C        = $(shell find src/* -type f -name '*.c')
SOURCE_C       += $(shell find z64viewer/src/* -type f -name '*.c')
SOURCE_O_LINUX  = $(foreach f,$(SOURCE_C:.c=.o),bin/linux/$f)
SOURCE_O_WIN32  = $(foreach f,$(SOURCE_C:.c=.o),bin/win32/$f)
RELEASE_EXECUTABLE_LINUX := app_linux/z64scene
RELEASE_EXECUTABLE_WIN32 := app_win32/z64scene.exe

ASSETS_IA16    := $(shell find assets/* -type f -name '*.ia16')
ASSETS_RGBA    := $(shell find assets/* -type f -name '*.rgba')
ASSETS_ZOBJ    := $(shell find assets/* -type f -name '*.zobj')
SOURCE_O_LINUX += $(foreach f,$(ASSETS_IA16:.ia16=.o),bin/linux/$f) \
				$(foreach f,$(ASSETS_RGBA:.rgba=.o),bin/linux/$f) \
				$(foreach f,$(ASSETS_ZOBJ:.zobj=.o),bin/linux/$f)
SOURCE_O_WIN32 += $(foreach f,$(ASSETS_IA16:.ia16=.o),bin/win32/$f) \
				$(foreach f,$(ASSETS_RGBA:.rgba=.o),bin/win32/$f) \
				$(foreach f,$(ASSETS_ZOBJ:.zobj=.o),bin/win32/$f)

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
		linux \
		format

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

include $(PATH_EXTLIB)/ExtLib.mk

clean:
	rm -f $(RELEASE_EXECUTABLE_LINUX)
	rm -f $(RELEASE_EXECUTABLE_WIN32)
	rm -f $(SOURCE_O_LINUX)
	rm -f $(SOURCE_O_WIN32)
	rm -f -R bin

format:
	@cd z64viewer && ./format.sh

# # # # # # # # # # # # # # # # # # # #
# LINUX BUILD                         #
# # # # # # # # # # # # # # # # # # # #

bin/linux/z64viewer/%.o: CFLAGS += -Wno-unused-variable -Wno-shift-count-overflow
	
bin/linux/%.o: %.c %.h $(HEADER) $(ExtLib_H) $(ExtGui_H)
bin/linux/%.o: %.c $(HEADER) $(ExtLib_H) $(ExtGui_H)
	@echo "$(PRNT_RSET)[$(PRNT_PRPL)$(notdir $@)$(PRNT_RSET)]"
	@gcc -c -o $@ $< $(OPT_LINUX) $(CFLAGS)
	
bin/linux/%.o: %.ia16 $(DataFileCompiler)
	@echo "$(PRNT_RSET)[$(PRNT_GREN)g$(ASSET_FILENAME)$(PRNT_RSET)]"
	@$(DataFileCompiler) -cc gcc -i $< -o $@
	
bin/linux/%.o: %.rgba $(DataFileCompiler)
	@echo "$(PRNT_RSET)[$(PRNT_GREN)g$(ASSET_FILENAME)$(PRNT_RSET)]"
	@$(DataFileCompiler) -cc gcc -i $< -o $@
	
bin/linux/%.o: %.zobj $(DataFileCompiler)
	@echo "$(PRNT_RSET)[$(PRNT_GREN)g$(ASSET_FILENAME)$(PRNT_RSET)]"
	@$(DataFileCompiler) -cc gcc -i $< -o $@

$(RELEASE_EXECUTABLE_LINUX): $(SOURCE_O_LINUX) $(ExtLib_Linux_O) $(ExtGui_Linux_O) $(ASSETS_O_LINUX)
	@echo "$(PRNT_RSET)[$(PRNT_PRPL)$(notdir $@)$(PRNT_RSET)] [$(PRNT_PRPL)$(notdir $^)$(PRNT_RSET)]"
	@gcc -o $@ $^ -lm -ldl -pthread $(OPT_LINUX) $(CFLAGS_MAIN) $(ExtGui_Linux_Flags)

# # # # # # # # # # # # # # # # # # # #
# WIN32 BUILD                         #
# # # # # # # # # # # # # # # # # # # #

bin/win32/z64viewer/%.o: CFLAGS += -Wno-unused-variable -Wno-shift-count-overflow
	
bin/win32/%.o: %.c %.h $(HEADER) $(ExtLib_H) $(ExtGui_H)
bin/win32/%.o: %.c $(HEADER) $(ExtLib_H) $(ExtGui_H)
	@echo "$(PRNT_RSET)[$(PRNT_PRPL)$(notdir $@)$(PRNT_RSET)]"
	@i686-w64-mingw32.static-gcc -c -o $@ $< $(OPT_WIN32) $(CFLAGS) -D_WIN32
	
bin/win32/%.o: %.ia16 $(DataFileCompiler)
	@echo "$(PRNT_RSET)[$(PRNT_GREN)g$(ASSET_FILENAME)$(PRNT_RSET)]"
	@$(DataFileCompiler) -cc i686-w64-mingw32.static-gcc -i $< -o $@
	
bin/win32/%.o: %.rgba $(DataFileCompiler)
	@echo "$(PRNT_RSET)[$(PRNT_GREN)g$(ASSET_FILENAME)$(PRNT_RSET)]"
	@$(DataFileCompiler) -cc i686-w64-mingw32.static-gcc -i $< -o $@
	
bin/win32/%.o: %.zobj $(DataFileCompiler)
	@echo "$(PRNT_RSET)[$(PRNT_GREN)g$(ASSET_FILENAME)$(PRNT_RSET)]"
	@$(DataFileCompiler) -cc i686-w64-mingw32.static-gcc -i $< -o $@

bin/win32/icon.o: src/icon.rc src/icon.ico
	@i686-w64-mingw32.static-windres -o $@ $<

$(RELEASE_EXECUTABLE_WIN32): bin/win32/icon.o $(SOURCE_O_WIN32) $(ExtLib_Win32_O) $(ExtGui_Win32_O) $(ASSETS_O_WIN32)
	@echo "$(PRNT_RSET)[$(PRNT_PRPL)$(notdir $@)$(PRNT_RSET)] [$(PRNT_PRPL)$(notdir $^)$(PRNT_RSET)]"
	@i686-w64-mingw32.static-gcc -o $@ $^ -lm -pthread $(OPT_WIN32) $(CFLAGS_MAIN) -D_WIN32 $(ExtGui_Win32_Flags)