
cmake_minimum_required(VERSION 3.15)

project(chess_common VERSION 0.0.0 LANGUAGES CXX)

add_library("${PROJECT_NAME}" INTERFACE)

set(
    include_files
    include/chess/common/assert.hpp
    include/chess/common/number_types.hpp
)

set(
    source_files
)

target_include_directories(
    "${PROJECT_NAME}"
    INTERFACE
    include
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${source_files} ${include_files})
