#include "core/input.hpp"
#include "editor_ui.hpp"
#include "scene/components.hpp"

#include <glm/gtx/euler_angles.hpp>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

static bool shortcut_wrapper(blood::input::mod_keys mod, blood::input::key key, bool *clicked)
{
    static bool save_pressed = false;

    bool pressed = blood::input::check_shotcut(mod, key);

    if (pressed && !save_pressed) {
        *clicked = true;
        save_pressed = true;
    }
    if (save_pressed && !pressed) {
        save_pressed = false;
    }

    return *clicked;
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

static uint32_t curr = 0;
static void draw_scene_info(blood::scene *scene)
{
    ImGui::Begin("Scene View");

    entt::registry &reg = scene->get_registry();

    auto view = reg.view<blood::component_tag>();

    int i = view.size();

    ImGui::Text("Scene:");
    if (ImGui::BeginListBox(
            "##sceneview", ImVec2(-FLT_MIN, -FLT_MIN - ImGui::GetTextLineHeightWithSpacing()))) {
        for (auto entity : view) {

            auto &tag = view.get<blood::component_tag>(entity);

            const bool is_selected = (curr == (uint64_t)entity);
            if (ImGui::Selectable(tag.name.c_str(), is_selected)) curr = (uint64_t)entity;

            if (is_selected) ImGui::SetItemDefaultFocus();
        }

        ImGui::EndListBox();
    }

    ImGui::Text("Current entity %X", curr);

    ImGui::End();
}

static void draw_transform(blood::component_transform &comp)
{
    ImGui::DragFloat3("Position", &comp.pos.x, 0.01);

    static bool euler = true;
    ImGui::Checkbox("Rotation Euler", &euler);
    if (euler) {
        glm::vec3 euler = glm::degrees(glm::eulerAngles(comp.rot));
        ImGui::DragFloat3("Rotation", &euler.x, 1.f);
        comp.rot = glm::quat(glm::radians(glm::vec3(euler.x, euler.y, euler.z)));
    } else
        ImGui::DragFloat4("Rotation", &comp.rot.x, 0.1);

    ImGui::DragFloat3("Scale", &comp.scale.x, 0.1);
}

static void draw_camera(blood::component_camera &cam)
{
    ImGui::DragFloat("FOV", &cam.fov);

    // TODO preview
    static bool preview = false;
    ImGui::Checkbox("Preview", &preview);
    if (preview) ImGui::Text("Not Implemented");
}

static void draw_mesh(blood::component_mesh &mesh)
{
    // TODO
    ImGui::Text("Not Implemented");
}

static void draw_components(blood::scene *scene)
{
    ImGui::Begin("Components");

    if (curr != 0) {
        if (scene->get_registry().any_of<blood::component_tag>((entt::entity)curr)) {
            auto &tag = scene->get_registry().get<blood::component_tag>((entt::entity)curr);

            ImGui::InputText("Name", &tag.name);
            ImGui::Text("ID: %X", curr);
        }

        if (scene->get_registry().any_of<blood::component_camera>((entt::entity)curr)) {
            if (ImGui::CollapsingHeader("Camera")) {
                auto &cam = scene->get_registry().get<blood::component_camera>((entt::entity)curr);
                draw_camera(cam);
            }
        }

        if (scene->get_registry().any_of<blood::component_transform>((entt::entity)curr)) {
            if (ImGui::CollapsingHeader("Transform")) {
                auto &trans =
                    scene->get_registry().get<blood::component_transform>((entt::entity)curr);
                draw_transform(trans);
            }
        }

        if (scene->get_registry().any_of<blood::component_mesh>((entt::entity)curr)) {
            if (ImGui::CollapsingHeader("Mesh")) {
                auto &mesh = scene->get_registry().get<blood::component_mesh>((entt::entity)curr);
                draw_mesh(mesh);
            }
        }
    }

    ImGui::End();
}

static void draw_scene(glm::uvec2 &size, size_t id)
{
    ImGui::Begin("Scene");

    ImVec2 wsize{(ImGui::GetWindowSize().x - 15), (ImGui::GetWindowSize().y - 40)};

    size = {wsize.x, wsize.y};

    ImGui::Image((ImTextureID)(uint64_t)id, wsize, ImVec2(0, 1), ImVec2(1, 0));

    ImGui::End();
}

editor_ui::editor_ui(blood::scene_manager *man) : m_scene_man(man) {}

bool editor_ui::draw()
{
    blood::scene *scene = m_scene_man->get_active_scene();
    static bool styled = false;

    bool close = false;

    if (!styled) {
        style_editor();
        styled = true;
    }

    static bool save = false;

    static bool show_window_scene_view = true;
    static bool show_window_components = true;
    static bool show_window_scene = true;

    shortcut_wrapper(blood::input::MODKEY_CTRL, blood::input::KEY_s, &save);

    bool load = false;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            save = ImGui::MenuItem("Save", "Ctrl+S");
            if (ImGui::MenuItem("Load")) {
                curr = 0;
                load = true;
            }
            ImGui::Separator();
            close = ImGui::MenuItem("Close");

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {
            ImGui::MenuItem("Scene", nullptr, &show_window_scene);
            ImGui::MenuItem("Scene View", nullptr, &show_window_scene_view);
            ImGui::MenuItem("Components", nullptr, &show_window_components);

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();

        if (load) ImGui::OpenPopup("Open");

        if (ImGui::BeginPopupModal("Open")) {
            ImGui::Text("Open File");

            static char name[32];

            bool entered = ImGui::InputText("Name", name, 32, ImGuiInputTextFlags_EnterReturnsTrue);

            static bool show_error = false;
            if (show_error) {
                ImGui::TextColored(ImVec4(0.75, 0.2, 0.2, 1.0), "Could not open or parse scene!");
            }

            if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();

            ImGui::SameLine();
            if (ImGui::Button("Open") || entered) {
                blood::scene *new_scene = new blood::scene();

                if (blood::scene_serializer::deserialize(
                        new_scene, "root://" + std::string(name) + ".bscn")) {
                    m_scene_man->stage_scene(new_scene);
                    m_scene_man->swap();
                    ImGui::CloseCurrentPopup();

                } else
                    show_error = true;
            }
            ImGui::EndPopup();
        }
    }

    ImGui::DockSpaceOverViewport();

    if (show_window_scene_view) draw_scene_info(scene);
    if (show_window_components) draw_components(scene);
    if (show_window_scene) draw_scene(size, tex_id);

    if (save) {
        blood::scene_serializer::serialize(scene, "root://" + scene->get_name() + ".bscn");
        save = false;
    }
    return close;
}
