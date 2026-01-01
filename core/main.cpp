#include "Bus.hpp"
#include "CPU.hpp"
#include "Kernel.hpp"
#include "Memory.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    std::cout << "RISCV Emulator\n";
    if (argc < 2)
    {
        std::cout << "Usage: ./rv32sysemu <bin_file>\n";
        return 1;
    }

    // init
    Memory memory;
    std::string filename = std::string(argv[1]);
    bool good = loadBinary(filename, memory);

    if (!good)
    {
        std::cout << "Failed to load binary.\n";
        return 1;
    }
    Bus bus(memory);
    CPU cpu(bus);
    cpu.reset();
    Kernel kernel(cpu);

    kernel.run();

    return 1;
}