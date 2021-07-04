#pragma once

#include "scene/scene.hpp"
#include "scene/scene_manager.hpp"
#include "serialize/scene_serializer.hpp"

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
};
