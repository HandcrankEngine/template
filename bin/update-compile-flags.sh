#!/bin/bash

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)

(

    mkdir -p build/

    cd "${SCRIPT_DIR}" || exit

    . find-sdl.sh

    cd ..

    cat > "compile_flags.txt" << EOF
-std=c++17
-I${SDL_INCLUDE_PATH}
-I${SDL_IMAGE_INCLUDE_PATH}
-I${SDL_TTF_INCLUDE_PATH}
-I${SDL_MIXER_INCLUDE_PATH}
EOF

)
