#include "SyscallHandler.hpp"
#include "Common.hpp"
#include "KernelAlias.hpp"
#include "KernelPanic.hpp"
#include "Loader.hpp"
#include "Logger.hpp"
#include "RV32UMOS.hpp"
#include "Stats.hpp"
#include "Utils.hpp"
#include <iostream>
#include <unistd.h>
#include <vector>

SyscallResult SyscallHandler::dispatch(const SyscallContext& ctx)
{
    STATS.incSyscalls();

    switch (ctx.id)
    {
    case SyscallID::SYS_EXIT:
        return this->handleExit(ctx);
    case SyscallID::SYS_WRITE:
        return this->handleWrite(ctx);
    case SyscallID::SYS_READ:
        return this->handleRead(ctx);
    case SyscallID::SYS_THREAD_CREATE:
        return this->handleThreadCreate(ctx);
    case SyscallID::SYS_THREAD_EXIT:
        return this->handleThreadExit(ctx);
    case SyscallID::SYS_THREAD_JOIN:
        return this->handleThreadJoin(ctx);
    case SyscallID::SYS_MUTEX_CREATE:
        return this->handleMutexCreate(ctx);
    case SyscallID::SYS_MUTEX_LOCK:
        return this->handleMutexLock(ctx);
    case SyscallID::SYS_MUTEX_UNLOCK:
        return this->handleMutexUnlock(ctx);
    case SyscallID::SYS_OPEN:
        return this->handleOpen(ctx);
    case SyscallID::SYS_CLOSE:
        return this->handleClose(ctx);
    case SyscallID::SYS_CREATE:
        return this->handleCreate(ctx);
    case SyscallID::SYS_SBRK:
        return this->handleSbrk(ctx);
    case SyscallID::SYS_CREATE_PROCESS:
        return this->handleCreateProcess(ctx);
    case SyscallID::SYS_JOIN_PROCESS:
        return this->handleJoinProcess(ctx);
    default:
        PANIC("Unimplemented syscall id: " + std::to_string((int)ctx.id));
        SyscallResult result;
        result.returnValue = (Word)-1;
        return result;
    }
}

SyscallResult SyscallHandler::handleThreadJoin(const SyscallContext& ctx)
{
    // reset status
    SyscallResult result;

    int targetThreadTid = ctx.arg0;
    Thread* current = K_PROC_MANAGER->getCurrentThread();
    Process* process = current->getProcess();

    Thread* targetThread = nullptr;

    // see if the target exist in the first place
    std::vector<Thread*>& threads = process->getThreads();
    for (Thread* thread : threads)
    {
        if (thread->getTid() == targetThreadTid)
        {
            targetThread = thread;
            break;
        }
    }

    // no need to reschedule, if the target does not exist in the first place
    if (targetThread == nullptr)
    {
        LOG(SYSCALL, ERROR, "Thread Join Failed: Target Thread ID does not exist " + std::to_string(targetThreadTid));
        result.returnValue = -1;
        return result;
    }

    // cannot join itself
    if (targetThread->getTid() == current->getTid())
    {
        LOG(SYSCALL, ERROR, "Thread Join Failed: Cannot join with yourself");
        result.returnValue = -1;
        return result;
    }

    // has been joined
    if (targetThread->getHasBeenJoined())
    {
        LOG(SYSCALL, ERROR, "Thread Join Failed: Target Thread with ID " + std::to_string(targetThreadTid) + " has been joined by other threads");
        result.returnValue = -1;
        return result;
    }

    // update status
    targetThread->setHasBeenJoined(true);

    // target already dead, return success and continue
    if (targetThread->getState() == ThreadState::TERMINATED)
    {
        result.returnValue = targetThread->getExitCode();
        return result;
    }

    // target still running, block the current thread
    LOG(SYSCALL, INFO, "Thread " + std::to_string(current->getTid()) + " blocking for Thread " + std::to_string(targetThreadTid));

    // set the id and block the current thread
    targetThread->setJoiner(current);
    current->setState(ThreadState::BLOCKED);

    result.needReschedule = true;
    result.advancePC = false;
    result.hasReturnValue = false;
    return result;
}

