#include "SyscallHandler.hpp"
#include "Common.hpp"
#include "Logger.hpp"
#include "Stats.hpp"
#include <iostream>
#include <unistd.h>
#include <vector>


SyscallHandler::SyscallHandler(KernelContext* context) : ctx(context) {}

bool SyscallHandler::dispatch(SyscallID id)
{

    STATS.incSyscalls();

    switch (id)
    {
    case SyscallID::SYS_EXIT:
        return handleExit();
    case SyscallID::SYS_WRITE:
        this->handleWrite();
        return false;
    case SyscallID::SYS_READ:
        this->handleRead();
        return false;
    default:
        LOG(SYSCALL, ERROR, "Unimplemented syscall id: " + std::to_string((int)id));
        this->ctx->cpu.halt();
        return false;
    }
}

bool SyscallHandler::handleExit()
{
    Word exitCode = this->ctx->cpu.readReg(10);
    if (this->ctx->currentProcessIndex != -1)
    {
        Process* current = this->ctx->processList[this->ctx->currentProcessIndex];
        current->setState(ProcessState::TERMINATED);
        LOG(KERNEL, INFO, "Process " + std::to_string(current->getPid()) + " exited with code " + std::to_string(exitCode));
        return true; // Signal to schedule
    }
    this->ctx->cpu.halt();
    return false;
}

void SyscallHandler::handleWrite()
{
    Process* current = this->ctx->processList[this->ctx->currentProcessIndex];
    LOG(SYSCALL, DEBUG, "Write called by PID " + std::to_string(current->getPid()));

    Word rawFD = this->ctx->cpu.readReg(10);
    Word addr = this->ctx->cpu.readReg(11);
    Word count = this->ctx->cpu.readReg(12);

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
    Process* current = this->ctx->processList[this->ctx->currentProcessIndex];
    LOG(SYSCALL, DEBUG, "Read called by PID " + std::to_string(current->getPid()));

    Word rawFD = this->ctx->cpu.readReg(10);
    Word addr = this->ctx->cpu.readReg(11);
    Word count = this->ctx->cpu.readReg(12);
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