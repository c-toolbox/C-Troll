function(set_compile_options project)
  set(MSVC_WARNINGS /MP /ZI /permissive- /W0)
  set(CLANG_WARNINGS -Wno-everything)
  set(GCC_WARNINGS -w)

  target_compile_features(${project} PUBLIC cxx_std_20)
  if (MSVC)
    target_compile_options(${project} PRIVATE ${MSVC_WARNINGS})
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${project} PRIVATE ${CLANG_WARNINGS})
  else ()
    target_compile_options(${project} PRIVATE ${GCC_WARNINGS})
  endif ()
  set_target_properties(${project} PROPERTIES FOLDER External)
endfunction()


add_subdirectory(catch2)
set_compile_options(Catch2)
set_compile_options(Catch2WithMain)

add_subdirectory(fmt)
set_compile_options(fmt)

add_subdirectory(json)

add_subdirectory(simplecrypt)
set_compile_options(simplecrypt)
