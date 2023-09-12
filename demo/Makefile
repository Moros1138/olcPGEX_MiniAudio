INCLUDE   := -I../ -I../third_party/miniaudio -I../third_party/olcPixelGameEngine
GCC_FLAGS := -std=c++17 -O2 -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17 -ldl -lm
EM_FLAGS  := -std=c++17 -O2 -s ALLOW_MEMORY_GROWTH=1 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 -s USE_LIBPNG=1

.PHONY: all linux emscripten clean

all: linux emscripten

demo: demo.cpp
	g++ demo.cpp -o demo ${INCLUDE} ${GCC_FLAGS}

demo.html: demo.cpp
	em++ demo.cpp -o demo.html ${INCLUDE} ${EM_FLAGS} --preload-file=./assets@assets

linux: demo

emscripten: demo.html

clean: 
	@rm -f demo demo.html demo.js demo.wasm demo.data
