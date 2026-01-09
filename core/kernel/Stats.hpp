#pragma once
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>

class Stats
{
public:
    static Stats& getInstance()
    {
        static Stats instance;
        return instance;
    }

    inline void incInstructions() { this->totalInstructions++; }
    inline void incContextSwitches() { this->totalContextSwitches++; }
    inline void incPageFaults() { this->totalPageFaults++; }
    inline void incSyscalls() { this->totalSyscalls++; }
    inline void incDiskReads() { this->totalDiskReads++; }
    inline void incAllocatedFrames() { this->totalAllocatedFrames++; }

    void printSummary()
    {
        std::cout << "\n============================================\n";
        std::cout << "            MACHINE STATISTICS              \n";
        std::cout << "============================================\n";

        printMetric("Total Instructions", totalInstructions);
        printMetric("Total Syscalls", totalSyscalls);
        printMetric("Total Page Faults", totalPageFaults);
        printMetric("Context Switches", totalContextSwitches);
        printMetric("Disk Reads (4KB)", totalDiskReads);
        printMetric("Physical Frames Used", totalAllocatedFrames);

        std::cout << "--------------------------------------------\n";

        double pagingRate = (totalInstructions > 0) ? (double)totalPageFaults / totalInstructions * 100.0 : 0.0;

        std::cout << " Paging Rate: " << std::fixed << std::setprecision(4)
                  << pagingRate << "% (Faults/Instr)\n";

        std::cout << "============================================\n";
    }

private:
    Stats() = default;

    uint64_t totalInstructions = 0;
    uint64_t totalContextSwitches = 0;
    uint64_t totalPageFaults = 0;
    uint64_t totalSyscalls = 0;
    uint64_t totalDiskReads = 0;
    uint64_t totalAllocatedFrames = 0;

    void printMetric(const std::string& label, uint64_t value)
    {
        std::cout << std::left << std::setw(25) << label
                  << ": " << std::right << std::setw(10) << value << "\n";
    }
};

#define STATS Stats::getInstance()