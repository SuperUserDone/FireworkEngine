#pragma once

#include "core/rate_limiter.hpp"
#include "core/util.hpp"

#include <atomic>
#include <queue>
#include <stdint.h>
#include <thread>

namespace fw {

struct action
{
    std::function<void(void)> async_action = []() {};
    std::function<void(void)> post_sync_action = []() {};
};

struct loader_stats
{
    std::vector<uint32_t> thread_rates;
};

class loader
{
public:
    static loader &get_instance()
    {
        static loader instance;
        return instance;
    }

    void queue_action(const action &action);
    void wait();
    void update();

    void de_init();

    loader_stats get_statistics();

    ~loader();

private:
    loader(uint8_t nthreads = 2);
    void worker(uint8_t id);

private:
    loader_stats m_stats;

    spinlock m_queue_lock;
    std::queue<action> m_action_queue;

    spinlock m_sync_queue_lock;
    std::queue<action> m_sync_action_queue;

    std::atomic_bool m_running = {true};

    std::vector<std::thread> m_threads;

private:
    loader(loader const &) = delete;
    void operator=(loader const &) = delete;
};

}; // namespace fw