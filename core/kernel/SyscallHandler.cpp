#include "SyscallHandler.hpp"
#include "Common.hpp"
#include "Logger.hpp"
#include "Stats.hpp"
#include "Utils.hpp"
#include <iostream>
#include <unistd.h>
#include <vector>

SyscallHandler::SyscallHandler(KernelContext* context) : ctx(context) {}

SyscallStatus SyscallHandler::dispatch(SyscallID id)
{
    STATS.incSyscalls();
    this->ctx->timer.tick(SYSCALL_BASE_TIME);

    SyscallStatus status;
    switch (id)
    {
    case SyscallID::SYS_EXIT:
        handleExit(status);
        break;
    case SyscallID::SYS_WRITE:
        this->handleWrite(status);
        break;
    case SyscallID::SYS_READ:
        this->handleRead(status);
        break;
    case SyscallID::SYS_THREAD_CREATE:
        this->handleThreadCreate(status);
        break;
    case SyscallID::SYS_THREAD_EXIT:
        this->handleThreadExit(status);
        break;
    case SyscallID::SYS_THREAD_JOIN:
        this->handleThreadJoin(status);
        break;
    case SyscallID::SYS_MUTEX_CREATE:
        this->handleMutexCreate(status);
        break;
    case SyscallID::SYS_MUTEX_LOCK:
        this->handleMutexLock(status);
        break;
    case SyscallID::SYS_MUTEX_UNLOCK:
        this->handleMutexUnlock(status);
        break;
    case SyscallID::SYS_OPEN:
        this->handleOpen(status);
        break;
    case SyscallID::SYS_CLOSE:
        this->handleClose(status);
        break;
    case SyscallID::SYS_CREATE:
        this->handleCreate(status);
        break;
    case SyscallID::SYS_SBRK:
        this->handleSbrk(status);
        break;
    default:
        LOG(SYSCALL, ERROR, "Unimplemented syscall id: " + std::to_string((int)id));
        this->ctx->cpu.halt();
        break;
    }

    return status;
}

void SyscallHandler::handleThreadJoin(SyscallStatus& status)
{
    // reset status
    status.needReschedule = false;
    status.error = false;

    int targetThreadTid = this->ctx->cpu.readReg(10);
    Thread* current = this->ctx->getCurrentThread();
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
        this->ctx->cpu.writeReg(10, -1);
        this->ctx->cpu.advancePC();
        return;
    }

    // cannot join itself
    if (targetThread->getTid() == current->getTid())
    {
        LOG(SYSCALL, ERROR, "Thread Join Failed: Cannot join with yourself");
        this->ctx->cpu.writeReg(10, -1);
        this->ctx->cpu.advancePC();
        return;
    }

    // has been joined
    if (targetThread->getHasBeenJoined())
    {
        LOG(SYSCALL, ERROR, "Thread Join Failed: Target Thread with ID " + std::to_string(targetThreadTid) + " has been joined by other threads");
        this->ctx->cpu.writeReg(10, -1);
        this->ctx->cpu.advancePC();
        return;
    }

    // update status
    targetThread->setHasBeenJoined(true);

    // target already dead, return success and continue
    if (targetThread->getState() == ThreadState::TERMINATED)
    {
        this->ctx->cpu.writeReg(10, targetThread->getExitCode());
        this->ctx->cpu.advancePC();
        return;
    }

    // target still running, block the current thread
    LOG(SYSCALL, INFO, "Thread " + std::to_string(current->getTid()) + " blocking for Thread " + std::to_string(targetThreadTid));

    // set the id and block the current thread
    targetThread->setJoiner(current);
    current->setState(ThreadState::BLOCKED);
    status.needReschedule = true;
}

void SyscallHandler::handleThreadExit(SyscallStatus& status)
{
    // reset status
    status.needReschedule = false;
    status.error = false;

    Word exitCode = this->ctx->cpu.readReg(10);
    Thread* current = this->ctx->getCurrentThread();

    if (current != nullptr)
    {
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
        status.needReschedule = true;
        return;
    }
    this->ctx->cpu.halt();
}

void SyscallHandler::handleThreadCreate(SyscallStatus& status)
{
    // reset status
    status.needReschedule = false;
    status.error = false;

    Word funcPtr = this->ctx->cpu.readReg(10);
    Word arg = this->ctx->cpu.readReg(11);

    Thread* current = this->ctx->getCurrentThread();
    if (current == nullptr) return;

    Process* proc = current->getProcess();
    Thread* newThread = proc->createThread(funcPtr, arg);
    if (newThread == nullptr)
    {
        LOG(SYSCALL, ERROR, "Thread Creation Failed");
        // Return -1 to user
        this->ctx->cpu.writeReg(10, -1);
        this->ctx->cpu.advancePC();
        return;
    }

    newThread->setState(ThreadState::READY);
    this->ctx->activeThreads.push_back(newThread);

    LOG(SYSCALL, INFO, "Created Thread " + std::to_string(newThread->getTid()) + " (PID " + std::to_string(proc->getPid()) + ")");

    this->ctx->cpu.writeReg(10, newThread->getTid());
    this->ctx->cpu.advancePC();
}

