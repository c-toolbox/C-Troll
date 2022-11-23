add_subdirectory(catch2)
add_subdirectory(fmt)
add_subdirectory(json)
add_subdirectory(simplecrypt)

target_compile_features(Catch2 PUBLIC cxx_std_20)
set_target_properties(Catch2 Catch2WithMain fmt simplecrypt PROPERTIES FOLDER External)
