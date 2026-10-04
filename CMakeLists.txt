cmake_minimum_required(VERSION 3.20)
project(cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------------------------------------------------
option (BUILD_CHERNO "Build the Cherno course exercise" ON)

if (BUILD_CHERNO)
  add_subdirectory(apps/cherno)
endif()