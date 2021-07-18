#include "material_panel.hpp"

#include <imgui.h>

material_panel::material_panel(fw::scene_manager *man) : m_man(man) {}

void material_panel::update()
{
    if (!m_show) return;

    ImGui::Begin("Materials", &m_show);

    ImGui::End();
}

material_panel::~material_panel() {}
