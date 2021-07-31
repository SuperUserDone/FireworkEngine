#pragma once

#include "scene/scene_manager.hpp"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

// Include after imgui
#include <ImGuizmo/ImGuizmo.h>

class component_panel
{
public:
    component_panel(fw::scene_manager *man);

    bool m_show = true;

    void update(const uint32_t &curr, ImGuizmo::OPERATION &opp);

    ~component_panel();

private:
    fw::scene_manager *m_manager;
};