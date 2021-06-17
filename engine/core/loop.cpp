#include "imgui.h"
#include "loop.hpp"
#include "rate_limiter.hpp"

namespace blood {
loop::loop()
{
    // Init Logger
    Logger::init_logger(0, "bloodlog.txt");
    LOG_I("Starting BloodEngine")
    m_renderer = new renderer(render_settings());

    m_scene_manager = std::make_shared<scene_manager>();
}

loop::loop(const std::string &logname, const std::string &name)
{
    // Init Logger
    Logger::init_logger(0, logname);
    LOG_I(name)

    m_scene_manager = std::make_shared<scene_manager>();
}

loop::~loop()
{
    // Set stop condiditons
    m_stop = true;

    // Delete renderer
    delete m_renderer;

    // Close loggger
    Logger::close_file();
}

void loop::start() { m_pause = false; }

void loop::stop() { m_pause = true; }

void loop::tickloop()
{
    while (!m_stop) {
        static double frametime = 0;
        rate_limiter limiter(m_fps_target, &frametime);

        update(frametime);
        render(frametime);
    }
    destroy_scripts();
}

void loop::destroy_scripts()
{
    // Destroy scripts
    {
        auto view = m_scene_manager->get_active_scene()->m_entt.view<component_nativescript>();

        for (auto ent : view) {
            auto &script = view.get<component_nativescript>(ent);

            if (script.script) {
                script.script->on_destroy();
                script.destroy_script(&script);
            }
        }
    }
}

void loop::update(double frametime)
{
    // Run update
    if (!m_pause) {
        // Update scripts
        auto view = m_scene_manager->get_active_scene()->m_entt.view<component_nativescript>();

        for (auto ent : view) {
            auto &script = view.get<component_nativescript>(ent);

            if (!script.script) {
                script.script = script.create_instance();
                script.script->m_entity = entity(&m_scene_manager->get_active_scene()->m_entt, ent);

                script.script->on_start();
            }

            script.script->on_tick_update(frametime);
        }
    }
}

void loop::render(double frametime)
{
    // Draw frame

    m_renderer->render(m_scene_manager->get_active_scene());

    // Update scripts
    {
        auto view = m_scene_manager->get_active_scene()->m_entt.view<component_nativescript>();

        for (auto ent : view) {
            auto &script = view.get<component_nativescript>(ent);

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

} // namespace blood