set(HEADER_FILES
    include/traycommand.h
)

set(SOURCE_FILES
    src/traycommand.cpp
)

# find_package(Qt5Widgets)
find_package(Qt5Network)

set(MOC_FILES "")
qt5_wrap_cpp(MOC_FILES 
)

qt5_wrap_cpp(MOC_FILES)

set(LIBRARY_NAME C-Troll-Common)
add_library(${LIBRARY_NAME}
    ${SOURCE_FILES}
    ${HEADER_FILES}
    ${MOC_FILES}
    ${RESOURCE_FILES}
)

target_include_directories(${LIBRARY_NAME} PUBLIC "include") 

target_link_libraries(${LIBRARY_NAME}
    Qt5::Network
)

set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_STANDARD_REQUIRED On)

