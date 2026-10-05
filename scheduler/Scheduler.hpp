#pragma once
#include <cstddef>

class Scheduler
{
public:
    Scheduler() = default;
    ~Scheduler() = default;
    void preempt();
    bool checkAllTerminated();

private:
    void contextSwitch(std::size_t nextIndex);
    bool checkCurrentThreadRunnable();
};
