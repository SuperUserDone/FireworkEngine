#include "core/logger.hpp"
#include "core/native_script.hpp"
#include "editor_loop.hpp"
#include "editor_ui.hpp"
#include "scene/entity.hpp"

#include <mutex>

editor_loop::editor_loop() : loop("marrowlog.txt", "starting Marrow Editor") {}

editor_loop::~editor_loop() { m_stop = true; }

void editor_loop::start() { m_pause = false; }

void editor_loop::stop() { m_pause = true; }

void editor_loop::tickloop()
{
    m_renderer = new blood::renderer(m_settings.renderer);

    while (!m_stop) {
        static double frametime = 0;
        blood::rate_limiter limiter(m_fps_target, &frametime);

        update(frametime);
        render(frametime);
    }
    destroy_scripts();

    // Delete renderer
    delete m_renderer;
}

void editor_loop::render(double frametime)
{
    // Draw frame
    m_renderer->render_editor(m_scene_manager->get_active_scene());

    // Update scripts
    {
        auto view =
            m_scene_manager->get_active_scene()->m_entt.view<blood::component_nativescript>();

        for (auto ent : view) {
            auto &script = view.get<blood::component_nativescript>(ent);

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
