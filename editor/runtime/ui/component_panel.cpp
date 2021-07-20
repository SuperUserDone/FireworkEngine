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

static void draw_mesh(fw::component_mesh &mesh)
{
    // TODO
    ImGui::Text("Not Implemented");
}

static void draw_material(fw::scene *scene, fw::component_material &mat)
{
    ImGui::Text("%s", mat.named_ref.c_str());
    if (mat.material_ref) {
        ImGui::Text("%s", mat.material_ref->m_shader_named_ref.c_str());
        if (mat.material_ref->get_attribs().size() > 0) {
            ImGui::InputText("Text", &mat.material_ref->get_attribs()[0].data_texture_name);
        }
    };
}

component_panel::component_panel(fw::scene_manager *man) : m_manager(man) {}

void component_panel::update(const uint32_t &curr)
{
    if (!m_show) return;

    ImGui::Begin("Components\t\uf592", &m_show);

    fw::scene *scene = m_manager->get_active_scene();

    if (curr != 0) {
        if (scene->get_registry().any_of<fw::component_tag>((entt::entity)curr)) {
            auto &tag = scene->get_registry().get<fw::component_tag>((entt::entity)curr);

            ImGui::InputText("Name", &tag.name);
            ImGui::Text("ID: %X", curr);
        }

        if (scene->get_registry().any_of<fw::component_camera>((entt::entity)curr)) {
            if (ImGui::CollapsingHeader("Camera")) {
                auto &cam = scene->get_registry().get<fw::component_camera>((entt::entity)curr);
                draw_camera(cam);
            }
        }

        if (scene->get_registry().any_of<fw::component_transform>((entt::entity)curr)) {
            if (ImGui::CollapsingHeader("Transform")) {
                auto &trans =
                    scene->get_registry().get<fw::component_transform>((entt::entity)curr);
                draw_transform(trans);
            }
        }

        if (scene->get_registry().any_of<fw::component_mesh>((entt::entity)curr)) {
            if (ImGui::CollapsingHeader("Mesh")) {
                auto &mesh = scene->get_registry().get<fw::component_mesh>((entt::entity)curr);
                draw_mesh(mesh);
            }
        }
        if (scene->get_registry().any_of<fw::component_material>((entt::entity)curr)) {
            if (ImGui::CollapsingHeader("Material")) {
                auto &mesh = scene->get_registry().get<fw::component_material>((entt::entity)curr);
                draw_material(scene, mesh);
            }
        }
    }

    ImGui::End();
}

component_panel::~component_panel() {}
