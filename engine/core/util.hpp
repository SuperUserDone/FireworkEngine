#pragma once

#include <stdint.h>

#include "logger.hpp"

#ifdef __linux__

#include <time.h>

#else

// FIXME
// Pray for wsl/cygwin/mingw for now
#include <time.h>

#endif

inline void sleep_precise(uint64_t time_us)
{
    timespec t;

    t.tv_sec = (time_us * 1000) / 10000000000;
    t.tv_nsec = (time_us * 1000) % 10000000000;

    nanosleep(&t, nullptr);
}
