#include "core/types/image2d.hpp"
#include "core/types/mesh.hpp"
#include "core/util.hpp"
#include "editor_ui.hpp"
#include "render/render_data/texture2d.hpp"
#include "scene/scene.hpp"
#include "scene/scene_manager.hpp"
#include "serialize/scene_serializer.hpp"
#include "ui/parts/modal.hpp"

#include <SDL_scancode.h>
#include <imgui.h>
#include <imgui_demo.cpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

float SRGBToLinear(float in)
{
    if (in <= 0.04045f)
        return in / 12.92f;
    else
        return std::pow((in) / 1.0f, 2.2f);
}

float LinearToSRGB(float in)
{
    if (in <= 0.0031308f)
        return in * 12.92f;
    else
        return std::pow(in, 1.0f / 2.2f);
}

ImVec4 SRGBToLinear(ImVec4 col)
{
    col.x = SRGBToLinear(col.x);
    col.y = SRGBToLinear(col.y);
    col.z = SRGBToLinear(col.z);
    // Alpha component is already linear

    return col;
}

ImVec4 LinearToSRGB(ImVec4 col)
{
    col.x = LinearToSRGB(col.x);
    col.y = LinearToSRGB(col.y);
    col.z = LinearToSRGB(col.z);
    // Alpha component is already linear

    return col;
}

static void style_editor()
{
    ImVec4 *colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(1.00f, 0.25f, 0.19f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.59f, 0.21f, 0.18f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.69f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.31f, 0.31f, 0.31f, 0.70f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.35f);
    colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);

    for (int i = 0; i < ImGuiCol_COUNT; i++) {
        ImVec4 &col = colors[i];

        col = SRGBToLinear(col);
    }

    ImGuiStyle &style = ImGui::GetStyle();

    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(3, 3);
    style.CellPadding = ImVec2(4, 2);
    style.ItemSpacing = ImVec2(8, 4);
    style.ItemInnerSpacing = ImVec2(4, 4);
    style.TouchExtraPadding = ImVec2(0, 0);

    style.IndentSpacing = 20;
    style.ScrollbarSize = 12;
    style.GrabMinSize = 6;

    style.WindowBorderSize = 0;
    style.ChildBorderSize = 0;
    style.PopupBorderSize = 0;
    style.FrameBorderSize = 0;
    style.TabBorderSize = 0;

    style.WindowRounding = 6;
    style.ChildRounding = 6;
    style.FrameRounding = 1;
    style.PopupRounding = 6;
    style.ScrollbarRounding = 12;
    style.GrabRounding = 1;
    style.TabRounding = 3;
}

editor_ui::editor_ui(fw::scene_manager *man)
    : m_scene_man(man), m_component_panel(man), m_scene_tree_panel(man), m_mat_panel(man),
      m_mesh_panel(man), m_texture_panel(man)
{}

bool editor_ui::draw()
{
    ImGui::ShowStyleEditor();

    fw::scene *scene = m_scene_man->get_active_scene();

    static bool styled = false;
    if (!styled) {
        style_editor();
        styled = true;
    }

    bool close = false;
    bool save = false;
    bool load = false;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {

            save = ImGui::MenuItem("Save", "Ctrl+S");
            load = ImGui::MenuItem("Load");

            ImGui::Separator();

            close = ImGui::MenuItem("Close");

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {

            ImGui::MenuItem("Scene", nullptr, &m_scene_view_panel.m_show);
            ImGui::MenuItem("Scene View", nullptr, &m_scene_tree_panel.m_show);
            ImGui::MenuItem("Components", nullptr, &m_component_panel.m_show);
            ImGui::MenuItem("Meshes", nullptr, &m_mesh_panel.m_show);
            ImGui::MenuItem("Textures", nullptr, &m_texture_panel.m_show);
            ImGui::MenuItem("Materials", nullptr, &m_mat_panel.m_show);

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (load) {

        m_curr = 0;
        fw::scene *new_scene = new fw::scene();

        // TODO name
        if (fw::scene_serializer::deserialize(new_scene, "root://Scene.fwscn")) {
            m_scene_man->stage_scene(new_scene);
            m_scene_man->swap();
            ImGui::CloseCurrentPopup();
        }
    }

    ImGui::DockSpaceOverViewport();

    m_scene_tree_panel.update(m_curr);
    m_component_panel.update(m_curr);
    m_scene_view_panel.update(size, tex_id);
    m_mat_panel.update();
    m_mesh_panel.update();
    m_texture_panel.update();

    auto &io = ImGui::GetIO();
    save |= (io.KeyCtrl && ImGui::IsKeyPressed(SDL_SCANCODE_S));

    if (save) {
        fw::scene_serializer::serialize(scene, "root://" + scene->get_name() + ".fwscn");
        save = false;
    }
    return close;
}
