add_subdirectory(catch2)
add_subdirectory(fmt)
add_subdirectory(json)
add_subdirectory(simplecrypt)

set_target_properties(Catch2 Catch2WithMain fmt simplecrypt PROPERTIES FOLDER External)
