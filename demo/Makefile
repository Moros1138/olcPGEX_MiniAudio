INCLUDE   := -I../ -I../third_party/miniaudio -I../third_party/olcPixelGameEngine
GCC_FLAGS := -std=c++20 -O2 -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17 -ldl -lm
EM_FLAGS  := -std=c++20 -O2 -s ALLOW_MEMORY_GROWTH=1 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 -s USE_LIBPNG=1

.PHONY: all linux emscripten clean

all: linux emscripten

demo: demo.cpp
	g++ demo.cpp -o demo ${INCLUDE} ${GCC_FLAGS}

demo_synthesis: demo_synthesis.cpp
	g++ demo_synthesis.cpp -o demo_synthesis ${INCLUDE} ${GCC_FLAGS}

demo_waveform: demo_waveform.cpp
	g++ demo_waveform.cpp -o demo_waveform ${INCLUDE} ${GCC_FLAGS}

demo.html: demo.cpp
	em++ demo.cpp -o demo.html ${INCLUDE} ${EM_FLAGS} --preload-file=./assets@assets

demo_synthesis.html: demo_synthesis.cpp
	em++ demo_synthesis.cpp -o demo_synthesis ${INCLUDE} ${EM_FLAGS} --preload-file=./assets@assets

demo_waveform.html: demo.cpp
	em++ demo_waveform.cpp -o demo_waveform.html ${INCLUDE} ${EM_FLAGS} --preload-file=./assets@assets

linux: demo demo_synthesis demo_waveform

emscripten: demo.html demo_synthesis.html demo_waveform.html

clean: 
	@rm -f \
		demo \
		demo.html \
		demo.js \
		demo.wasm \
		demo.data \
		demo_synthesis \
		demo_synthesis.html \
		demo_synthesis.js \
		demo_synthesis.wasm \
		demo_synthesis.data \
		demo_waveform \
		demo_waveform.html \
		demo_waveform.js \
		demo_waveform.wasm \
		demo_waveform.data
