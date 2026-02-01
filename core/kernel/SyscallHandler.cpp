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
        LOG(SYSCALL, ERROR, "Thread Creation Failed: Invalid Entry PC " + Utils::toHex(funcPtr));
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

    // 2 cycles per byte for modeling
    size_t copyCost = count * 2;
    this->ctx->timer.tick(SYSCALL_BASE_TIME + copyCost);

    FileDescriptor fd = static_cast<FileDescriptor>(rawFD);
    if (fd == FileDescriptor::STDOUT || fd == FileDescriptor::STDERR)
    {
        std::vector<char> hostBuffer(count);
        for (Word i = 0; i < count; ++i)
        {
            char c = this->ctx->cpu.loadVirtualMemory(addr + static_cast<Addr>(i), 1);
            hostBuffer[i] = c;
        }

        int hostFD = (fd == FileDescriptor::STDOUT) ? STDOUT_FILENO : STDERR_FILENO;
        ssize_t written = ::write(hostFD, hostBuffer.data(), count);
        std::cout.flush();

        this->ctx->cpu.writeReg(10, written);
        this->ctx->cpu.advancePC();
        return;
    }
    // other fd, not supported yet
    this->ctx->cpu.writeReg(10, static_cast<Word>(-1));
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

    // 2 cycles per byte for modeling
    size_t copyCost = count * 2;
    this->ctx->timer.tick(SYSCALL_BASE_TIME + copyCost);

    FileDescriptor fd = static_cast<FileDescriptor>(rawFD);
    if (fd == FileDescriptor::STDIN)
    {
        std::vector<char> hostBuffer(count);
        ssize_t bytesRead = ::read(STDIN_FILENO, hostBuffer.data(), count);

        if (bytesRead > 0)
        {
            for (ssize_t i = 0; i < bytesRead; ++i)
            {
                char rawChar = hostBuffer[i];
                uint8_t byte = static_cast<uint8_t>(rawChar);
                this->ctx->cpu.storeVirtualMemory(addr + static_cast<Addr>(i), 1, static_cast<Word>(byte));
            }
        }
        // other fd, not supported yet
        this->ctx->cpu.writeReg(10, static_cast<Word>(bytesRead));
        this->ctx->cpu.advancePC();
        return;
    }

    this->ctx->cpu.writeReg(10, static_cast<Word>(-1));
    this->ctx->cpu.advancePC();
    return;
}