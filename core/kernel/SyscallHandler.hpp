#pragma once
#include "Exception.hpp"
#include "KernelContext.hpp"

class SyscallHandler
{
public:
    SyscallHandler(KernelContext* ctx);
    SyscallStatus dispatch(SyscallID id);

private:
    KernelContext* ctx;

    // process related
    void handleExit(SyscallStatus& status);
    void handleWrite(SyscallStatus& status);
    void handleRead(SyscallStatus& status);

    // threads related
    void handleThreadCreate(SyscallStatus& status);
    void handleThreadExit(SyscallStatus& status);
    void handleThreadJoin(SyscallStatus& status);
};