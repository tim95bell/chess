
cmake_minimum_required(VERSION 3.15)

project(chess_engine VERSION 0.0.0 LANGUAGES CXX)

set(
    include_files
    include/chess/engine/base.hpp
    include/chess/engine/Bitboard.hpp
    include/chess/engine/engine.hpp
)

set(
    source_files
    src/engine.cpp
)

add_library(
    "${PROJECT_NAME}"
    STATIC
    ${source_files}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${source_files} ${include_files})

target_include_directories(
    "${PROJECT_NAME}"
    PUBLIC
    include
)

target_link_libraries(
    "${PROJECT_NAME}"
    PUBLIC chess_common
)

if(APPLE)
    set_target_properties("${PROJECT_NAME}" PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] YES)
endif()

if(CHESS_PERFT)
    add_subdirectory(perft)
endif()

if(CHESS_UNIT_TEST)
    add_subdirectory(test)
endif()
