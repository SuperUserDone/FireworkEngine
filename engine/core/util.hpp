#pragma once

#include <cstdint>
#include <stdint.h>

#include <random>

#include "logger.hpp"



namespace blood
{

#ifdef __linux__

#include <time.h>

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

#include <windows.h>

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