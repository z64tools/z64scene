.PHONY = z64viewer.exe clean

z64viewer.exe: src/*.c src/*.h z64viewer/src/*.c z64viewer/include/*.h elements/*/*.c elements/*/*.h
	i686-w64-mingw32.static-gcc z64viewer/src/*.c nanovg/src/*.c src/*.c elements/*/*.c -o z64viewer.exe -I include -I z64viewer/include -I nanovg/src -lm `i686-w64-mingw32.static-pkg-config --cflags --libs glfw3` -s -flto -Os -DNDEBUG

clean:
	rm -f z64viewer.exe
