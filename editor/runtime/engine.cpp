#include "engine.hpp"

#include "core/logger.hpp"
#include "core/native_script.hpp"

#include "core/util.hpp"
#include "scene/component_camera.hpp"
#include "scene/component_nativescript.hpp"
#include "scene/entity.hpp"
#include <bits/stdint-uintn.h>
#include <mutex>

engine::engine() : loop("marrowlog.txt", "starting Marrow Editor")
{

    // Start threads
    m_render_thread = std::thread(&engine::render_thread, this);
}

engine::~engine()
{
    // Set stop condiditons
    m_stop_render = true;
    m_stop_update = true;

    // Waith for threads to conclude
    if (m_update_thread.joinable())
        m_update_thread.join();
    if (m_render_thread.joinable())
        m_render_thread.join();

    // Close loggger
    Logger::close_file();
}

void engine::start()
{
    if (m_update_thread.joinable())
        m_update_thread.join();
    m_stop_update = false;
    m_update_thread = std::thread(&engine::update_thread, this);
}

void engine::stop()
{
    m_stop_update = true;

    if (m_update_thread.joinable())
        m_update_thread.join();
}

void engine::single_step()
{
    // TODO
    BLOODENGINE_ASSERT(false, "Unimplemented")
}

void engine::update_thread()
{
    while (!m_stop_update)
    {
        static double frametime = 0;
        int time_target = 0;

        // Detrimine target us for tick
        if (m_tps_target != 0)
            time_target = 1000000 / trunc(m_tps_target * m_tps_strech);

        // Start clock
        uint64_t time_begin = blood::get_precise_time_us();

        // Run update
        {
            std::lock_guard<std::mutex> lock(m_scene_manager->get_active_scene().m_scene_mutex);

            // Check for swap
            m_scene_manager->swap_on_ready();

            // Update scripts
            auto view =
                m_scene_manager->get_active_scene().m_entt.view<blood::component_nativescript>();

            for (auto ent : view)
            {
                auto &script = view.get<blood::component_nativescript>(ent);

                if (!script.script)
                {
                    script.script = script.create_instance();
                    script.script->m_entity =
                        blood::entity(&m_scene_manager->get_active_scene().m_entt, ent);

                    script.script->on_start();
                }

                script.script->on_tick_update(frametime);
            }
        }

        // Calclulate frametime
        uint64_t time_end = blood::get_precise_time_us();
        int time_ellapsed = time_end - time_begin;

        // Sleep for rest of the allocated frametime
        if (!(time_ellapsed <= 0) || m_tps_target != 0)
            blood::sleep_precise(time_target - time_ellapsed);

        // Calculate updated frametime
        time_end = blood::get_precise_time_us();
        time_ellapsed = time_end - time_begin;
        frametime = time_ellapsed / 1000.f;
    }

    // Delete scripts
    auto view = m_scene_manager->get_active_scene().m_entt.view<blood::component_nativescript>();

    for (auto ent : view)
    {
        auto &script = view.get<blood::component_nativescript>(ent);

        if (script.script)
        {
            script.script->on_destroy();
            script.destroy_script(&script);
        }
    }
}

void engine::render_thread()
{
    m_renderer = std::make_shared<blood::renderer>(blood::render_settings());

    while (!m_stop_render)
    {
        static double frametime = 0;
        static uint64_t last_clean = blood::get_precise_time_us();
        int time_target = 0;

        // Detrimine target us for tick
        if (m_fps_target != 0)
            time_target = 1000000 / m_fps_target;

        // Start clock
        uint64_t time_begin = blood::get_precise_time_us();

        // Clean unused GPU data
        if (time_begin - last_clean > 500000)
        {
            m_renderer->clean();
            last_clean = blood::get_precise_time_us();
        }

        // Draw frame
        {
            std::lock_guard<std::mutex> lock(m_scene_manager->get_active_scene().m_scene_mutex);

            // Draw scene for every camera
            {
                auto view =
                    m_scene_manager->get_active_scene().m_entt.view<blood::component_camera>();

                for (auto entity : view)
                {
                    blood::component_camera &camera = view.get<blood::component_camera>(entity);

                    m_renderer->render(frametime, m_scene_manager->get_active_scene(), camera);
                }
            }

            // Update scripts
            {
                auto view = m_scene_manager->get_active_scene()
                                .m_entt.view<blood::component_nativescript>();

                for (auto ent : view)
                {
                    auto &script = view.get<blood::component_nativescript>(ent);

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
            m_stop_render = true;
            m_stop_update = true;
        }

        // Calclulate frametime
        uint64_t time_end = blood::get_precise_time_us();
        int time_ellapsed = time_end - time_begin;

        // Sleep for rest of the allocated frametime
        if (m_fps_target != 0)
            blood::sleep_precise(time_target - time_ellapsed);

        // Calculate updated frametime
        time_end = blood::get_precise_time_us();
        time_ellapsed = time_end - time_begin;
        frametime = time_ellapsed / 1000.f;
    }
}