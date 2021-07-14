#pragma once

#include "logger.hpp"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <random>
#include <stdint.h>
#include <thread>

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

inline void sleep_precise(uint64_t time_us)
{
    timespec t;

    t.tv_sec = (time_us * 1000) / 1000000000;
    t.tv_nsec = (time_us * 1000) % 1000000000;

    nanosleep(&t, nullptr);
}

#else

inline void sleep_precise(uint64_t time_us)
{
    std::this_thread::sleep_for(std::chrono::microseconds(time_us));
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