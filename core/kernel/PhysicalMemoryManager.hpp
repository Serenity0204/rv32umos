#pragma once
#include "Common.hpp"
#include <vector>

class PhysicalMemoryManager
{
public:
    PhysicalMemoryManager() = default;
    ~PhysicalMemoryManager() = default;
    void init();
    Addr allocateFrame();
    void freeFrame(Addr frame);

private:
    std::vector<Addr> freeFrames;
};
