.PHONY = z64scene.exe z64scene default win32 linux clean

FLAGS = -Wall -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable \
				-I z64viewer/include -I nanovg/src \
				-s -Os -DNDEBUG

GIT_COMMIT_HASH := $(shell git log -1 --pretty=%h | tr -d '\n')
GIT_COMMIT_MSG  := $(shell git log -1 --pretty=%B | tr -d '\n')
FLAGS           += -DGIT_COMMIT_HASH="\"$(GIT_COMMIT_HASH)\""
FLAGS           += -DGIT_COMMIT_MSG="\"$(GIT_COMMIT_MSG)\""

SrcC_win32_z64scene  := $(shell find src/* -type f -name '*.c')
SrcO_win32_z64scene  := $(foreach f,$(SrcC_win32_z64scene:.c=.o),bin/win32/$f)
SrcC_win32_z64viewer := $(shell find z64viewer/src/* -type f -name '*.c')
SrcO_win32_z64viewer := $(foreach f,$(SrcC_win32_z64viewer:.c=.o),bin/win32/$f)
SrcC_win32_nanoVG    := $(shell find nanovg/src/* -type f -name '*.c')
SrcO_win32_nanoVG    := $(foreach f,$(SrcC_win32_nanoVG:.c=.o),bin/win32/$f)

SrcC_linux_z64scene  := $(shell find src/* -type f -name '*.c')
SrcO_linux_z64scene  := $(foreach f,$(SrcC_linux_z64scene:.c=.o),bin/linux/$f)
SrcC_linux_z64viewer := $(shell find z64viewer/src/* -type f -name '*.c')
SrcO_linux_z64viewer := $(foreach f,$(SrcC_linux_z64viewer:.c=.o),bin/linux/$f)
SrcC_linux_nanoVG    := $(shell find nanovg/src/* -type f -name '*.c')
SrcO_linux_nanoVG    := $(foreach f,$(SrcC_linux_nanoVG:.c=.o),bin/linux/$f)

# Make build directories
$(shell mkdir -p bin/ $(foreach dir,$(dir $(SrcO_win32_z64scene)) $(dir $(SrcO_win32_z64viewer)) $(dir $(SrcO_win32_nanoVG)) $(dir $(SrcO_linux_z64scene)) $(dir $(SrcO_linux_z64viewer)) $(dir $(SrcO_linux_nanoVG)),$(dir)))

default: win32

win32: src_win32 z64scene.exe
linux: src_linux z64scene

# WIN32

src_win32: $(SrcO_win32_z64scene) $(SrcO_win32_z64viewer) $(SrcO_win32_nanoVG)
	
bin/win32/z64viewer/src/%.o: z64viewer/src/%.c z64viewer/include/%.h
	@echo "Win32: [" $< "]"
	@i686-w64-mingw32.static-gcc $< -c -o $@ $(FLAGS)
	
bin/win32/%.o: %.c %.h
	@echo "Win32: [" $< "]"
	@i686-w64-mingw32.static-gcc $< -c -o $@ $(FLAGS)
	
bin/win32/%.o: %.c
	@echo "Win32: [" $< "]"
	@i686-w64-mingw32.static-gcc $< -c -o $@ $(FLAGS)
	
bin/win32/src/main.o: src/main.c
	@echo "Win32: [" $< "]"
	@i686-w64-mingw32.static-gcc $< -c -o $@ $(FLAGS)

z64scene.exe: $(SrcO_win32_z64scene) $(SrcO_win32_z64viewer) $(SrcO_win32_nanoVG)
	@echo "win32: [" $@ "]"
	@i686-w64-mingw32.static-gcc $^ -o z64scene.exe -lm -flto `i686-w64-mingw32.static-pkg-config --cflags --libs glfw3` $(FLAGS)

# LINUX

src_linux: $(SrcO_linux_z64scene) $(SrcO_linux_z64viewer) $(SrcO_linux_nanoVG)
	
bin/linux/z64viewer/src/%.o: z64viewer/src/%.c z64viewer/include/%.h
	@echo "Linux: [" $< "]"
	@gcc -lm -lglfw -ldl  $< -c -o $@ $(FLAGS)
	
bin/linux/%.o: %.c %.h
	@echo "Linux: [" $< "]"
	@gcc -lm -lglfw -ldl  $< -c -o $@ $(FLAGS)
	
bin/linux/%.o: %.c
	@echo "Linux: [" $< "]"
	@gcc -lm -lglfw -ldl  $< -c -o $@ $(FLAGS)
	
bin/linux/src/main.o: src/main.c
	@echo "Linux: [" $< "]"
	@gcc -lm -lglfw -ldl  $< -c -o $@ $(FLAGS)

z64scene: $(SrcO_linux_z64scene) $(SrcO_linux_z64viewer) $(SrcO_linux_nanoVG)
	@echo "Linux: [" $@ "]"
	@gcc $^ -o z64scene -lm -lglfw -ldl -flto $(FLAGS)

clean:
	rm -f z64scene.exe z64scene
	rm -f $(SrcO_win32_z64scene) $(SrcO_win32_z64viewer) $(SrcO_win32_nanoVG) $(SrcO_linux_z64scene) $(SrcO_linux_z64viewer) $(SrcO_linux_nanoVG)
