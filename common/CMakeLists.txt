set(HEADER_FILES
    include/genericmessage.h
    include/guicommand.h
    include/guiinitialization.h
    include/jsonsupport.h
    include/traycommand.h
    include/trayprocesslogmessage.h
    include/trayprocessstatus.h
)

set(SOURCE_FILES
    src/genericmessage.cpp
    src/guicommand.cpp
    src/guiinitialization.cpp
    src/jsonsupport.cpp
    src/traycommand.cpp
    src/trayprocesslogmessage.cpp
    src/trayprocessstatus.cpp
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

