# Handcrank Engine Demo Template

[![Made With SDL2](https://img.shields.io/badge/Made_With-SDL2-blue)](https://www.libsdl.org/)

## Dependencies

### macOS

#### CMake

```bash
brew install cmake
brew install SDL2
brew install SDL2_image
brew install SDL2_ttf
brew install SDL2_mixer
```

#### clang-format

```bash
brew install llvm
ln -s "$(brew --prefix llvm)/bin/clang-format" "/usr/local/bin/clang-format"
```

#### clang-tidy

```bash
brew install llvm
ln -s "$(brew --prefix llvm)/bin/clang-tidy" "/usr/local/bin/clang-tidy"
```

### Windows

#### CMake

Via installer <https://cmake.org/download/>

## Build

### CMake (Recommended)

> Note: Builds on Windows require [Microsoft Visual C++ Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170) to run.

#### Debug Build

```bash
mkdir build/
cd build/
cmake ..
cmake --build .
```

#### Release Build

> Note: To generate DMG files on macOS, first build with cmake, then run `cpack`.

```bash
mkdir build/
cd build/
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### g++

```bash
./bin/build.sh
```

## Editor Setup

> Note: Make sure to to run `cmake ..` and `cmake --build .` to generate the `compile_commands.json` and other necessary header files.

### VS Code

Install recommended extensions found in `.vscode/extensions.json`.

### zed

Install plugins recommended by the editor when opening cpp files.

### Sublime Text

Via Package Control, install `LSP` and `LSP-clangd`.
