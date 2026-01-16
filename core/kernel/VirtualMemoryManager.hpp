#pragma once
#include "KernelContext.hpp"

class VirtualMemoryManager
{
public:
    VirtualMemoryManager(KernelContext* ctx);

    // Returns true if fault was handled, false if it was fatal
    bool handlePageFault(Addr faultAddr);

private:
    KernelContext* ctx;

    bool handleStackGrowth(Process* proc, Addr faultAddr, Addr vpn);
    bool handleLazyLoading(Process* proc, Addr faultAddr, Addr vpn);
};