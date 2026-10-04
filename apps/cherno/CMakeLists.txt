function(add_cherno_chapter target)
    add_executable(${target} ${ARGN})
    target_compile_features(${target} PRIVATE cxx_std_20)
endfunction()

add_cherno_chapter(
  cherno_001_hello_world
  001-hello-world/main.cpp
)