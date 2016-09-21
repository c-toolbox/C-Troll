set(HEADER_FILES
    
)

set(SOURCE_FILES
    src/main.cpp
    
)

find_package(Qt5Network)
find_package(Qt5WebSockets)

set(MOC_FILES "")
qt5_wrap_cpp(MOC_FILES ${HEADER_FILES})

set(APPLICATION_NAME C-Troll-Webgui)
add_executable(${APPLICATION_NAME} MACOSX_BUNDLE
    ${SOURCE_FILES}
    ${HEADER_FILES}
    ${MOC_FILES}
    ${RESOURCE_FILES}
)

target_link_libraries(${APPLICATION_NAME}
    Qt5::Network
    Qt5::WebSockets
)

set_property(TARGET ${APPLICATION_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${APPLICATION_NAME} PROPERTY CXX_STANDARD_REQUIRED On)
