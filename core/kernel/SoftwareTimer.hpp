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
    TimerCallback callback;
    uint64_t id;

    bool operator>(const TimerEvent& other) const
    {
        return this->expiration > other.expiration;
    }
};

class SoftwareTimer
{
private:
    std::priority_queue<TimerEvent, std::vector<TimerEvent>, std::greater<TimerEvent>> timers;
    std::unordered_set<uint64_t> cancelledTimers;
    uint64_t nextId = 0;

public:
    SoftwareTimer() = default;
    ~SoftwareTimer() = default;

    uint64_t registerTimer(int delayMs, TimerCallback cb);
    void cancelTimer(uint64_t id);
    void tick();
};