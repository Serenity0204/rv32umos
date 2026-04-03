#pragma once
#include <iostream>
#include <signal.h>
#include <sys/time.h>

class HardwareTimer
{
public:
    HardwareTimer() = default;
    ~HardwareTimer();
    void start(int interruptIntervalMS);
    void stop();
};