#!/bin/bash

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)

(

    mkdir -p build/

    cd "${SCRIPT_DIR}" || exit

    . find-sdl.sh

    cd ..

    cat > "compile_flags.txt" << EOF
-std=c++17
-I${SDL2_PATH}/include/SDL2
-I${SDL2_IMAGE_PATH}/include/SDL2
-I${SDL2_TTF_PATH}/include/SDL2
EOF

)
