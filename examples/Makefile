INCLUDE   := -I../ -I../third_party/miniaudio -I../third_party/olcPixelGameEngine
GCC_FLAGS := -std=c++17 -O2 -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17 -ldl -lm
EM_FLAGS  := -std=c++17 -O2 -s ALLOW_MEMORY_GROWTH=1 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 -s USE_LIBPNG=1

.PHONY: all clean

all: example example.html

example: example.cpp
	g++ example.cpp -o example ${INCLUDE} ${GCC_FLAGS}

example.html: example.cpp
	em++ example.cpp -o example.html ${INCLUDE} ${EM_FLAGS} --preload-file=./assets@assets
clean:
	@rm example example.html example.js example.wasm example.data
