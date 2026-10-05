function(add_cherno_chapter target)
    add_executable(${target} ${ARGN})
    target_compile_features(${target} PRIVATE cxx_std_20)
endfunction()

add_cherno_chapter(
  cherno_000_hello_world
  000-hello-world/main.cpp
)

add_cherno_chapter(
  cherno_005_funcitons_in_cpp
  005-functions-in-cpp/PushStack.cpp
)

add_cherno_chapter(
  cherno_006_header_files_in_cpp
  006-header-files-in-cpp/Log.h 006-header-files-in-cpp/Log.cpp 006-header-files-in-cpp/Main.cpp 006-header-files-in-cpp/Main2.cpp
)