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
    fw::scene *scene = m_man->get_active_scene();

    ImGui::Begin("Textures", &m_show);

    if (ImGui::BeginTable(
            "Texture Table",
            2,
            ImGuiTableFlags_ScrollY,
            ImVec2{-FLT_MIN, -FLT_MIN - ImGui::GetTextLineHeightWithSpacing() - 8.f})) {

        ImGui::TableSetupColumn("Texture", ImGuiTableColumnFlags_WidthFixed, 132.f);
        ImGui::TableSetupColumn("Texture Properties");
        ImGui::TableHeadersRow();

        for (auto &&tex : scene->get_textures()) {
            auto &&[key, val] = tex;

            uint64_t id = *(uint32_t *)val->get_id();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", key.c_str());
            if (val->get_id()) ImGui::Image((ImTextureID)id, {128, 128});
            ImGui::TableNextColumn();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetTextLineHeightWithSpacing());
            ImGui::Text("Path: %s", val->m_path.c_str());
        }

        ImGui::EndTable();
    }

    ImGui::Separator();

    if (ImGui::Button("Add")) m_open.show();

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