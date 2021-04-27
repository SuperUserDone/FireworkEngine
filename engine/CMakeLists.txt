cmake_minimum_required(VERSION 3.20.0)

set(ENGINE_SRC "render/renderer.cpp" "core/loop.cpp" "scene/scene.cpp"
               "scene/entity.cpp" "scene/scene_manager.cpp")

add_library(blood-engine ${ENGINE_SRC})
add_dependencies(blood-engine SDL)

target_link_libraries(blood-engine ${INSTALL_DIR}/lib/libSDL2.a pthread dl
                      fmt::fmt)
