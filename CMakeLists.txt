cmake_minimum_required(VERSION 3.15)

set(CMAKE_POLICY_VERSION_MINIMUM 3.15)

project(demo-template VERSION 1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

include(FetchContent)

function(INSTALL_SDL_LIBRARY NAME VERSION REPO_NAME REPO_URL)
    string(TOUPPER "${NAME}" NAME_LIBRARY)

    if(EXISTS "/opt/homebrew/Cellar/${NAME}/${VERSION}/")
        set(${NAME_LIBRARY}_LIBRARY_DIR "/opt/homebrew/Cellar/${NAME}/${VERSION}/lib")
        set(${NAME_LIBRARY}_INCLUDE_DIR "/opt/homebrew/Cellar/${NAME}/${VERSION}/include/SDL2")

        find_library(${NAME_LIBRARY}_LIBRARY NAMES ${NAME_LIBRARY} HINTS ${${NAME_LIBRARY}_LIBRARY_DIR})

        include_directories(${${NAME_LIBRARY}_INCLUDE_DIR})

        message(STATUS "Found ${NAME} ${VERSION} installed via brew ${${NAME_LIBRARY}_LIBRARY_DIR}")
    else()
        message(STATUS "Installing ${NAME} ${VERSION} via FetchContent")

        FetchContent_Declare(
            ${REPO_NAME}
            GIT_REPOSITORY "${REPO_URL}"
            GIT_TAG release-${VERSION}
        )

        set(${NAME_LIBRARY}_LIBRARY ${REPO_NAME}::${REPO_NAME} PARENT_SCOPE)

        FetchContent_MakeAvailable(${NAME_LIBRARY})
    endif()
endfunction()

# SDL2 Flags
set(SDL2_DISABLE_INSTALL ON)

# SDL2_image Flags
set(SDL2IMAGE_AVIF OFF)
set(SDL2IMAGE_BMP OFF)
set(SDL2IMAGE_GIF OFF)
set(SDL2IMAGE_LBM OFF)
set(SDL2IMAGE_PCX OFF)
set(SDL2IMAGE_PNM OFF)
set(SDL2IMAGE_QOI OFF)
set(SDL2IMAGE_TGA OFF)
set(SDL2IMAGE_TIF OFF)
set(SDL2IMAGE_WEBP OFF)
set(SDL2IMAGE_XCF OFF)
set(SDL2IMAGE_XPM OFF)
set(SDL2IMAGE_INSTALL OFF)

# SDL2_ttf Flags
set(SDL2TTF_INSTALL OFF)

# SDL2_mixer Flags
set(SDL2MIXER_OPUS OFF)
set(SDL2MIXER_MIDI_FLUIDSYNTH OFF)
set(SDL2MIXER_WAVPACK OFF)
set(SDL2MIXER_XMP OFF)
set(SDL2MIXER_INSTALL OFF)

INSTALL_SDL_LIBRARY("sdl2" "2.32.10" "SDL2" "https://github.com/libsdl-org/SDL.git")
INSTALL_SDL_LIBRARY("sdl2_image" "2.8.8" "SDL2_image" "https://github.com/libsdl-org/SDL_image.git")
INSTALL_SDL_LIBRARY("sdl2_ttf" "2.24.0" "SDL2_ttf" "https://github.com/libsdl-org/SDL_ttf.git")
INSTALL_SDL_LIBRARY("sdl2_mixer" "2.8.1" "SDL2_mixer" "https://github.com/libsdl-org/SDL_mixer.git")

include_directories("fonts")
include_directories("images")
include_directories("include")

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE INCLUDES "include/*.hpp")

if(WIN32 AND CMAKE_BUILD_TYPE MATCHES "[Rr]elease")
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${INCLUDES})
else()
    add_executable(${PROJECT_NAME} ${SOURCES} ${INCLUDES})
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARY} SDL2main)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARY})
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_IMAGE_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_TTF_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_MIXER_LIBRARY})

if(APPLE AND CMAKE_BUILD_TYPE MATCHES "[Rr]elease")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.handcrankengine.demo"
        MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/resources/Info.plist.in"
    )

    set(MACOSX_BUNDLE_ICON_FILE icon.icns)
    set(APP_ICON_MACOSX ${CMAKE_SOURCE_DIR}/resources/icon.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )

    target_sources(${PROJECT_NAME} PRIVATE ${APP_ICON_MACOSX})

    install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        COMPONENT Runtime
    )

    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CMAKE_PROJECT_VERSION}")
    set(CPACK_DMG_FORMAT "UDZO")
    set(CPACK_SYSTEM_NAME "Darwin")
    set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_BINARY_DIR}")

    set(CPACK_BINARY_STGZ OFF)
    set(CPACK_BINARY_TGZ OFF)
    set(CPACK_BINARY_TZ OFF)

    include(CPack)
endif()
