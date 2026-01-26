#include "SyscallHandler.hpp"
#include "Common.hpp"
#include "Logger.hpp"
#include "Stats.hpp"
#include "Utils.hpp"
#include <iostream>
#include <unistd.h>
#include <vector>

SyscallHandler::SyscallHandler(KernelContext* context) : ctx(context) {}

bool SyscallHandler::dispatch(SyscallID id)
{

    STATS.incSyscalls();
    bool exited = false;

    switch (id)
    {
    case SyscallID::SYS_EXIT:
        exited = handleExit();
        break;
    case SyscallID::SYS_WRITE:
        this->handleWrite();
        break;
    case SyscallID::SYS_READ:
        this->handleRead();
        break;
    case SyscallID::SYS_THREAD_CREATE:
        this->handleCreateThread();
        break;
    case SyscallID::SYS_THREAD_EXIT:
        this->handleExitThread();
        break;
    default:
        LOG(SYSCALL, ERROR, "Unimplemented syscall id: " + std::to_string((int)id));
        this->ctx->cpu.halt();
        break;
    }

    return exited;
}

void SyscallHandler::handleCreateThread()
{
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

bool SyscallHandler::handleExitThread()
{
    Word exitCode = this->ctx->cpu.readReg(10);
    Thread* current = this->ctx->getCurrentThread();

    if (current != nullptr)
    {
        current->setState(ThreadState::TERMINATED);
        Process* proc = current->getProcess();
        LOG(KERNEL, INFO, "Thread " + std::to_string(current->getTid()) + " (PID " + std::to_string(proc->getPid()) + ")" + " exited code " + std::to_string(exitCode));
        return true; // Signal to schedule
    }
    this->ctx->cpu.halt();
    return false;
}

bool SyscallHandler::handleExit()
{
    Word exitCode = this->ctx->cpu.readReg(10);
    Thread* current = this->ctx->getCurrentThread();

    if (current == nullptr)
    {
        this->ctx->cpu.halt();
        return false;
    }

    Process* proc = current->getProcess();

    std::vector<Thread*>& threads = proc->getThreads();
    for (Thread* t : threads)
        t->setState(ThreadState::TERMINATED);

    LOG(KERNEL, INFO, "Process " + std::to_string(proc->getPid()) + " (Group Exit) terminated with code " + std::to_string(exitCode));

    return true;
}

void SyscallHandler::handleWrite()
{
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

void SyscallHandler::handleRead()
{
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