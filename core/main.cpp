#include "machine/Bus.hpp"
#include "machine/CPU.hpp"
#include "machine/Memory.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    std::cout << "RISCV Emulator\n";
    if (argc < 2)
    {
        std::cout << "Usage: ./rv32emu <bin_file>\n";
        return 1;
    }

    // init
    Memory memory;
    Bus bus(memory);
    CPU cpu(bus);

    std::string filename = std::string(argv[1]);
    bool good = loadBinary(filename, memory);
    if (!good)
    {
        std::cout << "Failed to load binary.\n";
        return 1;
    }

    // Run 10 cycles for testing
    for (int i = 0; i < 10; i++)
    {
        cpu.step();
    }
    return 0;
}