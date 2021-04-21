#include "loop.hpp"
namespace blood
{
loop::loop()
{
    m_update_thread = std::thread(&loop::update_thread, this);
    m_render_thread = std::thread(&loop::render_thread, this);
}

loop::~loop()
{
    m_stop = true;
    m_pause_pred = false;
    m_pause.notify_all();
    if (m_update_thread.joinable())
        m_update_thread.join();
    if (m_render_thread.joinable())
        m_render_thread.join();
}

void loop::start()
{
    m_pause_pred = false;
    m_pause.notify_all();
}

void loop::stop() { m_pause_pred = true; }

void loop::single_step()
{
    m_single_stepping = true;
    start();

    while (m_single_stepping)
        std::this_thread::yield();
}

void loop::update_thread()
{
    // TODO init code

    while (!m_stop)
    {
        // TODO throttle

        std::unique_lock<std::mutex> lck(m_stop_lck);
        m_pause.wait(lck, [this]() { return !this->m_pause_pred; });

        if (m_single_stepping)
        {
            m_pause_pred = true;
            m_single_stepping = false;
        }

        if (!m_stop)
        {
            // TODO
        }
    }
}

void loop::render_thread()
{
    m_renderer = std::make_shared<renderer>(render_settings());

    while (!m_stop)
    {
        // TODO throttle

        m_renderer->render();
        m_renderer->process_events();

        if (m_renderer->check_close())
        {
            m_stop = true;
            m_pause_pred = false;
            m_pause.notify_all();
        }
    }
}

} // namespace blood