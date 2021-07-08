cmake_minimum_required(VERSION 3.20.0)

set(ENGINE_SRC
    "core/loop.cpp"
    "core/input.cpp"
    "core/types/image2d.cpp"
    "render/renderer.cpp"
    "render/render_api/render_api.cpp"
    "render/render_api/opengl/render_api_opengl.cpp"
    "scene/scene.cpp"
    "scene/entity.cpp"
    "scene/scene_manager.cpp"
    "serialize/entity.capnp.c++"
    "serialize/scene.capnp.c++"
    "serialize/image.capnp.c++"
    "serialize/mesh.capnp.c++"
    "serialize/vector.capnp.c++"
    "serialize/scene_serializer.cpp"
    "vfs/loader.cpp"
    "../extern/imgui-docking/imgui.cpp"
    "../extern/imgui-docking/backends/imgui_impl_opengl3.cpp"
    "../extern/imgui-docking/imgui_widgets.cpp"
    "../extern/imgui-docking/imgui_draw.cpp"
    "../extern/imgui-docking/imgui_tables.cpp"
    "../extern/imgui-docking/backends/imgui_impl_sdl.cpp"
    "../extern/imgui-docking/misc/freetype/imgui_freetype.cpp"
    "../extern/imgui-docking/misc/cpp/imgui_stdlib.cpp")

add_library(firework-engine ${ENGINE_SRC})

target_compile_definitions(firework-engine PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD2
                                                  IMGUI_ENABLE_FREETYPE)

if(MSVC)
  set(ENGINE_LINK "")
elseif(UNIX)
  set(ENGINE_LINK "pthread" "dl" "tracy")
else()
  set(ENGINE_LINK "pthread")
endif(MSVC)

target_link_libraries(
  firework-engine
  SDL2-static
  ${ENGINE_LINK}
  freetype
  fmt::fmt
  capnp
  kj)
