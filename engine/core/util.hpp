#pragma once

#include <cstdint>
#include <stdint.h>

#include <random>

#include "logger.hpp"

#ifdef __linux__

#include <time.h>

#else

#include <windows.h> /* WinAPI */

/* Windows sleep in 100ns units */
void nanosleep(const struct timespec *time, struct timespec *);
{
    uint64_t ns = time.tv_sec * 1000000000 + time.tv_nsec;
    /* Declarations */
    HANDLE timer;     /* Timer handle */
    LARGE_INTEGER li; /* Time defintion */
    /* Create timer */
    if (!(timer = CreateWaitableTimer(NULL, TRUE, NULL)))
        return FALSE;
    /* Set timer properties */
    li.QuadPart = -ns;
    if (!SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE))
    {
        CloseHandle(timer);
        return FALSE;
    }
    /* Start & wait for timer */
    WaitForSingleObject(timer, INFINITE);
    /* Clean resources */
    CloseHandle(timer);
    /* Slept without problems */
    return TRUE;
}

#endif

namespace blood
{

inline void sleep_precise(uint64_t time_us)
{
    timespec t;

    t.tv_sec = (time_us * 1000) / 1000000000;
    t.tv_nsec = (time_us * 1000) % 1000000000;

    nanosleep(&t, nullptr);
}

inline uint64_t get_precise_time_us()
{
    timespec t;

    clock_gettime(CLOCK_REALTIME, &t);

    return (t.tv_nsec / 1000) + (t.tv_sec * 1000000);
}

inline uint64_t get_uuid()
{

    std::random_device rd;

    std::mt19937_64 e2(rd());

    std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

    uint64_t timestamp = (get_precise_time_us() >> 4) & 0xffffff;
    uint64_t random_value = dist(e2);
    uint64_t out = (timestamp << 32) | random_value;

    return out;
}

} // namespace blood