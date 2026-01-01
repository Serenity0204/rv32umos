#pragma once
#include "CPU.hpp"
#include "Exception.hpp"

class Kernel
{
public:
    Kernel(CPU& cpu);
    ~Kernel() = default;
    void run();
    void handleSyscall(SyscallID syscallID);

private:
    CPU& cpu;
};
