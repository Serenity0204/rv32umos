#pragma once
#include "Common.hpp"
#include "Device.hpp"
#include "DeviceMap.hpp"
#include "DiskInterface.hpp"
#include "HardwareTimer.hpp"
#include "Interrupt.hpp"
#include "KernelAlias.hpp"
#include "Machine.hpp"
#include "Memory.hpp"
#include <unordered_map>
#include <vector>

class HAL
{
private:
    DeviceMap devices;

public:
    HAL() = default;
    ~HAL() = default;

    // device related
    template <typename T>
    inline T* getDevice(DeviceType type) const
    {
        return this->devices.getDevice<T>(type);
    }

    inline void registerDevice(Device* dev)
    {
        this->devices.registerDevice(dev);
    }
};

#define CPU_HAL K_HAL->getDevice<Machine>(DeviceType::CPU)
#define MEMORY_HAL K_HAL->getDevice<Memory>(DeviceType::Memory)
#define TIMER_HAL K_HAL->getDevice<HardwareTimer>(DeviceType::Timer)
#define DISK_HAL K_HAL->getDevice<DiskInterface>(DeviceType::Disk)
#define INTERRUPT_HAL K_HAL->getDevice<Interrupt>(DeviceType::Interrupt)