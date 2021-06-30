cmake_minimum_required(VERSION 3.20.0)

project(BloodEngine VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 1)

include_directories(
  "engine"
  "extern/tracy"
  "extern/include"
  "extern/include/entt"
  "extern/fmt/include"
  "${CMAKE_BINARY_DIR}/extern/sdl/include"
  "extern/sdl/include"
  "extern/imgui-docking/"
  "extern/capnp/")

# add_definitions(-DTRACY_ENABLE)

add_subdirectory(extern/fmt)
add_subdirectory(extern/sdl)
add_subdirectory(extern/freetype)

set(BUILD_TESTING OFF)
add_subdirectory(extern/capnp)

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
