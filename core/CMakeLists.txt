set(HEADER_FILES

)

set(SOURCE_FILES
    main.cpp
)

# find_package(Qt5Widgets)
find_package(Qt5Network)

set(MOC_FILES "")
qt5_wrap_cpp(MOC_FILES ${HEADER_FILES})
# set(RESOURCE_FILES "")
# qt5_add_resources(RESOURCE_FILES ${application_path}/files.qrc)

set(APPLICATION_NAME C-Troll-Core)
add_executable(APPLICATION_NAME MACOSX_BUNDLE
    ${SOURCE_FILES}
    ${HEADER_FILES}
    ${MOC_FILES}
    ${RESOURCE_FILES}
)

target_link_libraries(${APPLICATION_NAME}
    Qt5::Network
)
