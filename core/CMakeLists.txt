set(HEADER_FILES
    src/application/application.h
    src/application/applicationhandler.h
    src/cluster/cluster.h
    src/cluster/clusterhandler.h
    src/handler/mainhandler.h
    src/handler/sockethandler.h
    src/configuration.h
    src/traycommand.h
)

set(SOURCE_FILES
    src/application/application.cpp
    src/application/applicationhandler.cpp
    src/cluster/cluster.cpp
    src/cluster/clusterhandler.cpp
    src/handler/mainhandler.cpp
    src/handler/sockethandler.cpp
    src/configuration.cpp
    src/main.cpp
    src/traycommand.cpp
)

# find_package(Qt5Widgets)
find_package(Qt5Network)

set(MOC_FILES 
    src/handler/mainhandler.h
    src/handler/sockethandler.h
)

qt5_wrap_cpp(
    MOC_FILES
)

# set(RESOURCE_FILES "")
# qt5_add_resources(RESOURCE_FILES ${application_path}/files.qrc)

set(APPLICATION_NAME C-Troll-Core)
add_executable(${APPLICATION_NAME} MACOSX_BUNDLE
    ${SOURCE_FILES}
    ${HEADER_FILES}
    ${MOC_FILES}
    ${RESOURCE_FILES}
)

target_include_directories(${APPLICATION_NAME} PUBLIC "src") 

target_link_libraries(${APPLICATION_NAME}
    Qt5::Network
)

set_property(TARGET ${APPLICATION_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${APPLICATION_NAME} PROPERTY CXX_STANDARD_REQUIRED On)

