#pragma once
#include "CPU.hpp"
#include "PhysicalMemoryManager.hpp"
#include "Process.hpp"
#include "Stats.hpp"
#include <vector>

using ProcessState = Process::ProcessState;

struct KernelContext
{
public:
    CPU cpu;
    PhysicalMemoryManager pmm;
    std::vector<Process*> processList;
    int currentProcessIndex = -1;

    inline bool isRunning()
    {
        return this->cpu.isHalted();
    }

    KernelContext()
    {
        this->pmm.init();
    }

    ~KernelContext()
    {
        for (auto* p : this->processList) delete p;
        this->processList.clear();
    }
};