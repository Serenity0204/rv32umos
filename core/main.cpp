#include "Bus.hpp"
#include "CPU.hpp"
#include "Kernel.hpp"
#include "Memory.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: ./rv32kernel <bin_file>\n";
        return 1;
    }

    std::string filename = std::string(argv[1]);

    Kernel kernel;

    bool good = kernel.createProcess(filename);
    if (!good)
    {
        std::cout << "Failed to create process.\n";
        return 1;
    }
    good = kernel.createProcess(filename);
    if (!good)
    {
        std::cout << "Failed to create process.\n";
        return 1;
    }

    kernel.run();

    return 0;
}