#include "imgui.h"
#include "loop.hpp"

#include "rate_limiter.hpp"

namespace blood
{
loop::loop()
{
    // Init Logger
    Logger::init_logger(0, "bloodlog.txt");
    LOG_I("Starting BloodEngine")

    m_scene_manager = std::make_shared<scene_manager>();

    // Start threads
    m_update_thread = std::thread(&loop::update_thread, this);
    m_render_thread = std::thread(&loop::render_thread, this);
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
    m_pause_pred = false;
    m_pause.notify_all();

    // Waith for threads to conclude
    if (m_update_thread.joinable())
        m_update_thread.join();
    if (m_render_thread.joinable())
        m_render_thread.join();

    // Close loggger
    Logger::close_file();
}

void loop::start()
{
    m_pause_pred = false;
    m_pause.notify_all();
}

void loop::stop() { m_pause_pred = true; }

void loop::single_step()
{
    m_single_stepping = true;
    start();

    while (m_single_stepping)
        std::this_thread::yield();
}

void loop::update_thread()
{
    while (!m_stop)
    {
        // Wait if singlestepping/paused
        std::unique_lock<std::mutex> lck(m_stop_lck);
        m_pause.wait(lck, [this]() { return !this->m_pause_pred; });

        static double frametime = 0;
        rate_limiter limiter(m_tps_target, &frametime);

        // Singlestepping flags
        if (m_single_stepping)
        {
            m_pause_pred = true;
            m_single_stepping = false;
        }

        // Run update
        if (!m_stop)
        {
            // Update scripts
            auto view = m_scene_manager->get_active_scene()->m_entt.view<component_nativescript>();

            for (auto ent : view)
            {
                auto &script = view.get<component_nativescript>(ent);

                if (!script.script)
                {
                    script.script = script.create_instance();
                    script.script->m_entity =
                        entity(&m_scene_manager->get_active_scene()->m_entt, ent);

                    script.script->on_start();
                }

                script.script->on_tick_update(frametime);
            }
        }
    }

    // Delete scripts
    auto view = m_scene_manager->get_active_scene()->m_entt.view<component_nativescript>();

    for (auto ent : view)
    {
        auto &script = view.get<component_nativescript>(ent);

        if (script.script)
        {
            script.script->on_destroy();
            script.destroy_script(&script);
        }
    }
}

void loop::render_thread()
{
    m_renderer = new renderer(render_settings());

    while (!m_stop)
    {
        static double frametime = 0;

        rate_limiter limiter(m_fps_target, &frametime);

        // Draw frame
        {
            // Draw scene for every camera
            {
                auto view = m_scene_manager->get_active_scene()
                                ->m_entt.view<component_camera, component_transform>();

                for (auto entity : view)
                {
                    auto [camera, trans] = view.get<component_camera, component_transform>(entity);

                    m_renderer->render(
                        frametime, m_scene_manager->get_active_scene(), camera, (glm::mat4)trans);
                }

                m_renderer->finish_render();
            }

            // Update scripts
            {
                auto view =
                    m_scene_manager->get_active_scene()->m_entt.view<component_nativescript>();

                for (auto ent : view)
                {
                    auto &script = view.get<component_nativescript>(ent);

                    if (script.script)
                    {
                        script.script->on_render_update(frametime);
                    }
                }
            }
        }

        // Process events
        m_renderer->process_events();

        // Check if should close and set appriate flags
        if (m_renderer->check_close())
        {
            m_stop = true;
            m_pause_pred = false;
            m_pause.notify_all();
        }
    }
    delete m_renderer;
}

} // namespace blood