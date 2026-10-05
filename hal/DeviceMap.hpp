#pragma once
#include "Device.hpp"
#include <unordered_map>

class DeviceMap
{
private:
    std::unordered_map<DeviceType, Device*> devices;

public:
    DeviceMap() = default;
    ~DeviceMap()
    {
        for (auto& [type, device] : this->devices) delete device;
        this->devices.clear();
    }

    // device tree
    inline void registerDevice(Device* dev)
    {
        if (dev != nullptr) this->devices[dev->getType()] = dev;
    }

    template <typename T>
    inline T* getDevice(DeviceType type) const
    {
        auto it = this->devices.find(type);
        if (it != this->devices.end())
            return static_cast<T*>(it->second);
        return nullptr;
    }
};