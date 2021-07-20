#include "mesh_panel.hpp"

#include <imgui.h>

mesh_panel::mesh_panel(fw::scene_manager *man) : m_man(man) {}

void mesh_panel::update()
{
    if (!m_show) return;
    ImGui::Begin("Meshes\t\ufa62", &m_show);

    ImGui::End();
}

mesh_panel::~mesh_panel() {}
