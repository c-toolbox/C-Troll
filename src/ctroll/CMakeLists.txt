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
  clusterconnectionhandler.h
  clusterwidget.h
  color.h
  configuration.h
  database.h
  logwidget.h
  mainwindow.h
  process.h
  processwidget.h
  programwidget.h
  restconnectionhandler.h
  settingswidget.h
)

set(SOURCE_FILES
  clusterconnectionhandler.cpp
  clusterwidget.cpp
  color.cpp
  configuration.cpp
  database.cpp
  logwidget.cpp
  mainwindow.cpp
  process.cpp
  processwidget.cpp
  programwidget.cpp
  restconnectionhandler.cpp
  settingswidget.cpp
  version.h
)

find_package(Qt6 COMPONENTS Core Gui Network Widgets REQUIRED)

set(MOC_FILES "")
qt_wrap_cpp(
  MOC_FILES
  clusterconnectionhandler.h
  clusterwidget.h
  mainwindow.h
  processwidget.h
  programwidget.h
  restconnectionhandler.h
  settingswidget.h
)

set(RESOURCE_FILES "")
qt_add_resources(RESOURCE_FILES ${CMAKE_SOURCE_DIR}/resources/resources.qrc)

add_executable(C-Troll WIN32 MACOSX_BUNDLE
  main.cpp
  ${SOURCE_FILES}
  ${HEADER_FILES}
  ${MOC_FILES}
  ${RESOURCE_FILES}
  ${CMAKE_SOURCE_DIR}/resources/icon.rc
)
target_link_libraries(C-Troll PUBLIC common Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network)
qt_disable_unicode_defines(C-Troll)

if (WIN32)
  # Find the windeployqt application
  get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
  get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
  find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
  add_custom_command(
    TARGET C-Troll POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}" --verbose 0 --no-compiler-runtime --no-translations --no-virtualkeyboard --no-opengl-sw --no-system-d3d-compiler \"$<TARGET_FILE:C-Troll>\"
    COMMENT "Deploying Qt libraries"
  )
endif ()

qt_finalize_target(C-Troll)
