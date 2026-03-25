#include "Kernel.hpp"
#include "KernelInstance.hpp"
#include "Logger.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: ./rv32simOS <elf_file> -<log flag>\n";
        return 1;
    }

    std::string filename = std::string(argv[1]);
    bool printLogs = argc == 3 && std::string(argv[2]) == "-log" ? true : false;

    Kernel& kernel = KernelInstance::instance();
    bool good = kernel.createProcess(filename);
    kernel.createProcess(filename);
    if (!good)
    {
        std::cout << "Failed to create process.\n";
        return 1;
    }

    kernel.init();

    if (printLogs) SHOW_LOGS();

    STATS.printSummary();
    return 0;
}
