##########################################################################################
#                                                                                        #
# Copyright (c) 2016-2023                                                                #
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

set(HEADER_FILES
  include/messages/erroroccurredmessage.h
  include/messages/exitcommandmessage.h
  include/messages/invalidauthmessage.h
  include/messages/killallmessage.h
  include/messages/killtraymessage.h
  include/messages/message.h
  include/messages/processoutputmessage.h
  include/messages/processstatusmessage.h
  include/messages/restartnodemessage.h
  include/messages/shutdownnodemessage.h
  include/messages/startcommandmessage.h
  include/messages/trayconnectedmessage.h
  include/messages/traystatusmessage.h
  include/baseconfiguration.h
  include/cluster.h
  include/jsonload.h
  include/jsonsocket.h
  include/jsonvalidation.h
  include/logconfiguration.h
  include/logging.h
  include/messages.h
  include/node.h
  include/commandlineparsing.h
  include/program.h
  include/typedid.h
  include/version.h
)

set(SOURCE_FILES
  src/messages/erroroccurredmessage.cpp
  src/messages/exitcommandmessage.cpp
  src/messages/invalidauthmessage.cpp
  src/messages/killallmessage.cpp
  src/messages/killtraymessage.cpp
  src/messages/message.cpp
  src/messages/processoutputmessage.cpp
  src/messages/processstatusmessage.cpp
  src/messages/restartnodemessage.cpp
  src/messages/shutdownnodemessage.cpp
  src/messages/startcommandmessage.cpp
  src/messages/trayconnectedmessage.cpp
  src/messages/traystatusmessage.cpp

  src/baseconfiguration.cpp
  src/cluster.cpp
  src/jsonsocket.cpp
  src/jsonvalidation.cpp
  src/logconfiguration.cpp
  src/logging.cpp
  src/node.cpp
  src/commandlineparsing.cpp
  src/program.cpp
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
target_link_libraries(
  common
  PUBLIC
    project_options
    nlohmann_json
    nlohmann_json_schema_validator
    Qt::Core
    Qt::Widgets
    Qt::Network
    simplecrypt
)
qt_disable_unicode_defines(common)
