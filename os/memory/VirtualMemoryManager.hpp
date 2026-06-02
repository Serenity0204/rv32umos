#pragma once
#include "Process.hpp"

class VirtualMemoryManager
{
public:
    VirtualMemoryManager() = default;
    ~VirtualMemoryManager() = default;

    // Returns true if fault was handled, false if it was fatal
    bool handlePageFault(Addr faultAddr);

private:
    bool handleSwapIn(Process* proc, Addr vpn);
    bool handleStackGrowth(Process* proc, Addr faultAddr, Addr vpn);
    bool handleHeapGrowth(Process* proc, Addr faultAddr, Addr vpn);
    bool handleLazyLoading(Process* proc, Addr faultAddr, Addr vpn);

    Addr allocateFrame(int pid, Addr vpn);
    void evictPage();
};