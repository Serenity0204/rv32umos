#pragma once
#include "SyscallDefinition.hpp"

class SyscallHandler
{
public:
    SyscallHandler() = default;
    ~SyscallHandler() = default;
    SyscallResult dispatch(const SyscallContext& ctx);

private:
    // process related
    SyscallResult handleExit(const SyscallContext& ctx);
    SyscallResult handleWrite(const SyscallContext& ctx);
    SyscallResult handleRead(const SyscallContext& ctx);
    SyscallResult handleOpen(const SyscallContext& ctx);
    SyscallResult handleClose(const SyscallContext& ctx);
    SyscallResult handleCreate(const SyscallContext& ctx);
    SyscallResult handleSbrk(const SyscallContext& ctx);
    SyscallResult handleCreateProcess(const SyscallContext& ctx);
    SyscallResult handleJoinProcess(const SyscallContext& ctx);

    // threads related
    SyscallResult handleThreadCreate(const SyscallContext& ctx);
    SyscallResult handleThreadExit(const SyscallContext& ctx);
    SyscallResult handleThreadJoin(const SyscallContext& ctx);

    // mutex related
    SyscallResult handleMutexCreate(const SyscallContext& ctx);
    SyscallResult handleMutexLock(const SyscallContext& ctx);
    SyscallResult handleMutexUnlock(const SyscallContext& ctx);
};