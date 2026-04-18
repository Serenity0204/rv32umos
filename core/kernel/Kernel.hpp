#pragma once
#include "HardwareTimer.hpp"
#include "KernelContext.hpp"
#include "Loader.hpp"
#include "Process.hpp"
#include "Scheduler.hpp"
#include "SoftwareTimer.hpp"
#include "SyscallHandler.hpp"
#include "VirtualMemoryManager.hpp"

class Kernel
{
public:
    Kernel();
    ~Kernel();
    void init();
    bool isRunning();

    bool createProcess(const std::string& filename);
    bool killProcess(int pid);

    static void runThread();

public:
    SystemContext* systemCtx;
    StorageContext* storageCtx;
    TimerContext* timerCtx;
    Scheduler* scheduler;
    VirtualMemoryManager* vmm;
    SyscallHandler* syscalls;
    Loader* loader;
};
