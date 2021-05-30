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
    m_renderer = new blood::renderer(blood::render_settings());

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
    // Draw scene for every camera
    {
        m_renderer->clear_fb(&blood::DEFAULT_FRAMEBUFFER);

        static blood::framebuffer fb;

        static glm::uvec2 size = {0, 0};

        m_renderer->clear_fb(&fb);

        m_renderer->render(
            frametime, m_scene_manager->get_active_scene(), m_edit_cam.cam, m_edit_cam, &fb, size);

        blood::framebuffer fb_cpy = fb;
        glm::uvec2 size_cpy = size;

        m_renderer->render_imgui(nullptr, [&]() {
            return draw_editor_ui(this->m_scene_manager.get(), fb_cpy, size_cpy);
        });

        size = size_cpy;

        m_renderer->finish_render();
    }

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

    // Process events
    m_renderer->process_events();

    // Check if should close and set appriate flags
    if (m_renderer->check_close()) {
        m_stop = true;
    }
}
