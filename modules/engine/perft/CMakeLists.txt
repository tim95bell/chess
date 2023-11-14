
cmake_minimum_required(VERSION 3.15)

project(chess_engine_perft VERSION 0.0.0 LANGUAGES CXX)

set(source_files perft.cpp)
set(include_files)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${source_files} ${include_files})

add_executable("${PROJECT_NAME}" ${source_files})

target_link_libraries(
    "${PROJECT_NAME}"
    PUBLIC chess_common chess_engine
)

if(APPLE)
    set_target_properties("${PROJECT_NAME}" PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] YES)
endif()
