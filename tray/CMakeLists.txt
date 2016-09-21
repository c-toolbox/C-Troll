set(HEADER_FILES
    src/sockethandler.h
    src/processhandler.h
)

set(SOURCE_FILES
    src/main.cpp
    src/processhandler.cpp
    src/sockethandler.cpp
)

# find_package(Qt5Widgets)
find_package(Qt5Network)

set(MOC_FILES "")
qt5_wrap_cpp(MOC_FILES ${HEADER_FILES})
# set(RESOURCE_FILES "")
# qt5_add_resources(RESOURCE_FILES ${application_path}/files.qrc)

set(APPLICATION_NAME C-Troll-Tray)
add_executable(${APPLICATION_NAME} MACOSX_BUNDLE
    ${SOURCE_FILES}
    ${HEADER_FILES}
    ${MOC_FILES}
    ${RESOURCE_FILES}
)

target_link_libraries(${APPLICATION_NAME}
    C-Troll-Common
    Qt5::Network
)

set_property(TARGET ${APPLICATION_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${APPLICATION_NAME} PROPERTY CXX_STANDARD_REQUIRED On)
