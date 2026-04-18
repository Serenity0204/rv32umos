#include "SoftwareTimer.hpp"

uint64_t SoftwareTimer::registerTimer(int delayMs, TimerCallback cb)
{
    ScopedCriticalSection lock;
    auto expireTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(delayMs);
    uint64_t id = ++this->nextId;
    this->activeTimers[id] = {expireTime, cb, true};
    this->queue.push({expireTime, id});
    return id;
}

void SoftwareTimer::cancelTimer(uint64_t id)
{
    ScopedCriticalSection lock;
    if (this->activeTimers.count(id))
        this->activeTimers[id].active = false;
}

bool SoftwareTimer::extendTimer(uint64_t id, int extraDelayMs)
{
    ScopedCriticalSection lock;
    if (this->activeTimers.count(id) && this->activeTimers[id].active)
    {
        // Add the extra time to the existing true expiration
        auto newExpire = this->activeTimers[id].expiration + std::chrono::milliseconds(extraDelayMs);
        this->activeTimers[id].expiration = newExpire;

        // Push the updated time into the queue.
        // The old event will be discarded later because the expiration won't match!
        this->queue.push({newExpire, id});
        return true;
    }
    return false;
}

void SoftwareTimer::tick()
{
    ScopedCriticalSection lock;
    auto now = std::chrono::steady_clock::now();

    while (!this->queue.empty())
    {
        // If the top timer is in the future, ALL timers are in the future. Stop checking.
        if (this->queue.top().expiration > now) break;

        TimerEvent event = this->queue.top();
        this->queue.pop();

        if (!this->activeTimers.count(event.id)) continue;

        TimerInfo& info = this->activeTimers[event.id];
        // Only execute if it wasn't cancelled AND it matches the latest extended time
        if (info.active && info.expiration == event.expiration)
        {
            info.active = false;
            TimerCallback cb = info.callback;
            this->activeTimers.erase(event.id);

            // Execute the callback
            cb();
        }
    }
}