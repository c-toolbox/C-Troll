##########################################################################################
#                                                                                        #
# Copyright (c) 2016 - 2020                                                              #
# Alexander Bock, Erik Sundén, Emil Axelsson                                             #
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

cmake_minimum_required (VERSION 3.14 FATAL_ERROR)

project (C-Troll VERSION 0.1)

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

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

include (cmake/compiler_warnings.cmake)
set_project_warnings(project_options)

option(ENABLE_PCH "Enable Precompiled Headers" OFF)
if (ENABLE_PCH)
  target_precompile_headers(project_options INTERFACE <vector> <string> <map> <utility>)
endif ()

if (APPLE)
  set(CMAKE_PREFIX_PATH
    "~/Qt/5.15.1/clang_64/lib/cmake"
    "~/Qt/5.6/clang_64/lib/cmake"
    "~/Qt/5.7/clang_64/lib/cmake"
  )
elseif (WIN32)
  set(CMAKE_PREFIX_PATH
    "C:/Qt/5.12.1/msvc2019_64"
    "C:/Qt/Development/5.12.1/msvc2019_64"
    "C:/Qt/5.12.6/msvc2019_64"
    "C:/Qt/Development/5.12.6/msvc2019_64"
    "C:/Qt/5.15.1/msvc2019_64"
    "C:/Qt/Development/5.15.1/msvc2019_64"
  )
endif ()

add_subdirectory(ext/simplecrypt)
add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(tray)
add_subdirectory(tests)
