cmake_minimum_required(VERSION 3.20.0)

project(
  BloodEngine
  LANGUAGES CXX
  VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC_VERSION GREATER_EQUAL "1900")
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag("/std:c++latest" _cpp_latest_flag_supported)
  if(_cpp_latest_flag_supported)
    add_compile_options("/std:c++latest")
  endif()
endif()

include_directories(
  "engine"
  "extern/tracy"
  "extern/include"
  "extern/include/entt"
  "extern/fmt/include"
  "${CMAKE_BINARY_DIR}/extern/sdl/include"
  "extern/sdl/include"
  "extern/imgui-docking/"
  "extern/capnp/c++")

# add_definitions(-DTRACY_ENABLE)

set(BUILD_TESTING OFF)
add_subdirectory(extern/capnp)

add_subdirectory(extern/fmt)
add_subdirectory(extern/sdl)
add_subdirectory(extern/freetype)

add_subdirectory(engine)
add_subdirectory(player)
add_subdirectory(editor)

if(UNIX)
  add_library(tracy SHARED "extern/tracy/TracyClient.cpp")
endif()

install(
  DIRECTORY engine/
  DESTINATION include/blood_engine
  FILES_MATCHING
  PATTERN "*.hpp")
