set(HEADER_FILES
    src/application/application.h
    src/application/applicationhandler.h
    src/cluster/cluster.h
    src/cluster/clusterhandler.h
)

set(SOURCE_FILES
    src/application/application.cpp
    src/application/applicationhandler.cpp
    src/cluster/cluster.cpp
    src/cluster/clusterhandler.cpp
    src/main.cpp
)

# find_package(Qt5Widgets)
find_package(Qt5Network)

set(MOC_FILES "")
qt5_wrap_cpp(
    MOC_FILES
    ""
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

