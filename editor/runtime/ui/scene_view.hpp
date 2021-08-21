#pragma once

#include "../editor_camera.hpp"
#include "glm/glm.hpp"
#include "scene/scene.hpp"

#include <imgui.h>

// After imgui
#include <ImGuizmo/ImGuizmo.h>

class scene_view
{
public:
    scene_view();

    bool m_show = true;

    void update(glm::uvec2 &size,
                size_t id,
                fw::scene *scene,
                uint32_t curr,
                ImGuizmo::OPERATION &opp,
                editor_camera &cam);

    void camera(editor_camera &cam, float x, float y);

    ~scene_view();

private:
};