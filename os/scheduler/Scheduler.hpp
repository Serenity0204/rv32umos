#pragma once
#include <cstddef>
#include <string>

class Scheduler
{
public:
    Scheduler() = default;
    ~Scheduler() = default;
    void preempt();
    inline bool getIsIdling() const { return this->isIdling; }
    void sleepCurrentThread(int delayMs, const std::string& reason);
    bool checkAllTerminated();

private:
    void contextSwitch(std::size_t nextIndex);
    bool checkCurrentThreadRunnable();
    bool isIdling = false;

    void* mainStackPtr = nullptr;
};

