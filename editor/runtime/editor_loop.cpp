#include "core/native_script.hpp"
#include "editor_loop.hpp"
#include "editor_ui.hpp"

editor_loop::editor_loop() : loop("fireeditlog.txt", "starting firework Editor")
{
    m_ui = fw::make_ref<editor_ui>(m_scene_manager.get());
}

editor_loop::~editor_loop() { m_stop = true; }

void editor_loop::start() { m_pause = false; }

void editor_loop::stop() { m_pause = true; }

void editor_loop::tickloop()
{

    while (!m_stop) {
        static double frametime = 0;
        fw::rate_limiter limiter(m_fps_target, &frametime);

        update(frametime);
        render(frametime);
    }
    destroy_scripts();
}

void editor_loop::render(double frametime)
{
    // Draw frame
    using namespace std::placeholders;

    m_renderer->render_editor(m_scene_manager->get_active_scene(),
                              (glm::mat4)m_edit_cam,
                              m_edit_cam.cam.get_projection(m_ui->get_size().x, m_ui->get_size().y),
                              std::bind(&editor_ui::draw, m_ui.get()),
                              std::bind(&editor_ui::get_size, m_ui.get()),
                              std::bind(&editor_ui::set_tex_id, m_ui.get(), _1)

    );

    // Update scripts
    {
        auto view = m_scene_manager->get_active_scene()->m_entt.view<fw::component_nativescript>();

        for (auto ent : view) {
            auto &script = view.get<fw::component_nativescript>(ent);

            if (script.script) {
                script.script->on_render_update(frametime);
            }
        }
    }

    // Check if should close and set appriate flags
    if (m_renderer->check_close()) {
        m_stop = true;
    }
}
