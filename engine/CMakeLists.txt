cmake_minimum_required(VERSION 3.20.0)

set(ENGINE_SRC "render/renderer.cpp" "core/loop.cpp" "scene/scene.cpp"
               "scene/entity.cpp")

add_library(blood-engine ${ENGINE_SRC})

target_link_libraries(blood-engine ${SDL_LIBRARIES} pthread dl fmt::fmt)
