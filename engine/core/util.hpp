#pragma once

#include <cstdint>
#include <stdint.h>

#include <random>

#include "logger.hpp"

#ifdef __linux__
#include <time.h>
#else
#include <windows.h>
#endif

namespace blood
{

#ifdef __linux__

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

#else

inline void sleep_precise(uint64_t time_us)
{
    uint64_t ns = time_us * 1000;

    HANDLE timer;
    LARGE_INTEGER li;

    if (!(timer = CreateWaitableTimer(NULL, TRUE, NULL)))
        return;

    li.QuadPart = -ns;
    if (!SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE))
    {
        CloseHandle(timer);
        return;
    }

    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}

inline uint64_t get_precise_time_us()
{
    LARGE_INTEGER StartingTime;
    LARGE_INTEGER Frequency;

    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&StartingTime);

    StartingTime.QuadPart *= 1000000;
    StartingTime.QuadPart /= Frequency.QuadPart;

    return StartingTime.QuadPart;
}

#endif

inline uint32_t get_uuid()
{
    std::random_device rd;

    std::mt19937_64 e2(rd());

    std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

    uint32_t timestamp = get_precise_time_us();
    uint32_t random_value = dist(e2);
    uint32_t out = timestamp ^ random_value;

    return out;
}

} // namespace blood