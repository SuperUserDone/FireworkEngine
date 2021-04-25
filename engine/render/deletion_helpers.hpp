#pragma once

#include <mutex>
#include <queue>
#include <stdint.h>

namespace blood
{

struct deletion_command
{
    enum
    {
        GPU_BUFFER,
        GPU_VAO,
        GPU_SHADER_PROGRAM
    } type;

    union
    {
        uint32_t id;
    };
};

class deletion_queue
{
public:
    static deletion_queue &get_queue()
    {
        static deletion_queue inst;
        return inst;
    }

    void push_queue(const deletion_command &cmd)
    {
        std::lock_guard<std::mutex> lck(m_queue_lock);

        m_queue.push(cmd);
    }

    deletion_command pop_queue()
    {
        std::lock_guard<std::mutex> lck(m_queue_lock);
        deletion_command cmd = m_queue.front();
        m_queue.pop();

        return cmd;
    }

    bool empty()
    {
        std::lock_guard<std::mutex> lck(m_queue_lock);
        return m_queue.empty();
    }

private:
    deletion_queue() {}

    std::queue<deletion_command> m_queue;
    std::mutex m_queue_lock;
};

} // namespace blood