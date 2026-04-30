#pragma once
#include "DiskInterface.hpp"
#include "HardwareTimer.hpp"
#include "Machine.hpp"
#include "PhysicalMemoryManager.hpp"
#include "SystemConfig.hpp"

struct HAL
{
public:
    Machine cpu;
    PhysicalMemoryManager pmm;
    HardwareTimer timer;
    DiskInterface* disk = nullptr;
    HAL()
    {
        this->pmm.init();
        this->disk = new DiskImpl(NUM_DISK_BLOCKS);
    }

    ~HAL()
    {
        delete this->disk;
    }
};
