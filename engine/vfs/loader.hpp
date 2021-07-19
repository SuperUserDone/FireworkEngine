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

    uint8_t load_priority = 0;
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
    bool is_empty();
    void update();

    uint64_t get_queued() { return m_queued_count; }
    uint64_t get_loaded() { return m_loaded_count; }

    void de_init();

    ~loader();

private:
    loader(uint8_t nthreads = 2);
    void worker(uint8_t id);

private:
    std::mutex m_queue_lock;
    std::queue<action> m_action_queue;

    std::mutex m_sync_queue_lock;
    std::queue<action> m_sync_action_queue;

    std::atomic_bool m_running = {true};
    std::atomic_uint64_t m_loaded_count = 0;
    std::atomic_uint64_t m_queued_count = 0;

    std::vector<std::thread> m_threads;

private:
    loader(loader const &) = delete;
    void operator=(loader const &) = delete;
};

}; // namespace fw