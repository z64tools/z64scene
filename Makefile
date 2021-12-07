.PHONY = z64viewer.exe clean

FLAGS = -s -flto -Os -DNDEBUG

GIT_COMMIT_HASH := $(shell git log -1 --pretty=%h | tr -d '\n')
GIT_COMMIT_MSG := $(shell git log -1 --pretty=%B | tr -d '\n')
FLAGS += -DGIT_COMMIT_HASH="\"$(GIT_COMMIT_HASH)\""
FLAGS += -DGIT_COMMIT_MSG="\"$(GIT_COMMIT_MSG)\""

z64viewer.exe: src/*.c src/*.h z64viewer/src/*.c z64viewer/include/*.h
	i686-w64-mingw32.static-gcc -Wall -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable z64viewer/src/*.c nanovg/src/*.c src/*.c -o z64viewer.exe -I include -I z64viewer/include -I nanovg/src -lm `i686-w64-mingw32.static-pkg-config --cflags --libs glfw3` $(FLAGS)

clean:
	rm -f z64viewer.exe
