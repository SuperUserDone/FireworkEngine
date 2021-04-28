cmake_minimum_required(VERSION 3.20.0)

set(ENGINE_SRC
    "render/renderer.cpp"
    "core/loop.cpp"
    "scene/scene.cpp"
    "scene/entity.cpp"
    "scene/scene_manager.cpp"
    "../extern/imgui-docking/imgui.cpp"
    "../extern/imgui-docking/backends/imgui_impl_opengl3.cpp"
    "../extern/imgui-docking/imgui_widgets.cpp"
    "../extern/imgui-docking/imgui_draw.cpp"
    "../extern/imgui-docking/imgui_tables.cpp"
    "../extern/imgui-docking/backends/imgui_impl_sdl.cpp")

add_library(blood-engine ${ENGINE_SRC})

target_compile_definitions(blood-engine PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD2)

if(MSVC)
  set(ENGINE_LINK "")
elseif(UNIX)
  set(ENGINE_LINK "pthread" "dl")
else()
  set(ENGINE_LINK "pthread")
endif(MSVC)

target_link_libraries(blood-engine SDL2 ${ENGINE_LINK} fmt::fmt)
