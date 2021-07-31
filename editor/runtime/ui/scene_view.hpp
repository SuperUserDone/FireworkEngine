#pragma once

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
                const glm::mat4 &view,
                const glm::mat4 &proj,
                ImGuizmo::OPERATION &opp);

    ~scene_view();

private:
};