#include "mesh_panel.hpp"
#include "parts/imgui_utils.hpp"

#include <imgui.h>

mesh_panel::mesh_panel(fw::scene_manager *man) : m_man(man) {}

void mesh_panel::update()
{
    if (!m_show) return;
    static int filtered = 0;

    ImGui::Begin("Meshes\t\ufa62", &m_show);

    ImGui::End();
}

mesh_panel::~mesh_panel() {}
