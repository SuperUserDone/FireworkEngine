#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <stdint.h>
#include <thread>

#include "render/renderer.hpp"
#include "scene/scene_manager.hpp"

namespace blood
{
/**
 * \brief The main game loop
 *
 */
class loop
{
public:
    /**
     * \brief Construct the loop
     *
     */
    loop();
    loop(const std::string &logname, const std::string &name);

    /**
     * \brief Destroy the loop
     *
     */
    virtual ~loop();

    /**
     * \brief Get the renderer
     *
     * \return std::weak_ptr<renderer> refrence to blood::renderer
     */
    std::weak_ptr<renderer> get_renderer() { return m_renderer; }

    /**
     * \brief Get the scenemanager
     *
     * \return std::weak_ptr<scene_manager> refrence to blood::scene_manager
     */
    std::weak_ptr<scene_manager> get_scenemanager() { return m_scene_manager; }
    // std::weak_ptr<void> get_physicsmanager() { return std::make_shared<void>(nullptr); }

    /**
     * \brief Block until render loop done
     *
     */
    virtual void block()
    {
        if (m_render_thread.joinable())
            m_render_thread.join();
    }

    /**
     * \brief Start the update loop
     *
     */
    virtual void start();

    /**
     * \brief Pause the update loop
     *
     */
    virtual void stop();

    /**
     * \brief Single step the update loop
     *
     * Pauses the loop if running
     *
     */
    virtual void single_step();

    /**
     * \brief Set the tps limit for update loop
     *
     * \param limit The target TPS
     */
    void set_tps_limit(uint16_t limit) { m_tps_target = limit; }

    /**
     * \brief Set the fps limit for the render loop
     *
     * \param limit The target FPS
     */
    void set_fps_limit(uint16_t limit) { m_fps_target = limit; }

    /**
     * \brief Get the fps limit
     *
     * \return uint16_t the limit
     */
    uint16_t get_fps_limit() { return m_fps_target; }

    /**
     * \brief Get the tps limit
     *
     * \return uint16_t The limit
     */
    uint16_t get_tps_limit() { return m_tps_target; }

private:
    std::thread m_update_thread;
    std::thread m_render_thread;

    std::mutex m_stop_lck;
    std::condition_variable m_pause;
    std::atomic_bool m_pause_pred{true};
    std::atomic_bool m_single_stepping{false};

    std::atomic_bool m_stop{false};

protected:
    virtual void update_thread();
    virtual void render_thread();

    uint16_t m_fps_target = 60;
    uint16_t m_tps_target = 60;
    float m_tps_strech = 1.0f;

    std::shared_ptr<renderer> m_renderer = nullptr;
    std::shared_ptr<scene_manager> m_scene_manager = nullptr;

private:
    loop(const loop &other) = delete;
    loop(loop &&other) = delete;
    loop &operator=(loop &&other) = delete;
    loop &operator=(loop other) = delete;
};

} // namespace blood
