#!/bin/bash
echo "compile commenced at:" $(date)
  emcc -std=c++1z $1.cpp -s WASM=1 -s USE_SDL=2 -O3 -o $1.js \
  -s EXPORTED_FUNCTIONS="['_main']" \
  -s EXTRA_EXPORTED_RUNTIME_METHODS=["cwrap"] \
  -s ALLOW_MEMORY_GROWTH=1 --use-preload-plugins\
  -lSDL -lsqlite3 \
  -s ASSERTIONS=1

  #FastNoise.cpp