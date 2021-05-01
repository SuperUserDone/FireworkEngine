#include "editor_loop.hpp"

#include <mutex>

#include "core/logger.hpp"
#include "core/native_script.hpp"
#include "core/util.hpp"
#include "editor_ui.hpp"
#include "scene/entity.hpp"

editor_loop::editor_loop() : loop("marrowlog.txt", "starting Marrow Editor")
{

    // Start threads
    m_render_thread = std::thread(&editor_loop::render_thread, this);
}

editor_loop::~editor_loop()
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

void editor_loop::start()
{
    if (m_update_thread.joinable())
        m_update_thread.join();
    m_stop_update = false;
    m_update_thread = std::thread(&editor_loop::update_thread, this);
}

void editor_loop::stop()
{
    m_stop_update = true;

    if (m_update_thread.joinable())
        m_update_thread.join();
}

void editor_loop::single_step()
{
    // TODO
    BLOODENGINE_ASSERT(false, "Unimplemented")
}

void editor_loop::update_thread()
{
    while (!m_stop_update)
    {
        static double frametime = 0;
        blood::rate_limiter limiter(m_tps_target, &frametime);

        // Run update
        {
            // Update scripts
            auto view =
                m_scene_manager->get_active_scene()->m_entt.view<blood::component_nativescript>();

            for (auto ent : view)
            {
                auto &script = view.get<blood::component_nativescript>(ent);

                if (!script.script)
                {
                    script.script = script.create_instance();
                    script.script->m_entity =
                        blood::entity(&m_scene_manager->get_active_scene()->m_entt, ent);

                    script.script->on_start();
                }

                script.script->on_tick_update(frametime);
            }
        }
    }

    // Delete scripts
    auto view = m_scene_manager->get_active_scene()->m_entt.view<blood::component_nativescript>();

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

void editor_loop::render_thread()
{
    m_renderer = new blood::renderer(blood::render_settings());

    static double frametime = 0;
    blood::rate_limiter limiter(m_fps_target, &frametime);

    while (!m_stop_render)
    {
        static double frametime = 0;

        // Draw frame
        {
            // Draw scene for every camera
            if (!m_stop_update)
            {
                auto view =
                    m_scene_manager->get_active_scene()
                        ->m_entt.view<blood::component_camera, blood::component_transform>();

                for (auto entity : view)
                {
                    auto [camera, trans] =
                        view.get<blood::component_camera, blood::component_transform>(entity);

                    m_renderer->render(
                        frametime, m_scene_manager->get_active_scene(), camera, (glm::mat4)trans);
                }

                m_renderer->finish_render();
            }
            else
            {
                m_renderer->clear_fb(&blood::DEFAULT_FRAMEBUFFER);

                static blood::framebuffer fb;

                static glm::uvec2 size = {0, 0};

                m_renderer->clear_fb(&fb);

                m_renderer->render(frametime,
                                   m_scene_manager->get_active_scene(),
                                   m_edit_cam.cam,
                                   m_edit_cam,
                                   &fb,
                                   size);

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
                auto view = m_scene_manager->get_active_scene()
                                ->m_entt.view<blood::component_nativescript>();

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
    }
    delete m_renderer;
}
