#pragma once
#include "KernelContext.hpp"
#include "SyscallDefinition.hpp"

class SyscallHandler
{
public:
    SyscallHandler() = default;
    ~SyscallHandler() = default;
    SyscallStatus dispatch(SyscallID id);

private:
    // process related
    void handleExit(SyscallStatus& status);
    void handleWrite(SyscallStatus& status);
    void handleRead(SyscallStatus& status);
    void handleOpen(SyscallStatus& status);
    void handleClose(SyscallStatus& status);
    void handleCreate(SyscallStatus& status);
    void handleSbrk(SyscallStatus& status);
    void handleCreateProcess(SyscallStatus& status);
    void handleJoinProcess(SyscallStatus& status);

    // threads related
    void handleThreadCreate(SyscallStatus& status);
    void handleThreadExit(SyscallStatus& status);
    void handleThreadJoin(SyscallStatus& status);

    // mutex related
    void handleMutexCreate(SyscallStatus& status);
    void handleMutexLock(SyscallStatus& status);
    void handleMutexUnlock(SyscallStatus& status);
};