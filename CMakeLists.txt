cmake_minimum_required(VERSION 3.20.0)

project(BloodEngine VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 1)

include_directories("engine" "extern/include" "extern/include/entt")

include(FindPkgConfig)

pkg_check_modules(SDL sdl2)

include_directories(${SDL_INCLUDE_DIRS})

add_subdirectory(engine)
add_subdirectory(player)
