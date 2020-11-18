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
)

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

set(MOC_FILES "")
qt5_wrap_cpp(
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
qt5_add_resources(RESOURCE_FILES ${CMAKE_SOURCE_DIR}/resources/resources.qrc)

add_executable(C-Troll WIN32 MACOSX_BUNDLE
  main.cpp
  ${SOURCE_FILES}
  ${HEADER_FILES}
  ${MOC_FILES}
  ${RESOURCE_FILES}
  ${CMAKE_SOURCE_DIR}/resources/icon.rc  
)
target_include_directories(C-Troll PUBLIC "src")
target_link_libraries(C-Troll PUBLIC core-shared Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)

if (WIN32)
  foreach (lib Qt5::Core Qt5::Gui Qt5::Network Qt5::Widgets)
    copy_dynamic_library(${lib} C-Troll)
  endforeach ()
  
  copy_qt_platforms_plugin(C-Troll)
endif (WIN32)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # These warnings are in Qt 5.12.2 and raised on GCC 9.3.0
  set_source_files_properties("${CMAKE_BINARY_DIR}/core/ctroll/moc_clusterconnectionhandler.cpp" PROPERTIES COMPILE_FLAGS "-Wno-useless-cast")
  set_source_files_properties("${CMAKE_BINARY_DIR}/core/ctroll/moc_clusterwidget.cpp" PROPERTIES COMPILE_FLAGS "-Wno-useless-cast")
  set_source_files_properties("${CMAKE_BINARY_DIR}/core/ctroll/moc_processwidget.cpp" PROPERTIES COMPILE_FLAGS "-Wno-useless-cast")
  set_source_files_properties("${CMAKE_BINARY_DIR}/core/ctroll/moc_programwidget.cpp" PROPERTIES COMPILE_FLAGS "-Wno-useless-cast")
  set_source_files_properties("${CMAKE_BINARY_DIR}/core/ctroll/moc_restconnectionhandler.cpp" PROPERTIES COMPILE_FLAGS "-Wno-useless-cast")
  set_source_files_properties("${CMAKE_BINARY_DIR}/core/ctroll/moc_settingswidget.cpp" PROPERTIES COMPILE_FLAGS "-Wno-useless-cast")
endif ()
