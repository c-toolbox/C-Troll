function(set_compile_options project)
  target_compile_features(${project} PUBLIC cxx_std_20)
  target_compile_options(${project} PRIVATE /MP /permissive-)
endfunction()


add_subdirectory(catch2)
set_compile_options(Catch2)
set_target_properties(Catch2 PROPERTIES FOLDER External)
set_compile_options(Catch2WithMain)
set_target_properties(Catch2WithMain PROPERTIES FOLDER External)

add_subdirectory(fmt)
set_compile_options(fmt)
set_target_properties(fmt PROPERTIES FOLDER External)

add_subdirectory(json)

add_subdirectory(simplecrypt)
set_compile_options(simplecrypt)
set_target_properties(simplecrypt PROPERTIES FOLDER External)
