##########################################################################################
#                                                                                        #
# Copyright (c) 2016 - 2019                                                              #
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
    src/application.h
    src/cluster.h
    src/coreprocess.h
    src/guiinitialization.h
    src/guiprocesscommand.h
    src/guistartcommand.h
    src/guiprocessstatus.h
    src/guiprocesslogmessage.h
    src/guiprocesslogmessagehistory.h
    src/program.h
    src/handler/incomingsockethandler.h
    src/handler/outgoingsockethandler.h
)

set(SOURCE_FILES
    src/application.cpp
    src/cluster.cpp
    src/coreprocess.cpp
    src/guiinitialization.cpp
    src/guiprocesscommand.cpp
    src/guistartcommand.cpp
    src/guiprocessstatus.cpp
    src/guiprocesslogmessage.cpp
    src/guiprocesslogmessagehistory.cpp
    src/main.cpp
    src/program.cpp
    src/handler/incomingsockethandler.cpp
    src/handler/outgoingsockethandler.cpp
)

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

set(MOC_FILES "")
qt5_wrap_cpp(
    MOC_FILES 
    src/application.h
    src/handler/incomingsockethandler.h
    src/handler/outgoingsockethandler.h
)

set(RESOURCE_FILES "")
qt5_add_resources(RESOURCE_FILES ${CMAKE_SOURCE_DIR}/resources/resources.qrc)

set(APPLICATION_NAME C-Troll-Core)
add_executable(${APPLICATION_NAME} WIN32 MACOSX_BUNDLE
    ${SOURCE_FILES}
    ${HEADER_FILES}
    ${MOC_FILES}
    ${RESOURCE_FILES}
)

target_include_directories(${APPLICATION_NAME} PUBLIC "src") 
target_link_libraries(${APPLICATION_NAME} PUBLIC C-Troll-Common Qt5::Widgets Qt5::Network)
