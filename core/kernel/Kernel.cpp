#include "Kernel.hpp"
#include <iostream>
#include <unistd.h>
#include <vector>

Kernel::Kernel(CPU& cpu) : cpu(cpu)
{
}

void Kernel::handleSyscall(SyscallID syscallID)
{
    switch (syscallID)
    {
    case SyscallID::SYS_EXIT:
    {
        this->cpu.halt();
        return;
    }
    case SyscallID::SYS_WRITE:
    {
        Word rawFD = this->cpu.readReg(10);
        Word addr = this->cpu.readReg(11);
        Word count = this->cpu.readReg(12);
        FileDescriptor fd = static_cast<FileDescriptor>(rawFD);
        if (fd == FileDescriptor::STDOUT || fd == FileDescriptor::STDERR)
        {
            std::vector<char> hostBuffer(count);
            for (Word i = 0; i < count; ++i)
            {
                char c = this->cpu.load(addr + static_cast<Addr>(i), 1);
                hostBuffer[i] = c;
            }
            int hostFD = (fd == FileDescriptor::STDOUT) ? STDOUT_FILENO : STDERR_FILENO;
            ssize_t written = ::write(hostFD, hostBuffer.data(), count);
            this->cpu.writeReg(10, written);
            return;
        }
        // invalid
        this->cpu.writeReg(10, static_cast<Word>(-1));
        return;
    }
    default:
    {
        std::cout << "Unimplemented syscall id:" << static_cast<int>(syscallID) << std::endl;
        exit(1);
    }
    }
}

void Kernel::run()
{
    std::cout << "Simulation started..." << std::endl;
    uint64_t instructions = 0;

    while (!this->cpu.isHalted())
    {
        try
        {
            this->cpu.step();
            this->cpu.advancePC();
            instructions++;
        }
        catch (SyscallException& sys)
        {
            this->handleSyscall(sys.getSyscallID());
            this->cpu.advancePC();
        }
        catch (std::exception& e)
        {
            std::cerr << "Unexpected exception: " << e.what() << std::endl;
            break;
        }
    }

    // x10 (a0)
    Word result = cpu.readReg(10);
    std::cout << "--------------------------------\n";
    std::cout << "Simulation finished in " << std::dec << instructions << " instructions.\n";
    std::cout << "Exit Code (x10): " << result << "\n";
    std::cout << "--------------------------------\n";
}