SyscallResult SyscallHandler::handleThreadExit(const SyscallContext& ctx)
{
    // reset status
    SyscallResult result;

    Word exitCode = ctx.arg0;
    Thread* current = K_PROC_MANAGER->getCurrentThread();

    current->setState(ThreadState::TERMINATED);
    current->setExitCode(exitCode);
    Process* proc = current->getProcess();
    LOG(KERNEL, INFO, "Thread " + std::to_string(current->getTid()) + " (PID " + std::to_string(proc->getPid()) + ")" + " exited code " + std::to_string(exitCode));

    // check for joiner
    Thread* joiner = current->getJoiner();
    if (joiner != nullptr)
    {
        joiner->setState(ThreadState::READY);
        // pass the exit code to joiner for their ecall
        joiner->getRegs().write(10, exitCode);
        // advance joiner PC
        joiner->setPC(joiner->getPC() + 4);
        LOG(SYSCALL, INFO, "Thread " + std::to_string(current->getTid()) + " waking up Joiner " + std::to_string(joiner->getTid()));
    }
    // Signal to schedule
    result.needReschedule = true;
    result.advancePC = false;
    result.hasReturnValue = false;
    return result;
}

SyscallResult SyscallHandler::handleThreadCreate(const SyscallContext& ctx)
{
    // reset status
    SyscallResult result;

    Word funcPtr = ctx.arg0;
    Word arg = ctx.arg1;

    Thread* current = K_PROC_MANAGER->getCurrentThread();

    Process* proc = current->getProcess();
    Thread* newThread = proc->createThread(funcPtr, arg);
    if (newThread == nullptr)
    {
        LOG(SYSCALL, ERROR, "Thread Creation Failed");
        // Return -1 to user
        result.returnValue = -1;
        return result;
    }
    newThread->setupHostContext(reinterpret_cast<void (*)()>(&RV32UMOS::runThread));
    newThread->setState(ThreadState::READY);
    K_PROC_MANAGER->activeThreads.push_back(newThread);

    LOG(SYSCALL, INFO, "Created Thread " + std::to_string(newThread->getTid()) + " (PID " + std::to_string(proc->getPid()) + ")");

    result.returnValue = newThread->getTid();
    return result;
}

SyscallResult SyscallHandler::handleWrite(const SyscallContext& ctx)
{
    // reset status
    SyscallResult result;

    Thread* currentThread = K_PROC_MANAGER->getCurrentThread();

    Process* current = currentThread->getProcess();
    LOG(SYSCALL, DEBUG, "Write called by PID " + std::to_string(current->getPid()));

    Word rawFD = ctx.arg0;
    Word addr = ctx.arg1;
    Word count = ctx.arg2;

    // get file handle
    int fd = static_cast<int>(rawFD);
    FileHandleInterface* handle = current->getFileHandle(fd);
    // if not valid
    if (handle == nullptr)
    {
        result.returnValue = -1;
        return result;
    }

    std::vector<Byte> buffer(count);
    for (Word i = 0; i < count; ++i)
    {
        char c = CPU_HAL->loadVirtualMemory(addr + static_cast<Addr>(i), 1);
        buffer[i] = c;
    }

    // will block if it's disk IO
    int written = handle->write(buffer, count);
    result.returnValue = written;

    if (currentThread->getState() == ThreadState::BLOCKED) result.needReschedule = true;
    return result;
}

SyscallResult SyscallHandler::handleRead(const SyscallContext& ctx)
{
    // reset status
    SyscallResult result;

    Thread* currentThread = K_PROC_MANAGER->getCurrentThread();
    Process* current = currentThread->getProcess();

    LOG(SYSCALL, DEBUG, "Read called by PID " + std::to_string(current->getPid()));

    Word rawFD = ctx.arg0;
    Word addr = ctx.arg1;
    Word count = ctx.arg2;

    // get file handle
    int fd = static_cast<int>(rawFD);
    FileHandleInterface* handle = current->getFileHandle(fd);
    // if not valid
    if (handle == nullptr)
    {
        result.returnValue = -1;
        return result;
    }

    std::vector<Byte> buffer(count);
    int bytesRead = handle->read(buffer, count);
    if (bytesRead > 0)
    {
        // Copy back to User VM
        for (int i = 0; i < bytesRead; ++i)
        {
            char rawChar = buffer[i];
            Byte byte = static_cast<uint8_t>(rawChar);
            CPU_HAL->storeVirtualMemory(addr + static_cast<Addr>(i), 1, static_cast<Word>(byte));
        }
    }
    result.returnValue = bytesRead;

    if (currentThread->getState() == ThreadState::BLOCKED) result.needReschedule = true;
    return result;
}

