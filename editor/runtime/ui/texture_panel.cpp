#include "core/types/image2d.hpp"
#include "parts/imgui_utils.hpp"
#include "render/fonts/codepoint.hpp"
#include "texture_panel.hpp"
#include "vfs/loader.hpp"

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
    static std::vector<std::string> selections;
    static std::vector<std::pair<uint32_t, std::string>> thumbs;

    auto &textures = scene->get_textures();

    thumbs.clear();

    for (auto &&tex : textures) {
        auto &&[key, val] = tex;

        thumbs.push_back({*(uint32_t *)val->get_id(), key});
    }

    draw_grid(thumbs, selections, m_size);

    ImGui::Separator();

    if (ImGui::Button("Add")) m_open.show();
    ImGui::SameLine();
    if (ImGui::Button("Delete")) {
        for (auto &a : selections) {
            scene->get_textures().erase(a);
            scene->set_dirty();
        }
        selections.clear();
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(256.f);
    ImGui::SliderFloat("##Size", &m_size, 64.f, 256.f, "Thumb size: %.0f");

    ImGui::End();

    m_open_error.update();

    if (m_open.update()) {
        fw::action act;
        std::string path = "root://" + m_name + ".fwtex";
        std::string name = m_name;

        fw::ref<fw::image2d> img = fw::make_ref<fw::image2d>();

        act.async_action = [path, img, this]() {
            if (!img->load_from_file(path)) {
                this->m_open_error.show();
                return false;
            }
            return true;
        };

        act.post_sync_action = [scene, name, img, path]() {
            auto tex = img->get_as_texture();
            tex->m_path = path;
            scene->get_textures()[name] = tex;
            return true;
        };
        act.load_priority = 16;

        fw::loader::get_instance().queue_action(act);
    }
}

texture_panel::~texture_panel() {}