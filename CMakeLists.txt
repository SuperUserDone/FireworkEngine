cmake_minimum_required(VERSION 3.20.0)

project(BloodEngine VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 1)

include_directories("engine" "extern/include" "extern/include/entt"
                    "extern/fmt/include")

include(ExternalProject)
ExternalProject_Add(
  SDL
  URL https://www.libsdl.org/release/SDL2-2.0.14.zip
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/SDL
  INSTALL_DIR SDL_install
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>)

ExternalProject_Get_Property(SDL INSTALL_DIR)
include_directories(${INSTALL_DIR}/include)

add_subdirectory(extern/fmt)

add_subdirectory(engine)
add_subdirectory(player)
add_subdirectory(editor)

install(
  DIRECTORY engine/
  DESTINATION include/blood_engine
  FILES_MATCHING
  PATTERN "*.hpp")
