#!/bin/bash

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)

(

    mkdir -p build/

    cd "${SCRIPT_DIR}" || exit

    . find-sdl.sh

    cd ..

    g++ -std=c++17 -o build/demo-template src/*.cpp -Ifonts -Iimages -Iinclude -Isrc \
        -I"${SDL_INCLUDE_PATH}" -L"${SDL_PATH}/lib" \
        -I"${SDL_IMAGE_INCLUDE_PATH}" -L"${SDL_IMAGE_PATH}/lib" \
        -I"${SDL_TTF_INCLUDE_PATH}" -L"${SDL_TTF_PATH}/lib" \
        -I"${SDL_MIXER_INCLUDE_PATH}" -L"${SDL_MIXER_PATH}/lib" \
        -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer \
        -static

)