SyscallResult SyscallHandler::handleOpen(const SyscallContext& ctx)
{
    SyscallResult result;

    Word pathAddr = ctx.arg0;
    Thread* currentThread = K_PROC_MANAGER->getCurrentThread();
    std::string filename;
    // read virtual memory string
    std::size_t offset = 0;
    while (true)
    {
        char c = static_cast<char>(CPU_HAL->loadVirtualMemory(pathAddr + offset, 1));
        if (c == 0) break;
        filename += c;
        ++offset;
        if (offset > MAX_FILE_NAME_LENGTH) break;
    }

    FileHandleInterface* handle = K_VFS->open(filename);
    if (handle == nullptr)
    {
        result.returnValue = -1;
        return result;
    }

    // file exists
    Process* current = K_PROC_MANAGER->getCurrentThread()->getProcess();
    int fd = current->addFileHandle(handle);
    LOG(SYSCALL, INFO, "Opened file: " + filename + " (FD " + std::to_string(fd) + ")");

    if (fd == -1)
    {
        result.returnValue = -1;
        return result;
    }

    result.returnValue = fd;

    if (currentThread->getState() == ThreadState::BLOCKED) result.needReschedule = true;
    return result;
}

SyscallResult SyscallHandler::handleClose(const SyscallContext& ctx)
{
    SyscallResult result;
    Word fd = ctx.arg0;

    Process* current = K_PROC_MANAGER->getCurrentThread()->getProcess();
    bool ok = current->closeFileHandle(fd);
    int code = ok ? 0 : 1;
    result.returnValue = code;
    return result;
}

SyscallResult SyscallHandler::handleCreate(const SyscallContext& ctx)
{
    SyscallResult result;

    Word pathAddr = ctx.arg0;
    Word size = ctx.arg1;
    Thread* currentThread = K_PROC_MANAGER->getCurrentThread();
    std::string filename;
    // read virtual memory string
    std::size_t offset = 0;
    while (true)
    {
        char c = static_cast<char>(CPU_HAL->loadVirtualMemory(pathAddr + offset, 1));
        if (c == 0) break;
        filename += c;
        ++offset;
        if (offset > MAX_FILE_NAME_LENGTH) break;
    }

    bool success = K_VFS->createFile(filename, size);
    if (!success)
    {
        LOG(SYSCALL, ERROR, "Failed to create file: " + filename);
        result.returnValue = -1;
        return result;
    }

    LOG(SYSCALL, INFO, "Created file: " + filename + " (Size: " + std::to_string(size) + ")");
    result.returnValue = 0;

    if (currentThread->getState() == ThreadState::BLOCKED) result.needReschedule = true;
    return result;
}

SyscallResult SyscallHandler::handleSbrk(const SyscallContext& ctx)
{
    // reset status
    SyscallResult result;

    int increment = static_cast<int>(ctx.arg0);
    Process* current = K_PROC_MANAGER->getCurrentThread()->getProcess();
    Addr oldBreak = current->sbrk(increment);

    // sbrk failed
    if (oldBreak == 0)
    {
        LOG(SYSCALL, ERROR, "Sbrk failed: Out of Heap Memory");
        result.returnValue = -1;
        return result;
    }

    LOG(SYSCALL, DEBUG, "Sbrk successful. New Break: " + Utils::toHex(current->getProgramBreak()));
    result.returnValue = oldBreak;
    return result;
}

SyscallResult SyscallHandler::handleMutexCreate(const SyscallContext&)
{
    // reset status
    SyscallResult result;

    Thread* current = K_PROC_MANAGER->getCurrentThread();
    int mutexID = current->getProcess()->createMutex();
    result.returnValue = mutexID;
    return result;
}

