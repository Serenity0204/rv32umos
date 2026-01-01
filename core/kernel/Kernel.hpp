#pragma once
#include "CPU.hpp"
#include "Exception.hpp"

enum class SyscallID
{
    SYS_EXIT = 0
};

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
