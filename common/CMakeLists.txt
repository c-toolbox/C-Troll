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

set(HEADER_FILES
  include/apiversion.h
  include/exitcommandmessage.h
  include/invalidauthmessage.h
  include/jsonload.h
  include/jsonsocket.h
  include/killallmessage.h
  include/logging.h
  include/message.h
  include/processoutputmessage.h
  include/processstatusmessage.h
  include/startcommandmessage.h
  include/traystatusmessage.h
)

set(SOURCE_FILES
  src/exitcommandmessage.cpp
  src/invalidauthmessage.cpp
  src/jsonsocket.cpp
  src/killallmessage.cpp
  src/logging.cpp
  src/message.cpp
  src/processoutputmessage.cpp
  src/processstatusmessage.cpp
  src/startcommandmessage.cpp
  src/traystatusmessage.cpp
)

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

set(MOC_FILES "")
qt5_wrap_cpp(
  MOC_FILES
  include/jsonsocket.h
)

add_library(common
  ${SOURCE_FILES}
  ${HEADER_FILES}
  ${MOC_FILES}
  ${RESOURCE_FILES}
)

add_subdirectory(ext/fmt)

target_include_directories(common PUBLIC "include" "ext" "ext/fmt/include")
target_link_libraries(common project_options fmt Qt5::Core Qt5::Widgets Qt5::Network)
