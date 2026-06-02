#pragma once

enum class DeviceType
{
    CPU,
    Memory,
    Interrupt,
    Timer,
    Disk,
};

class Device
{
public:
    virtual ~Device() = default;
    virtual DeviceType getType() const = 0;
};
