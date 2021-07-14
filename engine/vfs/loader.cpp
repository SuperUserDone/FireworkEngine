#include "loader.hpp"

namespace fw {
void loader::queue_action(const action &action)
{
    std::lock_guard<spinlock> lock(m_queue_lock);
    m_action_queue.push(action);
}

loader::loader(uint8_t nthreads)
{
    for (int i = 0; i < nthreads; i++) {
        m_threads.push_back(std::thread(&loader::worker, this, i));
    }
}

void loader::wait()
{
    bool flag = true;

    while (flag) {
        {
            std::lock_guard<spinlock> lock(m_queue_lock);
            if (m_action_queue.size() == 0) flag = false;
        }

        sleep_precise(100 * 1000);
    }
}

void loader::update()
{
    action act;
    {
        std::lock_guard<spinlock> lock(m_sync_queue_lock);

        while (m_sync_action_queue.size() > 0) {
            act = m_sync_action_queue.front();
            m_sync_action_queue.pop();
            act.post_sync_action();
        }
    }
}

void loader::de_init()
{
    LOG_I("Stopping loader");
    m_running = false;

    int thread_n = 0;

    for (auto &thread : m_threads) {
        LOG_DF("Thread number {} shutting down...", thread_n);
        thread.join();
        LOG_DF("Thread number {} shutdown done!", thread_n++);
    }

    m_threads.clear();
}

loader_stats loader::get_statistics() { return m_stats; }

void loader::worker(uint8_t id)
{
    int rate = 0;
    double time = 0;

    LOG_DF("Starting thread {}", id);

    // TODO stats
    while (m_running) {
        rate_limiter rate_limt(rate, &time);

        LOG_DF("Load rate thread {}: {}", id, time);

        action act;

        {
            std::lock_guard<spinlock> lock(m_queue_lock);
            std::lock_guard<spinlock> lock_sync(m_sync_queue_lock);

            if (m_action_queue.size() > 0) {
                rate = 0;
                act = m_action_queue.front();
                m_action_queue.pop();
                act.async_action();
                m_sync_action_queue.push(act);
            } else {
                rate = 1;
            }
        }
    }
}

loader::~loader() {}

} // namespace fw