#include "component_panel.hpp"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

static void draw_transform(fw::component_transform &comp)
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

static void draw_camera(fw::component_camera &cam)
{
    ImGui::DragFloat("FOV", &cam.fov);

    // TODO preview
    static bool preview = false;
    ImGui::Checkbox("Preview", &preview);
    if (preview) ImGui::Text("Not Implemented");
}

static void draw_mesh(fw::component_mesh_renderer &mesh)
{
    bool dirty = 0;

    dirty |= ImGui::InputText("Mesh", &mesh.mesh_named_ref);
    if (ImGui::BeginDragDropTarget()) {

        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("MESH")) {
            std::string name((const char *)payload->Data);
            mesh.mesh_named_ref = name;
            dirty = true;
        }

        ImGui::EndDragDropTarget();
    }

    dirty |= ImGui::InputText("Material", &mesh.mat_named_ref);
    if (ImGui::BeginDragDropTarget()) {

        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("MATERIAL")) {
            std::string name((const char *)payload->Data);
            mesh.mat_named_ref = name;
            dirty = true;
        }

        ImGui::EndDragDropTarget();
    }

    if (dirty) mesh.lookup_count = mesh.lookup_freq + 1;
}

component_panel::component_panel(fw::scene_manager *man) : m_manager(man) {}

void component_panel::update(const uint32_t &curr)
{
    if (!m_show) return;

    ImGui::Begin("Components\t\uf592", &m_show);

    fw::scene *scene = m_manager->get_active_scene();

    if (curr != 0) {
        ImGui::PushID(curr);
        if (scene->get_registry().any_of<fw::component_tag>((entt::entity)curr)) {
            auto &tag = scene->get_registry().get<fw::component_tag>((entt::entity)curr);

            ImGui::InputText("Name", &tag.name);
            ImGui::Text("ID: %X", curr);
        }

        if (scene->get_registry().any_of<fw::component_camera>((entt::entity)curr)) {
            if (ImGui::CollapsingHeader("Camera")) {
                auto &cam = scene->get_registry().get<fw::component_camera>((entt::entity)curr);
                draw_camera(cam);

                ImGui::Separator();
                if (ImGui::Button("Remove Camera"))
                    scene->get_registry().remove_if_exists<fw::component_camera>(
                        (entt::entity)curr);
            }
        }

        if (scene->get_registry().any_of<fw::component_transform>((entt::entity)curr)) {
            if (ImGui::CollapsingHeader("Transform")) {
                auto &trans =
                    scene->get_registry().get<fw::component_transform>((entt::entity)curr);
                draw_transform(trans);
            }
        }

        if (scene->get_registry().any_of<fw::component_mesh_renderer>((entt::entity)curr)) {
            if (ImGui::CollapsingHeader("Mesh Renderer")) {
                auto &mesh =
                    scene->get_registry().get<fw::component_mesh_renderer>((entt::entity)curr);
                draw_mesh(mesh);

                ImGui::Separator();
                if (ImGui::Button("Remove Mesh"))
                    scene->get_registry().remove_if_exists<fw::component_mesh_renderer>(
                        (entt::entity)curr);
            }
        }
        ImGui::PopID();
    }

    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && curr != 0) {
        ImGui::OpenPopup("Right Click Context");
    }

    if (ImGui::BeginPopup("Right Click Context", true)) {

        if (ImGui::BeginMenu("Add")) {

            if (ImGui::MenuItem("Add Component Mesh Renderer")) {
                scene->get_registry().emplace<fw::component_mesh_renderer>((entt::entity)curr);
            }
            if (ImGui::MenuItem("Add Component Camera")) {
                scene->get_registry().emplace<fw::component_camera>((entt::entity)curr);
            }

            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

component_panel::~component_panel() {}
