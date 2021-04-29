#include "imgui.h"
#include "loop.hpp"
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
        static double frametime = 0;
        int time_target = 0;

        // Detrimine target us for tick
        if (m_tps_target != 0)
            time_target = 1000000 / trunc(m_tps_target * m_tps_strech);

        // Start clock
        uint64_t time_begin = get_precise_time_us();

        // Wait if singlestepping/paused
        std::unique_lock<std::mutex> lck(m_stop_lck);
        m_pause.wait(lck, [this]() { return !this->m_pause_pred; });

        // Singlestepping flags
        if (m_single_stepping)
        {
            m_pause_pred = true;
            m_single_stepping = false;
        }

        // Run update
        if (!m_stop)
        {
            std::lock_guard<std::mutex> lock(m_scene_manager->get_active_scene()->m_scene_mutex);

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

        // Calclulate frametime
        uint64_t time_end = get_precise_time_us();
        uint64_t time_ellapsed = time_end - time_begin;

        // Sleep for rest of the allocated frametime
        if (time_target > time_ellapsed && m_tps_target != 0)
            sleep_precise(time_target - time_ellapsed);

        // Calculate updated frametime
        time_end = get_precise_time_us();
        time_ellapsed = time_end - time_begin;
        frametime = time_ellapsed / 1000.f;
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
    m_renderer = std::make_shared<renderer>(render_settings());

    while (!m_stop)
    {
        static double frametime = 0;
        static uint64_t last_clean = get_precise_time_us();
        int time_target = 0;

        // Detrimine target us for tick
        if (m_fps_target != 0)
            time_target = 1000000 / m_fps_target;

        // Start clock
        uint64_t time_begin = get_precise_time_us();

        // Clean unused GPU data
        if (time_begin - last_clean > 500000)
        {
            m_renderer->clean();
            last_clean = get_precise_time_us();
        }

        // Draw frame
        {
            std::lock_guard<std::mutex> lock(m_scene_manager->get_active_scene()->m_scene_mutex);

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

        // Calclulate frametime
        uint64_t time_end = get_precise_time_us();
        uint64_t time_ellapsed = time_end - time_begin;

        // Sleep for rest of the allocated frametime
        if (time_target > time_ellapsed && m_tps_target != 0)
            sleep_precise(time_target - time_ellapsed);

        // Calculate updated frametime
        time_end = get_precise_time_us();
        time_ellapsed = time_end - time_begin;
        frametime = time_ellapsed / 1000.f;
    }
}

} // namespace blood