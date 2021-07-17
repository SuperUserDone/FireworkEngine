#include "mesh_panel.hpp"

#include <imgui.h>

mesh_panel::mesh_panel(fw::scene_manager *man) : m_man(man) {}

void mesh_panel::update()
{
    ImGui::Begin("Meshes", &m_show);

    ImGui::End();
}

mesh_panel::~mesh_panel() {}
