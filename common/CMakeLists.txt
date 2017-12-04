set(HEADER_FILES
    include/genericmessage.h
    include/guistartcommand.h
    include/guiprocesscommand.h
    include/guiinitialization.h
    include/guiprocessstatus.h
    include/guiprocesslogmessage.h
    include/guiprocesslogmessagehistory.h
    include/jsonsocket.h
    include/jsonsupport.h
    include/logging.h
	include/standardmainwindow.h
    include/traycommand.h
    include/trayprocesslogmessage.h
    include/trayprocessstatus.h
)

set(SOURCE_FILES
    src/genericmessage.cpp
    src/guistartcommand.cpp
    src/guiprocesscommand.cpp
    src/guiinitialization.cpp
    src/guiprocessstatus.cpp
    src/guiprocesslogmessage.cpp
    src/guiprocesslogmessagehistory.cpp
    src/jsonsocket.cpp
    src/jsonsupport.cpp
    src/logging.cpp
	src/standardmainwindow.cpp
    src/traycommand.cpp
    src/trayprocesslogmessage.cpp
    src/trayprocessstatus.cpp
)

find_package(Qt5Widgets)
find_package(Qt5Network)

set(MOC_FILES "")
qt5_wrap_cpp(MOC_FILES 
    include/jsonsocket.h
	include/standardmainwindow.h
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
	Qt5::Widgets
    Qt5::Network
)

set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${LIBRARY_NAME} PROPERTY CXX_STANDARD_REQUIRED On)

