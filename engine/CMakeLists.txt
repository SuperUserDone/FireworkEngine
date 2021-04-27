cmake_minimum_required(VERSION 3.20.0)

set(ENGINE_SRC "render/renderer.cpp" "core/loop.cpp" "scene/scene.cpp"
               "scene/entity.cpp" "scene/scene_manager.cpp")

add_library(blood-engine ${ENGINE_SRC})
add_dependencies(blood-engine SDL2)

target_link_libraries(blood-engine SDL2 pthread dl fmt::fmt)
