#!/bin/bash
echo "compile commenced at:" $(date)
  em++ -std=c++1z $1.cpp Game.cpp TextureUtils.cpp FastNoise.cpp WorldUtils.cpp Paths.cpp\
  -s WASM=1 -s USE_SDL=2 -O1 \
  --profiling\
  -s EXPORTED_FUNCTIONS="['_main']" \
  -s EXTRA_EXPORTED_RUNTIME_METHODS=["cwrap"] \
  -s USE_SDL_IMAGE=2\
  -s ALLOW_MEMORY_GROWTH=1 --use-preload-plugins\
  -s SDL2_IMAGE_FORMATS='["png"]'\
  -s USE_SDL_TTF=2\
  -lSDL \
  --preload-file Assets\
  -o $1.js\
  -s ASSERTIONS=1\
  -s LLD_REPORT_UNDEFINED
  
  #-s DISABLE_EXCEPTION_CATCHING=0