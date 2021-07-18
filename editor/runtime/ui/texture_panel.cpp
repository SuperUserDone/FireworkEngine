#include "core/types/image2d.hpp"
#include "render/fonts/codepoint.hpp"
#include "texture_panel.hpp"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

texture_panel::texture_panel(fw::scene_manager *man)
    : m_man(man), m_open("Open texture", "Open file ", &m_name),
      m_open_error("Error", "Could not open file!")
{}

void texture_panel::update()
{
    if (!m_show) return;

    ImGui::Begin("Textures", &m_show);

    fw::scene *scene = m_man->get_active_scene();
    static ImVector<std::string> m_selections;

    if (ImGui::BeginTable(
            "Texture Table",
            2,
            ImGuiTableFlags_ScrollY,
            ImVec2{-FLT_MIN, -FLT_MIN - ImGui::GetTextLineHeightWithSpacing() - 8.f})) {

        ImGui::TableSetupColumn("Texture", ImGuiTableColumnFlags_WidthFixed, m_size + 4.f);
        ImGui::TableSetupColumn("Texture Properties");
        ImGui::TableHeadersRow();

        for (auto &&tex : scene->get_textures()) {
            auto &&[key, val] = tex;

            uint64_t id = *(uint32_t *)val->get_id();

            ImGui::TableNextRow(ImGuiTableRowFlags_None, m_size);
            ImGui::TableNextColumn();
            auto pos = ImGui::GetCursorPos();

            bool contains_selection = m_selections.contains(key);

            if (ImGui::Selectable(
                    ("##Selectable" + key).c_str(),
                    contains_selection,
                    ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap,
                    ImVec2(0, m_size))) {

                if (ImGui::GetIO().KeyCtrl) {
                    if (contains_selection)
                        m_selections.find_erase_unsorted(key);
                    else
                        m_selections.push_back(key);
                } else {
                    m_selections.clear();
                    m_selections.push_back(key);
                }
            }

            ImGui::SetCursorPos(pos);
            if (val->get_id()) ImGui::Image((ImTextureID)id, {m_size, m_size});
            ImGui::TableNextColumn();
            ImGui::Text("%s", key.c_str());
            ImGui::Text("Path: %s", val->m_path.c_str());
        }

        ImGui::EndTable();
    }

    ImGui::Separator();

    if (ImGui::Button("Add")) m_open.show();
    ImGui::SameLine();
    if (ImGui::Button("Delete")) {
        for (auto &a : m_selections) {
            scene->get_textures().erase(a);
            scene->set_dirty();
        }
        m_selections.clear();
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(256.f);
    ImGui::SliderFloat("##Size", &m_size, 64.f, 256.f, "Thumb size: %.0f");

    ImGui::End();

    m_open_error.update();

    if (m_open.update()) {
        fw::image2d img;

        std::string path = "root://" + m_name + ".fwtex";

        if (!img.load_from_file(path))
            m_open_error.show();
        else
            scene->get_textures()[m_name] = img.get_as_texture();
    }
}

texture_panel::~texture_panel() {}