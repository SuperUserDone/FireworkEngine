#pragma once

#include <bits/stdint-uintn.h>
#include <cstdint>
#include <stdint.h>

#include <random>

#include "logger.hpp"

#ifdef __linux__

#include <time.h>

#else

// FIXME
// Pray for wsl/cygwin/mingw for now
#include <time.h>

#endif

namespace blood
{

inline void sleep_precise(uint64_t time_us)
{
    timespec t;

    t.tv_sec = (time_us * 1000) / 10000000000;
    t.tv_nsec = (time_us * 1000) % 10000000000;

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