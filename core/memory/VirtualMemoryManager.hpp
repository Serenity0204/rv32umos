#pragma once
#include "KernelContext.hpp"

class VirtualMemoryManager
{
public:
    VirtualMemoryManager() = default;
    ~VirtualMemoryManager() = default;

    // Returns true if fault was handled, false if it was fatal
    bool handlePageFault(Addr faultAddr, bool& needReschedule);

private:
    bool handleStackGrowth(Process* proc, Addr faultAddr, Addr vpn, bool& needReschedule);
    bool handleHeapGrowth(Process* proc, Addr faultAddr, Addr vpn, bool& needReschedule);
    bool handleLazyLoading(Process* proc, Addr faultAddr, Addr vpn, bool& needReschedule);

    Addr allocateFrame(int pid, Addr vpn, bool& needReschedule);
    void evictPage(bool& needReschedule);
};