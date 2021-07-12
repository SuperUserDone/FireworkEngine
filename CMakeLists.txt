cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_TOOLCHAIN_FILE
    ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
    CACHE STRING "Vcpkg toolchain file")

project(
  FireworkEngine
  LANGUAGES CXX
  VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
  "extern/capnp/c++"
  "engine"
  "extern/tracy"
  "extern/include"
  "extern/include/entt"
  "extern/fmt/include"
  "${CMAKE_BINARY_DIR}/extern/sdl/include"
  "extern/sdl/include"
  "extern/imgui-docking/")

# add_definitions(-DTRACY_ENABLE)

find_package(SDL2 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(CapnProto CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(zstd CONFIG REQUIRED)

add_subdirectory(engine)
add_subdirectory(player)
add_subdirectory(editor)
add_subdirectory(converter)

if(UNIX)
  add_library(tracy SHARED "extern/tracy/TracyClient.cpp")
  add_compile_options("-fsanitize=address")
  add_link_options("-fsanitize=address")
endif()

install(
  DIRECTORY engine/
  DESTINATION include/firework_engine
  FILES_MATCHING
  PATTERN "*.hpp")
