#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <stdint.h>
#include <thread>

#include "render/renderer.hpp"

namespace blood
{
class loop
{
public:
    loop();
    virtual ~loop();

    std::weak_ptr<renderer> get_renderer() { return m_renderer; }
    // std::weak_ptr<void> get_scenemanager() { return std::make_shared<void>(nullptr); }
    // std::weak_ptr<void> get_physicsmanager() { return std::make_shared<void>(nullptr); }

    virtual void block()
    {
        if (m_render_thread.joinable())
            m_render_thread.join();
    }

    virtual void start();
    virtual void stop();

    virtual void single_step();

    void set_tps_limit(uint16_t limit) { m_tps = limit; }
    void set_fps_limit(uint16_t limit) { m_fps = limit; }

    uint16_t get_fps_limit() { return m_fps; }
    uint16_t get_tps_limit() { return m_tps; }

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
    uint16_t m_fps = 60;
    uint16_t m_tps = 60;

    std::shared_ptr<renderer> m_renderer = nullptr;
};

} // namespace blood
