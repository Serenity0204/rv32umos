#pragma once
#include "KernelContext.hpp"

class Scheduler
{
public:
    Scheduler() = default;
    ~Scheduler() = default;
    // void yield();
    void preempt();
    inline bool getIsIdling() const { return this->isIdling; }
    void sleepCurrentThread(int delayMs, const std::string& reason);

private:
    void contextSwitch(std::size_t nextIndex);
    bool checkAllTerminated();
    bool checkCurrentThreadRunnable();
    bool isIdling = false;
};