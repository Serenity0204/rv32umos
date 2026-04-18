#pragma once
#include "ScopedCriticalSection.hpp"
#include <chrono>
#include <functional>
#include <queue>
#include <unordered_set>
#include <vector>

using TimerCallback = std::function<void()>;

struct TimerEvent
{
    std::chrono::steady_clock::time_point expiration;
    uint64_t id;

    bool operator>(const TimerEvent& other) const
    {
        return this->expiration > other.expiration;
    }
};

struct TimerInfo
{
    std::chrono::steady_clock::time_point expiration;
    TimerCallback callback;
    bool active;
};

class SoftwareTimer
{
private:
    std::unordered_map<uint64_t, TimerInfo> activeTimers;
    std::priority_queue<TimerEvent, std::vector<TimerEvent>, std::greater<TimerEvent>> queue;
    uint64_t nextId = 0;

public:
    SoftwareTimer() = default;
    ~SoftwareTimer() = default;

    uint64_t registerTimer(int delayMs, TimerCallback cb);
    void cancelTimer(uint64_t id);
    bool extendTimer(uint64_t id, int extraDelayMs);
    void tick();
};