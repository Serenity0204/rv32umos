#include "Kernel.hpp"
#include <iostream>

Kernel::Kernel(CPU& cpu) : cpu(cpu)
{
}

void Kernel::handleSyscall(SyscallID syscallID)
{
    switch (syscallID)
    {
    case SyscallID::SYS_EXIT:
        this->cpu.halt();
        break;
    default:
        std::cout << "Unimplemented syscall id:" << static_cast<int>(syscallID) << std::endl;
        exit(1);
        break;
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
            this->handleSyscall(SyscallID::SYS_EXIT);
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
