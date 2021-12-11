.PHONY = z64scene.exe z64scene clean sourcefiles

FLAGS = -Wall -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable \
				-I z64viewer/include -I nanovg/src \
				-s -flto -Os #-DNDEBUG

GIT_COMMIT_HASH := $(shell git log -1 --pretty=%h | tr -d '\n')
GIT_COMMIT_MSG := $(shell git log -1 --pretty=%B | tr -d '\n')
FLAGS += -DGIT_COMMIT_HASH="\"$(GIT_COMMIT_HASH)\""
FLAGS += -DGIT_COMMIT_MSG="\"$(GIT_COMMIT_MSG)\""

SrcC_z64scene  := $(shell find src/* -type f -name '*.c')
SrcO_z64scene  := $(foreach f,$(SrcC_z64scene:.c=.o),bin/$f)
SrcC_z64viewer := $(shell find z64viewer/src/* -type f -name '*.c')
SrcO_z64viewer := $(foreach f,$(SrcC_z64viewer:.c=.o),bin/$f)
SrcC_nanoVG    := $(shell find nanovg/src/* -type f -name '*.c')
SrcO_nanoVG    := $(foreach f,$(SrcC_nanoVG:.c=.o),bin/$f)
SrcH_z64scene  := $(shell find src/* -type f -name '*.h')
SrcH_z64viewer := $(shell find z64viewer/src/* -type f -name '*.h')
SrcH_nanoVG    := $(shell find nanovg/src/* -type f -name '*.h')

# Make build directories
$(shell mkdir -p bin/ $(foreach dir,$(dir $(SrcO_z64scene)) $(dir $(SrcO_z64viewer)) $(dir $(SrcO_nanoVG)),$(dir)))

default: sourcefiles z64scene.exe

win32: sourcefiles z64scene.exe
linux: sourcefiles z64scene.exe

sourcefiles: $(SrcO_z64scene) $(SrcO_z64viewer) $(SrcO_nanoVG)

bin/%.o: %.c %.h
	i686-w64-mingw32.static-gcc $< -c -o $@ $(FLAGS)
	
bin/%.o: %.c
	i686-w64-mingw32.static-gcc $< -c -o $@ $(FLAGS)

bin/src/main.o: src/main.c
	i686-w64-mingw32.static-gcc $< -c -o $@ $(FLAGS)

z64scene.exe: $(SrcO_z64scene) $(SrcO_z64viewer) $(SrcO_nanoVG)
	i686-w64-mingw32.static-gcc $^ -o z64scene.exe -lm `i686-w64-mingw32.static-pkg-config --cflags --libs glfw3` $(FLAGS)

z64scene: $(SrcO_z64scene) $(SrcO_z64viewer) $(SrcO_nanoVG)
	gcc -lm -lglfw -ldl $(FLAGS)

clean:
	rm -f z64scene.exe z64scene
	rm -f $(SrcO_z64scene)
	rm -f $(SrcO_z64viewer)
	rm -f $(SrcO_nanoVG)
