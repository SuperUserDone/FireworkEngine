#pragma once

#include "logger.hpp"

#include <atomic>
#include <cstdint>
#include <random>
#include <stdint.h>

#ifdef __linux__
#include <time.h>
#else
#include <windows.h>
#endif

namespace fw {

template <typename T>
using ref = std::shared_ptr<T>;

template <typename T, typename... Args>
constexpr ref<T> make_ref(Args &&...args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

#ifdef __linux__

inline uint64_t get_precise_time_us()
{
    timespec t;

    clock_gettime(CLOCK_REALTIME, &t);

    return (t.tv_nsec / 1000) + (t.tv_sec * 1000000);
}

#else

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the w64 mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */

#include "pthread.h"
#include "pthread_time.h"
#include "winpthread_internal.h"

#include <errno.h>
#include <time.h>
#include <windows.h>

#define POW10_3         1000
#define POW10_4         10000
#define POW10_6         1000000
#define POW10_9         1000000000
#define MAX_SLEEP_IN_MS 4294967294UL

/**
 * Sleep for the specified time.
 * @param  request The desired amount of time to sleep.
 * @param  remain The remain amount of time to sleep.
 * @return If the function succeeds, the return value is 0.
 *         If the function fails, the return value is -1,
 *         with errno set to indicate the error.
 */
int nanosleep(const struct timespec *request, struct timespec *remain)
{
    unsigned long ms, rc = 0;
    unsigned __int64 u64, want, real;

    union
    {
        unsigned __int64 ns100;
        FILETIME ft;
    } _start, _end;

    if (request->tv_sec < 0 || request->tv_nsec < 0 || request->tv_nsec >= POW10_9) {
        errno = EINVAL;
        return -1;
    }

    if (remain != NULL) GetSystemTimeAsFileTime(&_start.ft);

    want = u64 = request->tv_sec * POW10_3 + request->tv_nsec / POW10_6;
    while (u64 > 0 && rc == 0) {
        if (u64 >= MAX_SLEEP_IN_MS)
            ms = MAX_SLEEP_IN_MS;
        else
            ms = (unsigned long)u64;

        u64 -= ms;
        rc = pthread_delay_np_ms(ms);
    }

    if (rc != 0) { /* WAIT_IO_COMPLETION (192) */
        if (remain != NULL) {
            GetSystemTimeAsFileTime(&_end.ft);
            real = (_end.ns100 - _start.ns100) / POW10_4;

            if (real >= want)
                u64 = 0;
            else
                u64 = want - real;

            remain->tv_sec = u64 / POW10_3;
            remain->tv_nsec = (long)(u64 % POW10_3) * POW10_6;
        }

        errno = EINTR;
        return -1;
    }

    return 0;
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

inline void sleep_precise(uint64_t time_us)
{
    timespec t;

    t.tv_sec = (time_us * 1000) / 1000000000;
    t.tv_nsec = (time_us * 1000) % 1000000000;

    nanosleep(&t, nullptr);
}

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

class spinlock
{
private:
    std::atomic_bool lock_ = {0};

public:
    void lock() noexcept
    {
        for (;;) {
            // Optimistically assume the lock is free on the first try
            if (!lock_.exchange(true, std::memory_order_acquire)) {
                return;
            }
            // Wait for lock to be released without generating cache misses
            while (lock_.load(std::memory_order_relaxed)) {
#ifdef _MSC_VER
#elif __clang__ || __GNUC__
                __builtin_ia32_pause();
#endif
            }
        }
    }

    bool try_lock() noexcept
    {
        // First do a relaxed load to check if lock is free in order to prevent
        // unnecessary cache misses if someone does while(!try_lock())
        return !lock_.load(std::memory_order_relaxed) &&
               !lock_.exchange(true, std::memory_order_acquire);
    }

    void unlock() noexcept { lock_.store(false, std::memory_order_release); }
};

} // namespace fw