void SyscallHandler::handleExit(SyscallStatus& status)
{
    // reset status
    status.needReschedule = false;
    status.error = false;

    Word exitCode = this->ctx->cpu.readReg(10);
    Thread* current = this->ctx->getCurrentThread();

    if (current == nullptr)
    {
        this->ctx->cpu.halt();
        return;
    }

    Process* proc = current->getProcess();

    std::vector<Thread*>& threads = proc->getThreads();
    for (Thread* t : threads)
        t->setState(ThreadState::TERMINATED);

    LOG(KERNEL, INFO, "Process " + std::to_string(proc->getPid()) + " (Group Exit) terminated with code " + std::to_string(exitCode));

    status.needReschedule = true;
}

void SyscallHandler::handleWrite(SyscallStatus& status)
{
    // reset status
    status.needReschedule = false;
    status.error = false;

    Thread* currentThread = this->ctx->getCurrentThread();
    if (currentThread == nullptr) return;

    Process* current = currentThread->getProcess();
    LOG(SYSCALL, DEBUG, "Write called by PID " + std::to_string(current->getPid()));

    Word rawFD = this->ctx->cpu.readReg(10);
    Word addr = this->ctx->cpu.readReg(11);
    Word count = this->ctx->cpu.readReg(12);

    // charge for extra cost
    this->ctx->timer.tick(count * 2);

    // get file handle
    int fd = static_cast<int>(rawFD);
    FileHandleInterface* handle = current->getFileHandle(fd);
    // if not valid
    if (handle == nullptr)
    {
        this->ctx->cpu.writeReg(10, -1);
        this->ctx->cpu.advancePC();
        return;
    }

    // charge for file io cost
    if (handle->type() == FileHandleInterface::Type::DiskFile) this->ctx->timer.tick(DISK_IO_TIME);

    std::vector<Byte> buffer(count);
    for (Word i = 0; i < count; ++i)
    {
        char c = this->ctx->cpu.loadVirtualMemory(addr + static_cast<Addr>(i), 1);
        buffer[i] = c;
    }

    int written = handle->write(buffer, count);
    this->ctx->cpu.writeReg(10, written);
    this->ctx->cpu.advancePC();
    return;
}

void SyscallHandler::handleRead(SyscallStatus& status)
{
    // reset status
    status.needReschedule = false;
    status.error = false;

    Thread* currentThread = this->ctx->getCurrentThread();
    if (currentThread == nullptr) return;

    Process* current = currentThread->getProcess();

    LOG(SYSCALL, DEBUG, "Read called by PID " + std::to_string(current->getPid()));

    Word rawFD = this->ctx->cpu.readReg(10);
    Word addr = this->ctx->cpu.readReg(11);
    Word count = this->ctx->cpu.readReg(12);
    // charge for extra cost
    this->ctx->timer.tick(count * 2);

    // get file handle
    int fd = static_cast<int>(rawFD);
    FileHandleInterface* handle = current->getFileHandle(fd);
    // if not valid
    if (handle == nullptr)
    {
        this->ctx->cpu.writeReg(10, -1);
        this->ctx->cpu.advancePC();
        return;
    }
    // charge for file io cost
    if (handle->type() == FileHandleInterface::Type::DiskFile) this->ctx->timer.tick(DISK_IO_TIME);

    std::vector<Byte> buffer(count);
    int bytesRead = handle->read(buffer, count);
    if (bytesRead > 0)
    {
        // Copy back to User VM
        for (int i = 0; i < bytesRead; ++i)
        {
            char rawChar = buffer[i];
            Byte byte = static_cast<uint8_t>(rawChar);
            this->ctx->cpu.storeVirtualMemory(addr + static_cast<Addr>(i), 1, static_cast<Word>(byte));
        }
    }
    this->ctx->cpu.writeReg(10, bytesRead);
    this->ctx->cpu.advancePC();
}

void SyscallHandler::handleOpen(SyscallStatus& status)
{
    status.needReschedule = false;
    status.error = false;

    Word pathAddr = this->ctx->cpu.readReg(10);

    std::string filename;
    // read virtual memory string
    std::size_t offset = 0;
    while (true)
    {
        char c = static_cast<char>(this->ctx->cpu.loadVirtualMemory(pathAddr + offset, 1));
        if (c == 0) break;
        filename += c;
        ++offset;
        if (offset > MAX_FILE_NAME_LENGTH) break;
    }

    FileHandleInterface* handle = this->ctx->vfs->open(filename);
    if (handle == nullptr)
    {
        this->ctx->cpu.writeReg(10, -1);
        this->ctx->cpu.advancePC();
        return;
    }

    // file exists
    Process* current = this->ctx->getCurrentThread()->getProcess();
    int fd = current->addFileHandle(handle);
    LOG(SYSCALL, INFO, "Opened file: " + filename + " (FD " + std::to_string(fd) + ")");

    if (fd == -1)
    {
        this->ctx->cpu.writeReg(10, -1);
        this->ctx->cpu.advancePC();
        return;
    }

    this->ctx->cpu.writeReg(10, fd);
    this->ctx->cpu.advancePC();
}

