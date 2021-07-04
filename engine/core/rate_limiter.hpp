#pragma once

#include "util.hpp"

namespace fw {
class rate_limiter
{
public:
    rate_limiter(uint16_t fps, double *time, float strech = 0.0f)
    {
        m_time = time;
        m_time_begin = get_precise_time_us();
        if (fps != 0) m_time_target = 1000000 / fps;
    }
    ~rate_limiter()
    {
        // Calclulate frametime
        uint64_t time_end = get_precise_time_us();
        uint64_t time_ellapsed = time_end - m_time_begin;

        // Sleep for rest of the allocated frametime
        if (m_time_target > time_ellapsed && m_time_target != 0)
            sleep_precise(m_time_target - time_ellapsed);

        // Calculate updated frametime
        time_end = get_precise_time_us();
        time_ellapsed = time_end - m_time_begin;
        *m_time = time_ellapsed / 1000.f;
    }

private:
    uint64_t m_time_begin;
    double *m_time;
    int m_time_target = 0;
};

} // namespace fw
