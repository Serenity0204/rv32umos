#pragma once
#include "Device.hpp"
#include <signal.h>

class Scheduler;

class Interrupt : public Device
{
private:
    bool setStatus(bool enableInterrupts);

private:
    static Scheduler* scheduler;

public:
    Interrupt() = default;
    ~Interrupt() = default;
    void enable();
    bool disable();
    void restore(bool status);
    DeviceType getType() const override;

    static void init(Scheduler* sched);
    static void timerInterruptHandler(int signum);
};
