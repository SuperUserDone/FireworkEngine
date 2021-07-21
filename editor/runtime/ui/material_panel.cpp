#include "material_panel.hpp"
#include "parts/imgui_utils.hpp"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

material_panel::material_panel(fw::scene_manager *man) : m_man(man) {}

void material_panel::update()
{
    if (!m_show) return;
    static int filtered = 0;

    ImGui::Begin("Materials\t\uf042", &m_show);

    std::string filter = "";
    ImGui::InputText("\uf0b0\tFilter Materials", &filter);

    fw::scene *scene = m_man->get_active_scene();
    static std::vector<std::string> selections;
    static std::vector<std::pair<uint32_t, std::string>> thumbs;

    auto &materials = scene->get_materials();

    ImGui::SameLine();
    bool refresh = ImGui::Button("\uf021\t Refresh previews");

    thumbs.clear();

    for (auto &&mat : materials) {
        auto &&[key, val] = mat;

        if (val->m_edit_data && val->m_edit_data->preview_texture) {
            thumbs.push_back({*(uint32_t *)val->m_edit_data->preview_texture->get_id(), key});

            if (m_size != val->m_edit_data->preview_size || refresh) val->m_edit_data->dirty = true;

            val->m_edit_data->preview_size = m_size;
        }
    }

    filtered = draw_grid(
        "Material Table", thumbs, selections, m_size, filter, [](const std::string &name) {
            LOG_DF("Edit material: {}", name);
        });

    ImGui::Separator();

    // if (ImGui::Button("\uf0fe\tAdd")) m_open.show();
    ImGui::SameLine();
    if (ImGui::Button("\uf146\tDelete")) {
        for (auto &a : selections) {
            scene->get_materials().erase(a);
            scene->set_dirty();
        }
        selections.clear();
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(256.f);
    ImGui::SliderFloat("##MatThumbSize", &m_size, 64.f, 1024.f, "\ufb15\tThumb size: %.0f");

    ImGui::End();
}

material_panel::~material_panel() {}
