##########################################################################################
#                                                                                        #
# Copyright (c) 2016-2026                                                                #
# Alexander Bock                                                                         #
#                                                                                        #
# All rights reserved.                                                                   #
#                                                                                        #
# Redistribution and use in source and binary forms, with or without modification, are   #
# permitted provided that the following conditions are met:                              #
#                                                                                        #
# 1. Redistributions of source code must retain the above copyright notice, this list    #
#    of conditions and the following disclaimer.                                         #
#                                                                                        #
# 2. Redistributions in binary form must reproduce the above copyright notice, this      #
#    list of conditions and the following disclaimer in the documentation and/or other   #
#    materials provided with the distribution.                                           #
#                                                                                        #
# 3. Neither the name of the copyright holder nor the names of its contributors may be   #
#    used to endorse or promote products derived from this software without specific     #
#    prior written permission.                                                           #
#                                                                                        #
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY    #
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES   #
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT    #
# SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,         #
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   #
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR     #
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN       #
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     #
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH    #
# DAMAGE.                                                                                #
#                                                                                        #
##########################################################################################

cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(C-Troll VERSION 3.0.0)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

if (NOT (WIN32 AND MSVC))
  message(FATAL_ERROR "C-Troll only supports Visual Studio on Windows")
endif ()

# Match the dynamic CRT used by the x64-windows vcpkg triplet in both Debug and Release
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

find_package(Qt6 COMPONENTS Core Gui Network Widgets REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(nlohmann_json_schema_validator CONFIG REQUIRED)
find_package(httplib CONFIG REQUIRED)
find_package(Catch2 3 CONFIG REQUIRED)

# Generate compile_commands.json to make it easier to work with clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(
  ENABLE_IPO
  "Enable Interprocedural Optimization, aka Link Time Optimization (LTO)"
  OFF
)
if (ENABLE_IPO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT result OUTPUT output)
  if (result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  else ()
    message(SEND_ERROR "IPO is not supported: ${output}")
  endif ()
endif ()

##########################################################################################
# Qt plugins                                                                             #
##########################################################################################

# vcpkg's applocal deployment only copies the libraries that the executables link against
# and thus misses the Qt plugins, which are loaded at runtime instead. Without at least
# the platform plugin being present next to the executable, no Qt application is able to
# start. The OpenSSL TLS backend is left out on purpose as Schannel is the native backend
# on Windows
set(QtPlugins
  Qt6::QWindowsIntegrationPlugin
  Qt6::QModernWindowsStylePlugin
  Qt6::QSchannelBackendPlugin
  Qt6::QTlsBackendCertOnlyPlugin
)
set(QtPluginFolders
  "platforms"
  "styles"
  "tls"
  "tls"
)

# Has to be called from the same directory in which the target was created
function (copy_qt_plugins target)
  foreach (plugin folder IN ZIP_LISTS QtPlugins QtPluginFolders)
    add_custom_command(
      TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:${target}>/${folder}"
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:${plugin}>"
        "$<TARGET_FILE_DIR:${target}>/${folder}"
      VERBATIM
    )
  endforeach ()
endfunction ()

add_subdirectory(src)
add_subdirectory(tests)


##########################################################################################
# Deployment                                                                             #
##########################################################################################

install(TARGETS C-Troll Editor Starter Tray RUNTIME DESTINATION ".")

# Qt, the Qt plugins, and the third-party libraries are all copied next to the executables
# during the build, so they are picked up from the build output directory
install(
  DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/"
  DESTINATION "."
  FILES_MATCHING
    PATTERN "*.dll"
    PATTERN "Qt6Svg*.dll" EXCLUDE
)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/example" DESTINATION ".")
install(
  FILES
    "${PROJECT_SOURCE_DIR}/CREDITS"
    "${PROJECT_SOURCE_DIR}/LICENSE"
    "${PROJECT_SOURCE_DIR}/README.md"
    "${PROJECT_SOURCE_DIR}/THIRD_PARTY_LICENSE"
  DESTINATION "."
)

set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_NAME "C-Troll")
set(CPACK_PACKAGE_FILE_NAME "C-Troll")
set(CPACK_PACKAGE_DIRECTORY "${PROJECT_SOURCE_DIR}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_VERBATIM_VARIABLES ON)
include(CPack)
