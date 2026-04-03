#include "SoftwareTimer.hpp"

uint64_t SoftwareTimer::registerTimer(int delayMs, TimerCallback cb)
{
    ScopedCriticalSection lock;
    auto expireTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(delayMs);
    uint64_t id = ++this->nextId;
    this->timers.push({expireTime, cb, id});
    return id;
}

void SoftwareTimer::cancelTimer(uint64_t id)
{
    ScopedCriticalSection lock;
    this->cancelledTimers.insert(id);
}

void SoftwareTimer::tick()
{
    ScopedCriticalSection lock;
    auto now = std::chrono::steady_clock::now();

    while (!this->timers.empty())
    {
        // If the top timer is in the future, ALL timers are in the future. Stop checking.
        if (this->timers.top().expiration > now) break;

        TimerEvent event = this->timers.top();
        this->timers.pop();

        // check if the timer was cancelled
        if (this->cancelledTimers.count(event.id) > 0)
        {
            this->cancelledTimers.erase(event.id);
            continue;
        }
        // Execute the callback function
        event.callback();
    }
}