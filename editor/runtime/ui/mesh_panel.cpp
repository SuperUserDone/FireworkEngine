#include "mesh_panel.hpp"
#include "parts/imgui_utils.hpp"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

mesh_panel::mesh_panel(fw::scene_manager *man) : m_man(man) {}

void mesh_panel::update()
{
    if (!m_show) return;
    static int filtered = 0;

    ImGui::Begin("Meshes\t\ufa62", &m_show);
    std::string filter = "";
    ImGui::InputText("\uf0b0\tFilter Meshes", &filter);

    fw::scene *scene = m_man->get_active_scene();
    static std::vector<std::string> selections;
    static std::vector<thumbnail> thumbs;

    auto &meshes = scene->get_meshes();
    thumbs.clear();

    for (auto &&mesh : meshes) {
        auto &&[key, val] = mesh;

        thumbs.push_back({0, key, u8"\uf6a6"});
    }

    filtered =
        draw_grid("Mesh Table", thumbs, selections, m_size, filter, [](const std::string &name) {
            LOG_DF("Edit mesh: {}", name);
        });

    ImGui::Separator();

    // if (ImGui::Button("\uf0fe\tAdd")) m_open.show();
    ImGui::SameLine();
    if (ImGui::Button("\ufae7\tDelete")) {
        for (auto &a : selections) {
            scene->get_meshes().erase(a);
            scene->set_dirty();
        }
        selections.clear();
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(256.f);
    ImGui::SliderFloat("##MeshThumbSize", &m_size, 64.f, 128.f, "\ufb15\tThumb size: %.0f");
    ImGui::End();
}

mesh_panel::~mesh_panel() {}
