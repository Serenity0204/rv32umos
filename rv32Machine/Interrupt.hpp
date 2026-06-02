#pragma once
#include "Device.hpp"
#include <signal.h>

class Scheduler;

class Interrupt : public Device
{
private:
    Interrupt() = default;
    ~Interrupt() = default;
    Interrupt(const Interrupt&) = delete;
    Interrupt& operator=(const Interrupt&) = delete;
    static bool setStatus(bool enableInterrupts);

    DeviceType getType() const override;

private:
    static Scheduler* scheduler;

public:
    static void enable();
    static bool disable();
    static void restore(bool status);

    static void init(Scheduler* sched);
    static void timerInterruptHandler(int signum);
};
