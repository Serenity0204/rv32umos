#pragma once
#include "Exception.hpp"
#include "KernelContext.hpp"

class SyscallHandler
{
public:
    SyscallHandler(KernelContext* ctx);

    // Returns true if the current thread or process exited (requiring a schedule)
    bool dispatch(SyscallID id);

private:
    KernelContext* ctx;

    bool handleExit();
    void handleWrite();
    void handleRead();
    void handleCreateThread();
    bool handleExitThread();
};