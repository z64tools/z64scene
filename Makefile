ifeq (,$(wildcard settings.mk))
  $(error Please run ./setup.sh to automatically install ExtLib)
endif
include settings.mk

CFLAGS          = -Wall -Wno-switch -Wno-unused-function -DEXTLIB=220 -DNDEBUG -I z64viewer/include/ -I src/
OPT_WIN32      := -O1
OPT_LINUX      := -O1
SOURCE_C        = $(shell find src/* -type f -name '*.c' -not -name 'TEST.c')
SOURCE_C       += $(shell find z64viewer/src/* -type f -name '*.c')
SOURCE_O_LINUX  = $(foreach f,$(SOURCE_C:.c=.o),bin/linux/$f)
SOURCE_O_WIN32  = $(foreach f,$(SOURCE_C:.c=.o),bin/win32/$f)
RELEASE_EXECUTABLE_LINUX := app_linux/z64scene
RELEASE_EXECUTABLE_WIN32 := app_win32/z64scene.exe

ASSETS_IA16    := $(shell find assets/* -type f -name '*.ia16')
ASSETS_RGBA    := $(shell find assets/* -type f -name '*.rgba')
ASSETS_ZOBJ    := $(shell find assets/* -type f -name '*.zobj')
ASSETS_SVG     := $(shell find assets/* -type f -name '*.svg')
SOURCE_O_LINUX += $(foreach f,$(ASSETS_IA16:.ia16=.o),bin/linux/$f) \
				$(foreach f,$(ASSETS_RGBA:.rgba=.o),bin/linux/$f) \
				$(foreach f,$(ASSETS_ZOBJ:.zobj=.o),bin/linux/$f) \
				$(foreach f,$(ASSETS_SVG:.svg=.o),bin/linux/$f)
SOURCE_O_WIN32 += $(foreach f,$(ASSETS_IA16:.ia16=.o),bin/win32/$f) \
				$(foreach f,$(ASSETS_RGBA:.rgba=.o),bin/win32/$f) \
				$(foreach f,$(ASSETS_ZOBJ:.zobj=.o),bin/win32/$f) \
				$(foreach f,$(ASSETS_SVG:.svg=.o),bin/win32/$f)

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

include $(PATH_EXTLIB)/ext_lib.mk

clean:
	rm -f $(RELEASE_EXECUTABLE_LINUX)
	rm -f $(RELEASE_EXECUTABLE_WIN32)
	rm -f $(SOURCE_O_LINUX)
	rm -f $(SOURCE_O_WIN32)
	rm -f -R bin

format-z64viewer:
	@cd z64viewer && ./format.sh
	
# # # # # # # # # # # # # # # # # # # #
# Dependency Include                  #
# # # # # # # # # # # # # # # # # # # #

-include $(SOURCE_O_LINUX:.o=.d)
-include $(SOURCE_O_WIN32:.o=.d)

# # # # # # # # # # # # # # # # # # # #
# LINUX BUILD                         #
# # # # # # # # # # # # # # # # # # # #

bin/linux/z64viewer/%.o: CFLAGS += -Wno-unused-variable -Wno-shift-count-overflow
	
bin/linux/%.o: %.c
	@echo "$(PRNT_RSET)[$(PRNT_PRPL)$(notdir $@)$(PRNT_RSET)]"
	@gcc -c -o $@ $< $(OPT_LINUX) $(CFLAGS)
	$(GD_LINUX)
	
bin/linux/%.o: %.ia16 $(DataFileCompiler)
	@echo "$(PRNT_RSET)[$(PRNT_GREN)g$(ASSET_FILENAME)$(PRNT_RSET)]"
	@$(DataFileCompiler) --cc gcc --i $< --o $@
bin/linux/%.o: %.rgba $(DataFileCompiler)
	@echo "$(PRNT_RSET)[$(PRNT_GREN)g$(ASSET_FILENAME)$(PRNT_RSET)]"
	@$(DataFileCompiler) --cc gcc --i $< --o $@
bin/linux/%.o: %.zobj $(DataFileCompiler)
	@echo "$(PRNT_RSET)[$(PRNT_GREN)g$(ASSET_FILENAME)$(PRNT_RSET)]"
	@$(DataFileCompiler) --cc gcc --i $< --o $@
bin/linux/%.o: %.svg $(DataFileCompiler)
	@echo "$(PRNT_RSET)[$(PRNT_GREN)g$(ASSET_FILENAME)$(PRNT_RSET)]"
	@$(DataFileCompiler) --cc gcc --i $< --o $@

$(RELEASE_EXECUTABLE_LINUX): $(SOURCE_O_LINUX) $(ExtLib_Linux_O) $(ExtGui_Linux_O) $(Zip_Linux_O) $(ASSETS_O_LINUX)
	@echo "$(PRNT_RSET)[$(PRNT_PRPL)$(notdir $@)$(PRNT_RSET)] [$(PRNT_PRPL)$(notdir $^)$(PRNT_RSET)]"
	@gcc -o $@ $^ $(XFLAGS) $(CFLAGS)

# # # # # # # # # # # # # # # # # # # #
# WIN32 BUILD                         #
# # # # # # # # # # # # # # # # # # # #

bin/win32/z64viewer/%.o: CFLAGS += -Wno-unused-variable -Wno-shift-count-overflow
	
bin/win32/%.o: %.c
	@echo "$(PRNT_RSET)[$(PRNT_PRPL)$(notdir $@)$(PRNT_RSET)]"
	@i686-w64-mingw32.static-gcc -c -o $@ $< $(OPT_WIN32) $(CFLAGS) -D_WIN32 -municode
	$(GD_WIN32)
	
bin/win32/%.o: %.ia16 $(DataFileCompiler)
	@echo "$(PRNT_RSET)[$(PRNT_GREN)g$(ASSET_FILENAME)$(PRNT_RSET)]"
	@$(DataFileCompiler) --cc i686-w64-mingw32.static-gcc --i $< --o $@
bin/win32/%.o: %.rgba $(DataFileCompiler)
	@echo "$(PRNT_RSET)[$(PRNT_GREN)g$(ASSET_FILENAME)$(PRNT_RSET)]"
	@$(DataFileCompiler) --cc i686-w64-mingw32.static-gcc --i $< --o $@
bin/win32/%.o: %.zobj $(DataFileCompiler)
	@echo "$(PRNT_RSET)[$(PRNT_GREN)g$(ASSET_FILENAME)$(PRNT_RSET)]"
	@$(DataFileCompiler) --cc i686-w64-mingw32.static-gcc --i $< --o $@
bin/win32/%.o: %.svg $(DataFileCompiler)
	@echo "$(PRNT_RSET)[$(PRNT_GREN)g$(ASSET_FILENAME)$(PRNT_RSET)]"
	@$(DataFileCompiler) --cc i686-w64-mingw32.static-gcc --i $< --o $@

$(RELEASE_EXECUTABLE_WIN32): bin/win32/icon.o $(SOURCE_O_WIN32) $(ExtLib_Win32_O) $(ExtGui_Win32_O) $(Zip_Win32_O) $(ASSETS_O_WIN32)
	@echo "$(PRNT_RSET)[$(PRNT_PRPL)$(notdir $@)$(PRNT_RSET)] [$(PRNT_PRPL)$(notdir $^)$(PRNT_RSET)]"
	@i686-w64-mingw32.static-gcc -o $@ $^ $(XFLAGS) $(CFLAGS) -D_WIN32 -municode