cmake_minimum_required(VERSION 3.0.3)

set(EDITOR_SRC
    "edit.cpp"
    "runtime/editor_loop.cpp"
    "runtime/editor_ui.cpp"
    "runtime/ui/parts/modal.cpp"
    "runtime/ui/component_panel.cpp"
    "runtime/ui/scene_tree.cpp"
    "runtime/ui/scene_view.cpp"
    "runtime/ui/material_panel.cpp"
    "runtime/ui/mesh_panel.cpp"
    "runtime/ui/texture_panel.cpp")

add_executable(firework-editor ${EDITOR_SRC})
target_link_libraries(firework-editor firework-engine)
