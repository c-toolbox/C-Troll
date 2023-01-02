##########################################################################################
#                                                                                        #
# Copyright (c) 2016 - 2023                                                              #
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
  centralwidget.h
  configuration.h
  mainwindow.h
  processhandler.h
  sockethandler.h
)

set(SOURCE_FILES
  centralwidget.cpp
  configuration.cpp
  main.cpp
  mainwindow.cpp
  processhandler.cpp
  sockethandler.cpp
)

set(MOC_FILES "")
qt_wrap_cpp(MOC_FILES
  centralwidget.h
  mainwindow.h
  processhandler.h
  sockethandler.h
)

set(RESOURCE_FILES "")
qt_add_resources(RESOURCE_FILES ${CMAKE_SOURCE_DIR}/resources/resources.qrc)

add_executable(Tray WIN32
  ${SOURCE_FILES}
  ${HEADER_FILES}
  ${MOC_FILES}
  ${RESOURCE_FILES}
  ${CMAKE_SOURCE_DIR}/resources/Tray.rc
)
target_link_libraries(Tray PRIVATE common Qt6::Core Qt6::Widgets Qt6::Network)
qt_disable_unicode_defines(Tray)
qt_finalize_target(Tray)