void SyscallHandler::handleClose(SyscallStatus& status)
{
    status.needReschedule = false;
    status.error = false;
    Word fd = this->ctx->cpu.readReg(10);

    Process* current = this->ctx->getCurrentThread()->getProcess();
    bool ok = current->closeFileHandle(fd);
    int code = ok ? 0 : 1;
    this->ctx->cpu.writeReg(10, code);
    this->ctx->cpu.advancePC();
}

void SyscallHandler::handleCreate(SyscallStatus& status)
{
    status.needReschedule = false;
    status.error = false;

    Word pathAddr = this->ctx->cpu.readReg(10);
    Word size = this->ctx->cpu.readReg(11);

    std::string filename;
    // read virtual memory string
    std::size_t offset = 0;
    while (true)
    {
        char c = static_cast<char>(this->ctx->cpu.loadVirtualMemory(pathAddr + offset, 1));
        if (c == 0) break;
        filename += c;
        ++offset;
        if (offset > MAX_FILE_NAME_LENGTH) break;
    }

    bool success = this->ctx->vfs->createFile(filename, size);
    if (!success)
    {
        LOG(SYSCALL, ERROR, "Failed to create file: " + filename);
        this->ctx->cpu.writeReg(10, -1);
        this->ctx->cpu.advancePC();
        return;
    }

    LOG(SYSCALL, INFO, "Created file: " + filename + " (Size: " + std::to_string(size) + ")");
    this->ctx->cpu.writeReg(10, 0);
    this->ctx->cpu.advancePC();
}

void SyscallHandler::handleSbrk(SyscallStatus& status)
{
    // reset status
    status.needReschedule = false;
    status.error = false;

    int increment = static_cast<int>(this->ctx->cpu.readReg(10));
    Process* current = this->ctx->getCurrentThread()->getProcess();
    Addr oldBreak = current->sbrk(increment);

    // sbrk failed
    if (oldBreak == 0)
    {
        LOG(SYSCALL, ERROR, "Sbrk failed: Out of Heap Memory");
        this->ctx->cpu.writeReg(10, -1);
        this->ctx->cpu.advancePC();
        return;
    }

    LOG(SYSCALL, DEBUG, "Sbrk successful. New Break: " + Utils::toHex(current->getProgramBreak()));
    this->ctx->cpu.writeReg(10, oldBreak);
    this->ctx->cpu.advancePC();
}

void SyscallHandler::handleMutexCreate(SyscallStatus& status)
{
    // reset status
    status.needReschedule = false;
    status.error = false;

    Thread* current = this->ctx->getCurrentThread();
    int mutexID = current->getProcess()->createMutex();
    this->ctx->cpu.writeReg(10, mutexID);
    this->ctx->cpu.advancePC();
}

void SyscallHandler::handleMutexLock(SyscallStatus& status)
{
    // reset status
    status.needReschedule = false;
    status.error = false;

    int mutexID = this->ctx->cpu.readReg(10);
    Thread* current = this->ctx->getCurrentThread();
    Process* process = current->getProcess();
    Mutex* lock = process->getMutex(mutexID);

    // not valid lock, error
    if (lock == nullptr)
    {
        status.error = true;
        this->ctx->cpu.writeReg(10, -1);
        this->ctx->cpu.advancePC();
        return;
    }

    bool acquired = lock->acquire(current);

    // if lock is held, need to reschedule, and dont advance PC so later when all woke up, it can retry
    if (!acquired)
    {
        status.needReschedule = true;
        return;
    }

    // lock is acquired, just advance PC and return 0
    this->ctx->cpu.writeReg(10, 0);
    this->ctx->cpu.advancePC();
}
void SyscallHandler::handleMutexUnlock(SyscallStatus& status)
{
    // reset status
    status.needReschedule = false;
    status.error = false;

    int mutexID = this->ctx->cpu.readReg(10);
    Thread* current = this->ctx->getCurrentThread();
    Process* process = current->getProcess();
    Mutex* lock = process->getMutex(mutexID);

    // not valid lock
    if (lock == nullptr)
    {
        status.error = true;
        this->ctx->cpu.writeReg(10, -1);
        this->ctx->cpu.advancePC();
        return;
    }

    // try to release
    bool success = lock->release(current);

    // not the owner releasing the lock
    if (!success)
    {
        status.error = true;
        this->ctx->cpu.writeReg(10, -1);
        this->ctx->cpu.advancePC();
        return;
    }

    // releasing success
    this->ctx->cpu.writeReg(10, 0);
    this->ctx->cpu.advancePC();
}