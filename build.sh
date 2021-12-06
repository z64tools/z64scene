mkdir -p bin
gcc z64viewer/src/*.c nanovg/src/*.c src/*.c -o bin/z64scene -I include -I z64viewer/include -I nanovg/src -lm -lglfw -ldl

