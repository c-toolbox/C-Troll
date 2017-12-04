set(HEADER_FILES
    src/cluster.h
    src/handler/incomingsockethandler.h
    src/handler/outgoingsockethandler.h
    src/coreprocess.h
    src/program.h
    src/application.h
)

set(SOURCE_FILES
    src/cluster.cpp
    src/handler/incomingsockethandler.cpp
    src/handler/outgoingsockethandler.cpp
    src/coreprocess.cpp
    src/program.cpp
    src/application.cpp
    src/main.cpp
)

find_package(Qt5Widgets)
find_package(Qt5Network)

set(MOC_FILES "")
qt5_wrap_cpp(MOC_FILES 
    src/handler/incomingsockethandler.h
    src/handler/outgoingsockethandler.h
)

qt5_wrap_cpp(
    MOC_FILES
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

target_link_libraries(${APPLICATION_NAME}
    C-Troll-Common
	Qt5::Widgets
    Qt5::Network
)

set_property(TARGET ${APPLICATION_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${APPLICATION_NAME} PROPERTY CXX_STANDARD_REQUIRED On)
set_property(TARGET ${APPLICATION_NAME} PROPERTY LINK_FLAGS "/SUBSYSTEM:WINDOWS")

