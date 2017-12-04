set(HEADER_FILES
    src/sockethandler.h
    src/processhandler.h
    src/trayprocess.h
)

set(SOURCE_FILES
    src/main.cpp
    src/processhandler.cpp
    src/sockethandler.cpp
    src/trayprocess.cpp
)

find_package(Qt5Widgets)
find_package(Qt5Network)

set(MOC_FILES "")
qt5_wrap_cpp(MOC_FILES ${HEADER_FILES})
set(RESOURCE_FILES "")
qt5_add_resources(RESOURCE_FILES ${CMAKE_SOURCE_DIR}/resources/resources.qrc)

set(APPLICATION_NAME C-Troll-Tray)
add_executable(${APPLICATION_NAME} WIN32 MACOSX_BUNDLE
    ${SOURCE_FILES}
    ${HEADER_FILES}
    ${MOC_FILES}
    ${RESOURCE_FILES}
)

target_link_libraries(${APPLICATION_NAME}
    C-Troll-Common
    Qt5::Widgets
    Qt5::Network
)

set_property(TARGET ${APPLICATION_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${APPLICATION_NAME} PROPERTY CXX_STANDARD_REQUIRED On)
set_property(TARGET ${APPLICATION_NAME} PROPERTY LINK_FLAGS "/SUBSYSTEM:WINDOWS")