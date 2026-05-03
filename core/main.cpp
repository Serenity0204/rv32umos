#include "Logger.hpp"
#include "RV32UMOS.hpp"
#include "Stats.hpp"
#include <iostream>
#include <string>
#include <vector>

static std::vector<std::string> parseArgs(int argc, char* argv[])
{
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) args.push_back(std::string(argv[i]));
    return args;
}

static bool parseEnableLogs(const std::vector<std::string>& args)
{
    for (const std::string& arg : args)
    {
        if (arg == "-logs")
            return true;
    }
    return false;
}

static std::vector<std::string> parseElfFiles(const std::vector<std::string>& args)
{
    std::vector<std::string> elfFiles;
    for (const std::string& arg : args)
    {
        if (arg.size() >= 4 && arg.substr(arg.size() - 4) == ".elf")
            elfFiles.push_back(arg);
    }
    return elfFiles;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: ./rv32umos <elf_file 1> <elf_file 2> <elf_file 3> ... <-logs>\n";
        return 1;
    }

    std::vector<std::string> args = parseArgs(argc, argv);
    bool printLogs = parseEnableLogs(args);
    std::vector<std::string> filenames = parseElfFiles(args);

    RV32UMOS::init();

    for (const std::string& filename : filenames)
    {
        bool ok = RV32UMOS::loadApplication(filename);
        if (!ok)
        {
            std::cout << "Create Process with filename: " << filename << " failed." << std::endl;
            RV32UMOS::destroy();
            return 1;
        }
    }

    RV32UMOS rv32umos;
    rv32umos.start();
    rv32umos.waitForShutdown();

    if (printLogs) SHOW_LOGS();
    STATS.printSummary();
    RV32UMOS::destroy();
    return 0;
}
