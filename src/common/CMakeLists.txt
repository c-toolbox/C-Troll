##########################################################################################
#                                                                                        #
# Copyright (c) 2016 - 2022                                                              #
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

set(HEADER_FILES
  include/apiversion.h
  include/baseconfiguration.h
  include/cluster.h
  include/erroroccurredmessage.h
  include/exitcommandmessage.h
  include/invalidauthmessage.h
  include/jsonload.h
  include/jsonsocket.h
  include/killallmessage.h
  include/killtraymessage.h
  include/logconfiguration.h
  include/logging.h
  include/message.h
  include/node.h
  include/processoutputmessage.h
  include/processstatusmessage.h
  include/program.h
  include/restartnodemessage.h
  include/startcommandmessage.h
  include/trayconnectedmessage.h
  include/traystatusmessage.h
  include/typedid.h
)

set(SOURCE_FILES
  src/baseconfiguration.cpp
  src/cluster.cpp
  src/erroroccurredmessage.cpp
  src/exitcommandmessage.cpp
  src/invalidauthmessage.cpp
  src/jsonsocket.cpp
  src/killallmessage.cpp
  src/killtraymessage.cpp
  src/logconfiguration.cpp
  src/logging.cpp
  src/message.cpp
  src/node.cpp
  src/processoutputmessage.cpp
  src/processstatusmessage.cpp
  src/program.cpp
  src/restartnodemessage.cpp
  src/startcommandmessage.cpp
  src/trayconnectedmessage.cpp
  src/traystatusmessage.cpp
)

set(MOC_FILES "")
qt_wrap_cpp(
  MOC_FILES
  include/jsonsocket.h
)

add_library(common
  ${SOURCE_FILES}
  ${HEADER_FILES}
  ${MOC_FILES}
  ${RESOURCE_FILES}
)

target_include_directories(common PUBLIC "include")
target_link_libraries(common PUBLIC project_options fmt nlohmann_json Qt::Core Qt::Widgets Qt::Network simplecrypt)
qt_disable_unicode_defines(common)