SyscallResult SyscallHandler::handleMutexLock(const SyscallContext& ctx)
{
    // reset status
    SyscallResult result;

    int mutexID = ctx.arg0;
    Thread* current = K_PROC_MANAGER->getCurrentThread();
    Process* process = current->getProcess();
    Mutex* lock = process->getMutex(mutexID);

    // not valid lock, error
    if (lock == nullptr)
    {
        result.error = true;
        result.returnValue = -1;
        return result;
    }

    bool acquired = lock->acquire(current);

    // if lock is held, need to reschedule, and dont advance PC so later when all woke up, it can retry
    if (!acquired)
    {
        result.needReschedule = true;
        result.advancePC = false;
        result.hasReturnValue = false;
        return result;
    }

    // lock is acquired, just advance PC and return 0
    result.returnValue = 0;
    return result;
}
SyscallResult SyscallHandler::handleMutexUnlock(const SyscallContext& ctx)
{
    // reset status
    SyscallResult result;

    int mutexID = ctx.arg0;
    Thread* current = K_PROC_MANAGER->getCurrentThread();
    Process* process = current->getProcess();
    Mutex* lock = process->getMutex(mutexID);

    // not valid lock
    if (lock == nullptr)
    {
        result.error = true;
        result.returnValue = -1;
        return result;
    }

    // try to release
    bool success = lock->release(current);

    // not the owner releasing the lock
    if (!success)
    {
        result.error = true;
        result.returnValue = -1;
        return result;
    }

    // releasing success
    result.returnValue = 0;
    return result;
}

SyscallResult SyscallHandler::handleCreateProcess(const SyscallContext& ctx)
{
    SyscallResult result;

    // read virtual memory string for filename
    Word pathAddr = ctx.arg0;
    std::string filename;
    std::size_t offset = 0;
    while (true)
    {
        char c = static_cast<char>(CPU_HAL->loadVirtualMemory(pathAddr + offset, 1));
        if (c == 0) break;
        filename += c;
        if (++offset > MAX_FILE_NAME_LENGTH) break;
    }

    int createdPid = Loader::loadELF(filename);
    if (createdPid == -1)
    {
        LOG(SYSCALL, ERROR, "Create process failed for file: " + filename);
        result.returnValue = -1;
        return result;
    }
    result.returnValue = createdPid;
    return result;
}

SyscallResult SyscallHandler::handleJoinProcess(const SyscallContext& ctx)
{
    SyscallResult result;

    int targetPid = static_cast<int>(ctx.arg0);
    Word statusPtr = ctx.arg1;

    // Fast Path: process already dead
    if (K_PROC_MANAGER->exitCodes.count(targetPid))
    {
        int exitCode = K_PROC_MANAGER->exitCodes[targetPid];
        if (statusPtr != 0)
            CPU_HAL->storeVirtualMemory(statusPtr, 4, static_cast<Word>(exitCode));

        K_PROC_MANAGER->exitCodes.erase(targetPid);
        result.returnValue = 1;
        LOG(SYSCALL, INFO, "Join instantly reaped cached Exit Code for PID " + std::to_string(targetPid));
        return result;
    }

    bool validPID = (targetPid >= 0) && (targetPid < MAX_PROCESS) && K_PROC_MANAGER->processList[targetPid]->isActive();

    // invalid PID
    if (!validPID)
    {
        LOG(SYSCALL, ERROR, "Join failed: invalid or inactive PID " + std::to_string(targetPid));
        result.returnValue = -1;
        return result;
    }

    // if process still running
    Thread* current = K_PROC_MANAGER->getCurrentThread();
    K_PROC_MANAGER->processWaiters[targetPid].push_back(current);
    current->setState(ThreadState::BLOCKED);

    result.needReschedule = true;
    result.advancePC = false;
    result.hasReturnValue = false;

    LOG(SYSCALL, INFO, "Thread " + std::to_string(current->getTid()) + " BLOCKED waiting for PID " + std::to_string(targetPid));
    // PC is NOT advanced. When woken up, it will re-execute and hit Fast Path.
    return result;
}

SyscallResult SyscallHandler::handleExit(const SyscallContext& ctx)
{
    // reset status
    SyscallResult result;

    Word exitCode = ctx.arg0;
    Thread* current = K_PROC_MANAGER->getCurrentThread();

    Process* proc = current->getProcess();
    int currentPid = proc->getPid();

    bool ok = Process::terminate(currentPid, static_cast<int>(exitCode), false);
    if (!ok) PANIC("KERNEL PANIC: Failed to terminate process " + std::to_string(currentPid));

    result.needReschedule = true;
    result.advancePC = false;
    result.hasReturnValue = false;
    return result;
}