cmake_minimum_required(VERSION 3.20.0)

project(BloodEngine VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 1)

include_directories("engine" "extern/include" "extern/include/entt"
                    "extern/fmt/include")

include(FindPkgConfig)

pkg_check_modules(SDL sdl2)

add_subdirectory(extern/fmt)

include_directories(${SDL_INCLUDE_DIRS})

add_subdirectory(engine)
add_subdirectory(player)
add_subdirectory(editor)

install(
  DIRECTORY engine/
  DESTINATION include/blood_engine
  FILES_MATCHING
  PATTERN "*.hpp")
