#include "scene_tree.hpp"

#include <imgui.h>

void scene_tree::update(uint32_t &curr)
{
    if (!m_show) return;

    fw::scene *scene = m_man->get_active_scene();

    ImGui::Begin("Scene View", &m_show);

    entt::registry &reg = scene->get_registry();

    auto view = reg.view<fw::component_tag>();

    int i = view.size();

    ImGui::Text("Scene:");
    if (ImGui::BeginListBox(
            "##sceneview", ImVec2(-FLT_MIN, -FLT_MIN - ImGui::GetTextLineHeightWithSpacing()))) {
        for (auto entity : view) {

            auto &tag = view.get<fw::component_tag>(entity);

            const bool is_selected = (curr == (uint64_t)entity);
            if (ImGui::Selectable(tag.name.c_str(), is_selected)) curr = (uint64_t)entity;

            if (is_selected) ImGui::SetItemDefaultFocus();
        }

        ImGui::EndListBox();
    }

    ImGui::Text("Current entity %X", curr);

    ImGui::End();
}

scene_tree::~scene_tree() {}

scene_tree::scene_tree(fw::scene_manager *man) : m_man(man) {}
