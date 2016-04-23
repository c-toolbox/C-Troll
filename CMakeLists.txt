cmake_minimum_required (VERSION 3.0 FATAL_ERROR)

project (C-Troll)

if (APPLE)
    set(CMAKE_PREFIX_PATH "~/Qt/5.6/clang_64/lib/cmake")
endif ()

add_subdirectory(core)
add_subdirectory(tray)

# message(STATUS "Generating OpenSpace project")

# set(OPENSPACE_BASE_DIR "${PROJECT_SOURCE_DIR}")
# set(OPENSPACE_APPS_DIR "${OPENSPACE_BASE_DIR}/apps")
# set(OPENSPACE_EXT_DIR "${OPENSPACE_BASE_DIR}/ext")
# set(OPENSPACE_MODULE_DIR "${OPENSPACE_BASE_DIR}/modules")
# set(OPENSPACE_CMAKE_EXT_DIR "${OPENSPACE_BASE_DIR}/support/cmake")
# set(GHOUL_BASE_DIR "${OPENSPACE_BASE_DIR}/ext/ghoul")

# include(${OPENSPACE_CMAKE_EXT_DIR}/support_macros.cmake)
# include(${OPENSPACE_CMAKE_EXT_DIR}/module_common.cmake)
# include(${GHOUL_BASE_DIR}/support/cmake/CopySharedLibraries.cmake) 

# test_compiler_compatibility()
# cleanup_project()
# set_build_output_directories()

# option(OPENSPACE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)

# include(src/CMakeLists.txt)
# create_openspace_target()
# add_external_dependencies()
# handle_applications()

# if (MSVC)
#     option(OPENSPACE_ENABLE_VLD "Enable the Visual Leak Detector" OFF)
#     handle_option_vld()
# endif ()

# option(OPENSPACE_HAVE_TESTS "Activate the OpenSpace unit tests" ON)
# handle_option_tests()

# handle_internal_modules()
# copy_dynamic_libraries()
