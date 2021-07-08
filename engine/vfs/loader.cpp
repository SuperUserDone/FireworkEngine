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

loader_stats loader::get_statistics() { return m_stats; }

void loader::worker(uint8_t id)
{
    // TODO stats
    while (m_running) {
        static int rate = 0;
        static double time = 0;

        action act;

        {
            std::lock_guard<spinlock> lock(m_queue_lock);

            if (m_action_queue.size() > 0) {
                rate = 0;
                act = m_action_queue.front();
                m_action_queue.pop();
            } else {
                rate = 1;
            }
        }

        act.async_action();

        {
            std::lock_guard<spinlock> lock(m_sync_queue_lock);
            m_sync_action_queue.push(act);
        }

        rate_limiter(rate, &time);
    }
}

loader::~loader()
{
    m_running = false;
    for (auto &thread : m_threads) {
        thread.join();
    }
}

} // namespace fw