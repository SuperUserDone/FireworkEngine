#include "loop.hpp"

#include "logger.hpp"

#include "util.hpp"

namespace blood
{
loop::loop()
{
    Logger::init_logger(0, "bloodlog.txt");
    LOG_I("Starting BloodEngine")

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

    Logger::close_file();
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
        static double frametime = 0;
        int time_target = 0;

        if (m_tps_target != 0)
            time_target = 1000000 / m_tps_target;

        uint64_t time_begin = get_precise_time_us();

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

        uint64_t time_end = get_precise_time_us();
        int time_ellapsed = time_end - time_begin;

        if (!(time_ellapsed <= 0) || m_tps_target != 0)
            sleep_precise(time_target - time_ellapsed);

        time_end = get_precise_time_us();
        time_ellapsed = time_end - time_begin;
        frametime = time_ellapsed / 1000.f;
    }
}

void loop::render_thread()
{
    m_renderer = std::make_shared<renderer>(render_settings());

    while (!m_stop)
    {
        static double frametime = 0;
        int time_target = 0;

        if (m_tps_target != 0)
            time_target = 1000000 / m_tps_target;
        uint64_t time_begin = get_precise_time_us();

        m_renderer->render(frametime);
        m_renderer->process_events();

        if (m_renderer->check_close())
        {
            m_stop = true;
            m_pause_pred = false;
            m_pause.notify_all();
        }

        uint64_t time_end = get_precise_time_us();
        int time_ellapsed = time_end - time_begin;
        if (m_fps_target != 0)
            sleep_precise(time_target - time_ellapsed);

        time_end = get_precise_time_us();
        time_ellapsed = time_end - time_begin;
        frametime = time_ellapsed / 1000.f;
    }
}

} // namespace blood