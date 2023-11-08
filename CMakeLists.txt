
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_C_STANDARD 11)

project(chess VERSION 0.0.0 LANGUAGES CXX)

option(CHESS_UNIT_TEST "Build Unit Tests" TRUE)
option(CHESS_HOT_RELOAD "Enable hot reloading of app code" TRUE)
option(CHESS_RELEASE "Release build" FALSE)

add_compile_definitions(
    "CHESS_DEBUG=$<BOOL:$<CONFIG:Debug>>"
    "CHESS_HOT_RELOAD=$<BOOL:${CHESS_HOT_RELOAD}>")

if(CHESS_UNIT_TEST)
    add_subdirectory(external/Catch2)
endif()

if(CHESS_RELEASE)
    if(CHESS_HOT_RELOAD)
        set(raylib_DIR "./libs/raylib/dynamic/Release")
        add_library(raylib SHARED IMPORTED)
        set_target_properties(raylib PROPERTIES
            IMPORTED_LOCATION "${raylib_DIR}/lib/libraylib.dylib")
    else()
        set(raylib_DIR "./libs/raylib/static/Release")
        add_library(raylib STATIC IMPORTED GLOBAL)
        set_target_properties(raylib PROPERTIES
            IMPORTED_LOCATION "${raylib_DIR}/lib/libraylib.a")
    endif()
else()
    if(CHESS_HOT_RELOAD)
        set(raylib_DIR "./libs/raylib/dynamic/Debug")
        add_library(raylib SHARED IMPORTED)
        set_target_properties(raylib PROPERTIES
            IMPORTED_LOCATION "${raylib_DIR}/lib/libraylib.dylib")
    else()
        set(raylib_DIR "./libs/raylib/static/Debug")
        add_library(raylib STATIC IMPORTED GLOBAL)
        set_target_properties(raylib PROPERTIES
            IMPORTED_LOCATION "${raylib_DIR}/lib/libraylib.a")
    endif()
endif()
target_include_directories(raylib INTERFACE "${raylib_DIR}/include")
target_link_libraries(raylib INTERFACE "-framework IOKit" "-framework Cocoa")

add_subdirectory(modules/common)
add_subdirectory(modules/engine)
add_subdirectory(modules/app)
