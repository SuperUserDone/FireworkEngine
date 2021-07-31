#include "scene_tree.hpp"

#include <imgui.h>

void scene_tree::update(uint64_t &curr)
{
    if (!m_show) return;

    fw::scene *scene = m_man->get_active_scene();

    ImGui::Begin("Scene View\t\ufb44", &m_show);

    entt::registry &reg = scene->get_registry();

    auto view = reg.view<fw::component_tag>();

    int i = view.size();

    ImGui::Text("Scene:");
    if (ImGui::BeginListBox(
            "##sceneview", ImVec2(-FLT_MIN, -FLT_MIN - ImGui::GetTextLineHeightWithSpacing()))) {
        for (auto entity : view) {

            auto &tag = view.get<fw::component_tag>(entity);

            const bool is_selected = (curr == (uint64_t)entity);
            ImGui::PushID((uint32_t)entity);
            if (ImGui::Selectable(tag.name.c_str(), is_selected)) curr = (uint64_t)entity;
            if (is_selected) ImGui::SetItemDefaultFocus();
            ImGui::PopID();
        }

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("Right Click Context");
        }

        if (ImGui::BeginPopup("Right Click Context", true)) {
            if (ImGui::MenuItem(u8"\uf0fe\tAdd Entity", nullptr)) add_entity();
            if (ImGui::MenuItem(u8"\ufae7\tDelete", nullptr, false, (curr != 0)))
                remove_entity(curr);

            ImGui::EndPopup();
        }

        ImGui::EndListBox();
    }

    ImGui::Text("Current entity %lX", curr);

    ImGui::End();
}

scene_tree::~scene_tree() {}

void scene_tree::add_entity()
{
    fw::scene *scene = m_man->get_active_scene();

    scene->create_entity("New Entity");
}

void scene_tree::remove_entity(uint64_t &id)
{
    fw::scene *scene = m_man->get_active_scene();

    scene->get_registry().destroy((entt::entity)id);
    id = 0;
}

scene_tree::scene_tree(fw::scene_manager *man) : m_man(man) {}
