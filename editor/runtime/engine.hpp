#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <stdint.h>
#include <thread>

#include "core/loop.hpp"
#include "render/renderer.hpp"
#include "scene/scene_manager.hpp"

class engine : public blood::loop
{
public:
    engine();

    virtual ~engine() override;

    virtual void block() override
    {
        if (m_render_thread.joinable())
            m_render_thread.join();
    }

    virtual void start() override;
    virtual void stop() override;
    virtual void single_step() override;

protected:
    virtual void update_thread() override;
    virtual void render_thread() override;

private:
    std::thread m_update_thread;
    std::thread m_render_thread;

    std::atomic_bool m_stop_update{false};
    std::atomic_bool m_stop_render{false};

private:
    engine(const engine &other) = delete;
    engine(engine &&other) = delete;
    engine &operator=(engine &&other) = delete;
    engine &operator=(engine other) = delete;
};
