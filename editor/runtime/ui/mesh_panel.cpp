#include "mesh_panel.hpp"
#include "parts/imgui_utils.hpp"
#include "vfs/loader.hpp"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

mesh_panel::mesh_panel(fw::scene_manager *man)
    : m_man(man), m_open("Open Mesh", "Open file ", &m_name),
      m_open_error("Mesh open error", "Could not open mesh file!")
{}

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

    filtered = draw_grid(
        "Mesh Table", "MESH", thumbs, selections, m_size, filter, [](const std::string &name) {
            LOG_DF("Edit mesh: {}", name);
        });

    ImGui::Separator();

    if (ImGui::Button("\uf71f\tImport")) m_open.show();
    ImGui::SameLine();
    if (ImGui::Button("\ufae7\tDelete")) {
        for (auto &a : selections) {
            scene->get_meshes().erase(a);
            scene->set_dirty();
        }
        selections.clear();
    }

    ImGui::SameLine();
    ImGui::End();

    m_open_error.update();

    if (m_open.update()) {
        fw::action act;
        std::string path = "root://" + m_name + ".fwmesh";
        std::string name = m_name;

        fw::ref<fw::mesh> mesh = fw::make_ref<fw::mesh>();

        act.async_action = [path, mesh, this]() {
            if (!mesh->load_from_file(path)) {
                this->m_open_error.show();
                return false;
            }
            return true;
        };

        act.post_sync_action = [scene, name, mesh, path]() {
            mesh->m_path = path;
            scene->get_meshes()[name] = mesh;
            return true;
        };
        act.load_priority = 16;

        fw::loader::get_instance().queue_action(act);
    }
}

mesh_panel::~mesh_panel() {}
