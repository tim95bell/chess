
cmake_minimum_required(VERSION 3.15)

project(chess_app VERSION 0.0.0 LANGUAGES CXX)

set(chess_app_name chess)
set(chess_app_lib_name "${chess_app_name}_lib")
set(chess_hot_reload_name chess_hot_reload)

#region app_lib
set(
    app_lib_source_files
    src/app.cpp
)

set(
    app_lib_include_files
    include/chess/app/app.hpp
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${app_lib_source_files} ${app_lib_include_files})

if(CHESS_HOT_RELOAD)
    add_library(
        "${chess_app_lib_name}"
        SHARED
        ${app_lib_source_files}
    )

    target_link_libraries("${chess_app_lib_name}" PRIVATE chess_common raylib PUBLIC chess_engine)

    target_include_directories(
        "${chess_app_lib_name}"
        PUBLIC
        include
    )

    set_target_properties("${chess_app_lib_name}" PROPERTIES OUTPUT_NAME "${chess_app_name}")
            
    add_custom_target("${chess_hot_reload_name}"
        DEPENDS "${chess_app_lib_name}"
        COMMAND ${CMAKE_COMMAND} -E rename
            "$<TARGET_FILE:${chess_app_lib_name}>"
            "$<TARGET_FILE:${chess_app_lib_name}>.copy")
endif()
#endregion

#region app
set(
    app_source_files
    src/main.cpp
)

set(
    app_header_files
)

add_executable(
    "${chess_app_name}"
    ${app_source_files}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${app_source_files} ${app_header_files})

target_include_directories(
    "${chess_app_name}"
    PUBLIC
    include
)

target_link_libraries("${chess_app_name}" PRIVATE chess_common chess_engine raylib)

if(CHESS_HOT_RELOAD)
    add_dependencies(
        "${chess_app_name}"
        "${chess_hot_reload_name}"
    )
endif()
# endregion

if(APPLE)
    set_target_properties("${chess_app_name}" PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] YES)
endif()
