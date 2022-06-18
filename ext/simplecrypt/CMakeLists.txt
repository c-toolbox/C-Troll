##########################################################################################
#                                                                                        #
# Copyright (c) 2016 - 2020                                                              #
# Alexander Bock, Erik Sunden, Emil Axelsson                                             #
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

find_package(Qt6 COMPONENTS Core REQUIRED)

add_library(simplecrypt simplecrypt.cpp)
target_include_directories(simplecrypt SYSTEM PUBLIC "include")
target_include_directories(simplecrypt SYSTEM PRIVATE "include/simplecrypt")
target_link_libraries(simplecrypt PUBLIC Qt6::Core)

set(MSVC_WARNINGS
  /MP
  /ZI
  /std:c++17
  /permissive-
  /W0
)

set(CLANG_WARNINGS
  -Wno-everything
)

set(GCC_WARNINGS
  -w
)

if (MSVC)
  target_compile_options(simplecrypt PRIVATE ${MSVC_WARNINGS})
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(simplecrypt PRIVATE ${CLANG_WARNINGS})
else ()
  target_compile_options(simplecrypt PRIVATE ${GCC_WARNINGS})
endif ()

qt_finalize_target(simplecrypt)
