cmake_minimum_required (VERSION 3.0 FATAL_ERROR)

project (C-Troll)

if (APPLE)
    set(CMAKE_PREFIX_PATH
        "~/Qt/5.6/clang_64/lib/cmake"
        "~/Qt/5.7/clang_64/lib/cmake"
    )
elseif (WIN32)
    set(CMAKE_PREFIX_PATH
        "C:/Qt/5.12.1/msvc2017_64"
    )
endif ()

add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(tray)

message(STATUS "Generating C-Troll project")