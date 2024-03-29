ifeq (,$(wildcard settings.mk))
  $(error Please run ./setup.sh to automatically install ExtLib)
endif
include settings.mk

CFLAGS          = -Os -g -Wall -Wno-switch -Wno-unused-function -DEXTLIB=220 -DNDEBUG -I z64viewer/include/ -I src/
SOURCE_C        = $(shell find src/* -type f -name '*.c' -not -name 'TEST.c')
SOURCE_C       += $(shell find z64viewer/src/* -type f -name '*.c')
SOURCE_O_LINUX  = $(foreach f,$(SOURCE_C:.c=.o),bin/linux/$f)
SOURCE_O_WIN32  = $(foreach f,$(SOURCE_C:.c=.o),bin/win32/$f)
RELEASE_EXECUTABLE_LINUX := app_linux/z64scene
RELEASE_EXECUTABLE_WIN32 := app_win32/z64scene.exe

OBJECT_SRC      = $(shell find assets/3D/* -type f -name '*.objex')
OBJECT_ZBJ      = $(foreach f,$(OBJECT_SRC:%.objex=%.zobj),$f)
ASSETS          = $(OBJECT_ZBJ)
ASSETS         += $(shell find assets/* -type f -name '*.zobj')
ASSETS         += $(shell find assets/* -maxdepth 0 -type f -name '*.ia16')
ASSETS         += $(shell find assets/* -maxdepth 0 -type f -name '*.rgba')
ASSETS         += $(shell find assets/* -maxdepth 0 -type f -name '*.svg')
ASSETS         += $(shell find assets/* -maxdepth 0 -type f -name '*.png')
ASSETS         += $(shell find assets/* -maxdepth 0 -type f -name '*.ico')
SOURCE_O_LINUX += $(foreach f,$(ASSETS:%=%.o),bin/linux/$f)
SOURCE_O_WIN32 += $(foreach f,$(ASSETS:%=%.o),bin/win32/$f)

DATABASE_SRC     += $(shell find resources/* -maxdepth 0 -type f -name '*.toml')
DATABASE_O_LINUX  = $(foreach f,$(DATABASE_SRC:resources/%=%), app_linux/resources/$f)
DATABASE_O_WIN32  = $(foreach f,$(DATABASE_SRC:resources/%=%), app_win32/resources/$f)

.PHONY: default \
		win32 \
		linux

default: linux
object: $(OBJECT_ZBJ)
linux: $(OBJECT_ZBJ) $(RELEASE_EXECUTABLE_LINUX) $(DATABASE_O_LINUX)
win32: $(OBJECT_ZBJ) $(RELEASE_EXECUTABLE_WIN32) $(DATABASE_O_WIN32)
data: $(DATABASE_O_WIN32)

$(shell mkdir -p bin/ $(foreach dir, \
	$(dir $(RELEASE_EXECUTABLE_LINUX)) \
	$(dir $(SOURCE_O_LINUX)) \
	$(dir $(DATABASE_O_LINUX)) \
	\
	$(dir $(RELEASE_EXECUTABLE_WIN32)) \
	$(dir $(SOURCE_O_WIN32)) \
	$(dir $(DATABASE_O_WIN32)) \
	, $(dir)))

include $(PATH_EXTLIB)/ext_lib.mk

clean:
	rm -rf bin
	rm -rf app_win32
	rm -rf app_linux

format-z64viewer:
	@cd z64viewer && ./format.sh
	
# # # # # # # # # # # # # # # # # # # #
# Dependency Include                  #
# # # # # # # # # # # # # # # # # # # #

-include $(SOURCE_O_LINUX:.o=.d)
-include $(SOURCE_O_WIN32:.o=.d)

assets/3D/%.zobj: assets/3D/%.objex
	./assets/3D/z64convert-cli --in $< --out $@ --address 0x06000000 --scale 1.0f --macros $(basename $<).h --silent

# # # # # # # # # # # # # # # # # # # #
# LINUX BUILD                         #
# # # # # # # # # # # # # # # # # # # #

app_linux/resources/%: resources/%
	@cp $< $@

bin/linux/z64viewer/%.o: CFLAGS += -Wno-unused-variable -Wno-shift-count-overflow
	
bin/linux/%.o: %.c
	@echo "$(PRNT_RSET)[$(PRNT_PRPL)$(notdir $@)$(PRNT_RSET)]"
	@gcc -c -o $@ $< $(CFLAGS)
	$(GD_LINUX)
	
bin/linux/assets/%.o: assets/% $(DataFileCompiler)
	@echo "$(PRNT_RSET)[$(PRNT_GREN)g$(ASSET_FILENAME)$(PRNT_RSET)]"
	@$(DataFileCompiler) --cc gcc --i $< --o $@

$(RELEASE_EXECUTABLE_LINUX): $(SOURCE_O_LINUX) $(ExtLib_Linux_O) $(NanoGrid_Linux_O) $(ExtGui_Linux_O) $(Zip_Linux_O) $(Texel_Linux_O) $(ASSETS_O_LINUX) $(Image_Linux_O) $(ExtPy_Linux_O)
	@echo "$(PRNT_RSET)[$(PRNT_PRPL)$(notdir $@)$(PRNT_RSET)] [$(PRNT_PRPL)$(notdir $^)$(PRNT_RSET)]"
	@gcc -o $@ $^ $(XFLAGS) $(CFLAGS)

# # # # # # # # # # # # # # # # # # # #
# WIN32 BUILD                         #
# # # # # # # # # # # # # # # # # # # #

app_win32/resources/%: resources/%
	@cp $< $@

bin/win32/z64viewer/%.o: CFLAGS += -Wno-unused-variable -Wno-shift-count-overflow
	
bin/win32/%.o: %.c
	@echo "$(PRNT_RSET)[$(PRNT_PRPL)$(notdir $@)$(PRNT_RSET)]"
	@i686-w64-mingw32.static-gcc -c -o $@ $< $(CFLAGS) -D_WIN32 -municode
	$(GD_WIN32)
	
bin/win32/assets/%.o: assets/% $(DataFileCompiler)
	@echo "$(PRNT_RSET)[$(PRNT_GREN)g$(ASSET_FILENAME)$(PRNT_RSET)]"
	@$(DataFileCompiler) --cc i686-w64-mingw32.static-gcc --i $< --o $@

$(RELEASE_EXECUTABLE_WIN32): bin/win32/icon.o bin/win32/info.o $(SOURCE_O_WIN32) $(NanoGrid_Win32_O) $(ExtLib_Win32_O) $(ExtGui_Win32_O) $(Zip_Win32_O) $(Texel_Win32_O) $(ASSETS_O_WIN32) $(Image_Win32_O) $(ExtPy_Win32_O)
	@echo "$(PRNT_RSET)[$(PRNT_PRPL)$(notdir $@)$(PRNT_RSET)] [$(PRNT_PRPL)$(notdir $^)$(PRNT_RSET)]"
	i686-w64-mingw32.static-gcc -o $@ $^ $(XFLAGS) $(CFLAGS) -D_WIN32 -municode
#	i686-w64-mingw32.static-strip --strip-unneeded $@
