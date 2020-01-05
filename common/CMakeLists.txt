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
# of conditions and the following disclaimer.                                            #
#                                                                                        #
# 2. Redistributions in binary form must reproduce the above copyright notice, this      #
# list of conditions and the following disclaimer in the documentation and/or other      #
# materials provided with the distribution.                                              #
#                                                                                        #
# 3. Neither the name of the copyright holder nor the names of its contributors may be   #
# used to endorse or promote products derived from this software without specific prior  #
# written permission.                                                                    #
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

set(HEADER_FILES
    include/genericmessage.h
    include/jsonload.h
    include/jsonsocket.h
    include/logging.h
    include/traycommand.h
    include/trayprocesslogmessage.h
    include/trayprocessstatus.h
)

set(SOURCE_FILES
    src/genericmessage.cpp
    src/jsonsocket.cpp
    src/logging.cpp
    src/traycommand.cpp
    src/trayprocesslogmessage.cpp
    src/trayprocessstatus.cpp
)

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

set(MOC_FILES "")
qt5_wrap_cpp(
    MOC_FILES
    include/jsonsocket.h
)

set(LIBRARY_NAME C-Troll-Common)

add_library(${LIBRARY_NAME}
    ${SOURCE_FILES}
    ${HEADER_FILES}
    ${MOC_FILES}
    ${RESOURCE_FILES}
)

add_subdirectory(ext/fmt)

target_include_directories(${LIBRARY_NAME} PUBLIC "include" "ext" "ext/fmt/include")
target_link_libraries(${LIBRARY_NAME} fmt Qt5::Widgets Qt5::Network)

set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_STANDARD_REQUIRED On)



if (MSVC)
    target_compile_options(
        ${LIBRARY_NAME}
        PUBLIC
        "/ZI"
        "/MP"
        "/W4"
        "/std:c++latest"
        "/permissive-"
        "/wd4127"           # conditional expression is constant (thrown in Qt)
    )
    target_compile_definitions(
        ${LIBRARY_NAME}
        PUBLIC
        "_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING"
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (APPLE)
        target_compile_options(${LIBRARY_NAME} PUBLIC "-stdlib=libc++")
    endif ()
    target_compile_options(
        ${LIBRARY_NAME}
        PUBLIC
        "-Wall"
        "-Wextra"
        "-Wpedantic"
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(
        ${LIBRARY_NAME}
        PUBLIC
        "-ggdb"
        "-Wall"
        "-Wextra"
        "-Wpedantic"
    )
endif ()
