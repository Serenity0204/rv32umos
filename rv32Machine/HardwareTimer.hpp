#pragma once
#include "Device.hpp"
#include <iostream>
#include <signal.h>
#include <sys/time.h>

class HardwareTimer : public Device
{
public:
    HardwareTimer() = default;
    ~HardwareTimer();
    void start(int interruptIntervalMS);
    void stop();
    DeviceType getType() const override;
};