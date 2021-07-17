#pragma once

#include "scene/scene.hpp"
#include "scene/scene_manager.hpp"
#include "serialize/scene_serializer.hpp"
#include "ui/component_panel.hpp"
#include "ui/material_panel.hpp"
#include "ui/mesh_panel.hpp"
#include "ui/scene_tree.hpp"
#include "ui/scene_view.hpp"
#include "ui/texture_panel.hpp"

#include <glm/glm.hpp>
#include <stddef.h>
#include <stdint.h>

namespace fw {
class scene_manager;
} // namespace fw

class editor_ui
{
public:
    editor_ui(fw::scene_manager *man);

    glm::uvec2 get_size() { return size; }

    bool draw();

    void set_tex_id(size_t id) { tex_id = id; }

    ~editor_ui() {}

private:
    glm::uvec2 size = {10, 10};

    size_t tex_id;

    fw::scene_manager *m_scene_man;

    component_panel m_component_panel;
    scene_tree m_scene_tree_panel;
    scene_view m_scene_view_panel;

    material_panel m_mat_panel;
    mesh_panel m_mesh_panel;
    texture_panel m_texture_panel;

    uint32_t m_curr = 0;
};
