cmake_minimum_required(VERSION 3.20)

project(git_issues LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.1/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/)

add_executable(git-issue 
  src/main.cpp
  src/util.cpp

  src/commands/addCommand.cpp
  src/commands/baseCommand.cpp
  src/commands/closeCommand.cpp
  src/commands/initCommand.cpp
  src/commands/removeCommand.cpp
)
target_include_directories(git-issue PRIVATE include/)
target_compile_definitions(git-issue PUBLIC GIT_ISSUE_VERSION="v0.1")

CPMAddPackage(
  NAME cxxopts
  GITHUB_REPOSITORY jarro2783/cxxopts
  VERSION 3.3.1
)

CPMAddPackage(
  NAME json
  GITHUB_REPOSITORY nlohmann/json
  VERSION 3.12.0
)

set(REPROC++ ON)
CPMAddPackage(
  NAME reproc
  GITHUB_REPOSITORY daandemeyer/reproc
  VERSION 14.2.5
)

target_link_libraries(git-issue PRIVATE cxxopts nlohmann_json::nlohmann_json reproc++)