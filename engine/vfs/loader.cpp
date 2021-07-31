#include "loader.hpp"

namespace fw {
void loader::queue_action(const action &action)
{
    std::lock_guard<std::mutex> lock(m_queue_lock);
    m_action_queue.push(action);
    m_queued_count++;
}

loader::loader(uint8_t nthreads)
{
    for (int i = 0; i < nthreads; i++) {
        m_threads.push_back(std::thread(&loader::worker, this, i));
    }
}

bool loader::is_empty() { return m_queued_count > m_loaded_count; }

void loader::update()
{
    action act;
    {
        std::lock_guard<std::mutex> lock(m_sync_queue_lock);

        while (m_sync_action_queue.size() > 0) {
            act = m_sync_action_queue.front();
            m_sync_action_queue.pop();
            act.post_sync_action();
            m_loaded_count++;
            LOG_DF("Loaded: {}/{} FAILED: {}",
                   m_loaded_count,
                   m_queued_count - m_failed_count,
                   m_failed_count);
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

void loader::worker(uint8_t id)
{
    int rate = 0;
    double time = 0;

    LOG_DF("Starting thread {}", id);

    // TODO stats
    while (m_running) {
        rate_limiter rate_limt(rate, &time);

        bool flag = false;
        action act;

        rate = 1;

        {
            std::lock_guard<std::mutex> lock(m_queue_lock);
            while (m_action_queue.size() > 0) {

                rate = 0;

                act = m_action_queue.front();
                m_action_queue.pop();
                if (act.load_priority > 0) {
                    act.load_priority--;
                    m_action_queue.push(act);
                } else {
                    flag = true;
                    break;
                }
            }
        }

        if (!act.async_action()) {
            m_failed_count++;
            LOG_EF("Fail to load, see prev log entries. (Total failed = {})", m_failed_count);
            continue;
        }
        if (flag) {
            std::lock_guard<std::mutex> lock_sync(m_sync_queue_lock);
            m_sync_action_queue.push(act);
        }
    }

    LOG_DF("Thread {} returning", id);
}

loader::~loader() {}

} // namespace fw