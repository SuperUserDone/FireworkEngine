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
class loop
{
public:
    loop();

    virtual ~loop();

    std::weak_ptr<renderer> get_renderer() { return m_renderer; }
    std::weak_ptr<scene_manager> get_scenemanager() { return m_scene_manager; }
    // std::weak_ptr<void> get_physicsmanager() { return std::make_shared<void>(nullptr); }

    virtual void block()
    {
        if (m_render_thread.joinable())
            m_render_thread.join();
    }

    virtual void start();
    virtual void stop();

    virtual void single_step();

    void set_tps_limit(uint16_t limit) { m_tps_target = limit; }
    void set_fps_limit(uint16_t limit) { m_fps_target = limit; }

    uint16_t get_fps_limit() { return m_fps_target; }
    uint16_t get_tps_limit() { return m_tps_target; }

private:
    void update_thread();
    void render_thread();

    std::thread m_update_thread;
    std::thread m_render_thread;

    std::mutex m_stop_lck;
    std::condition_variable m_pause;
    std::atomic_bool m_pause_pred{true};
    std::atomic_bool m_single_stepping{false};

    std::atomic_bool m_stop{false};

protected:
    uint16_t m_fps_target = 60;
    uint16_t m_tps_target = 60;

    std::shared_ptr<renderer> m_renderer = nullptr;
    std::shared_ptr<scene_manager> m_scene_manager = nullptr;

private:
    loop(const loop &other) = delete;
    loop(loop &&other) = delete;
    loop &operator=(loop &&other) = delete;
    loop &operator=(loop other) = delete;
};

} // namespace blood
