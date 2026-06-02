#pragma once
#include "Thread.hpp"
#include <queue>

class Mutex
{
public:
    Mutex();
    ~Mutex() = default;

    bool acquire(Thread* thread);
    bool release(Thread* thread);
    bool isLocked() const;

private:
    Thread* owner;
    std::queue<Thread*> waitQueue;
    bool locked;
};