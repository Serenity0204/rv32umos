#pragma once

#include "Alarm.hpp"
#include "Exception.hpp"
#include "HAL.hpp"
#include "Loader.hpp"
#include "PageReplacementPolicy.hpp"
#include "PhysicalMemoryManager.hpp"
#include "ProcessManager.hpp"
#include "Scheduler.hpp"
#include "SwapManager.hpp"
#include "SyscallHandler.hpp"
#include "VFSInterface.hpp"
#include "VirtualMemoryManager.hpp"

class HAL;

class Kernel
{
private:
    // kernel core components
    Scheduler* scheduler = nullptr;
    ProcessManager* procManager = nullptr;
    PhysicalMemoryManager* pmm = nullptr;
    SyscallHandler* syscalls = nullptr;
    VirtualMemoryManager* vmm = nullptr;
    Alarm* alarm = nullptr;
    SwapManager* swap = nullptr;

    // filesystem related
    PageReplacementPolicy* pageReplacementPolicy = nullptr;
    VFSInterface* vfs = nullptr;

    // hardware abstraction
    HAL* hal = nullptr;

public:
    Kernel() = default;
    ~Kernel() = default;

    static void initKernelSubsystem(Kernel* kernel, HAL* hal);
    static void destroyKernelSubsystem(Kernel* kernel);

    static void handleSyscall(SyscallException& sys);
    static void handlePageFault(PageFaultException& pf);
};
