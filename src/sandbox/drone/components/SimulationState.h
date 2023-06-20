#pragma once
#include <chrono>
#include <cstdint>
#include <fmt/format.h>

struct Time
{
    uint64_t milliseconds_accumulator{};
    uint64_t seconds{};
    void     add(std::chrono::milliseconds ms, float time_scale)
    {
        milliseconds_accumulator += ms.count() * time_scale;
        seconds += milliseconds_accumulator / 1000;
        milliseconds_accumulator %= 1000;
    }

    std::string format() const
    {
        auto secs   = std::chrono::seconds(seconds);
        auto years  = std::chrono::duration_cast<std::chrono::years>(secs);
        auto months = std::chrono::duration_cast<std::chrono::months>(secs) % std::chrono::duration_cast<std::chrono::months>(std::chrono::years(1)).count();
        auto days   = std::chrono::duration_cast<std::chrono::days>(secs) % std::chrono::duration_cast<std::chrono::days>(std::chrono::months(1)).count();
        auto hours  = std::chrono::duration_cast<std::chrono::hours>(secs) % std::chrono::duration_cast<std::chrono::hours>(std::chrono::days(1)).count();
        return fmt::format("{}.{:02}.{:02}.{:05}", years.count(), months.count(), days.count(), secs.count() % std::chrono::duration_cast<std::chrono::seconds>(std::chrono::days(1)).count());
    }
};
struct SimulationState
{
    Time time;
};