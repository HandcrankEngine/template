#!/bin/bash

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)

(

    rm -rf build/web

    mkdir -p build/web

    emcc -std=c++17 -Os -o build/web/index.html src/*.cpp -Ifonts -Iimages -Iinclude -Isrc \
        -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png", "svg"]' -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 \
        --shell-file templates/web-minimal.html

    gzip -k build/web/index.wasm

)
