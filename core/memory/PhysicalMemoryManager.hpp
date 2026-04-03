#pragma once
#include "Common.hpp"
#include "PageReplacementPolicy.hpp"
#include <vector>

struct FrameAllocInfo
{
public:
    Addr paddr;
    bool status;
};

class PhysicalMemoryManager
{
public:
    PhysicalMemoryManager() = default;
    ~PhysicalMemoryManager() = default;
    void init();
    FrameAllocInfo allocateFrame();
    void freeFrame(Addr frame);

    void registerFrameOwner(Addr ppn, Addr vpn, int ownerPid);

    inline std::size_t getTotalFrames() const { return this->frameTable.size(); }
    inline const std::vector<FrameInfo>& getFrameTable() { return this->frameTable; }

private:
    std::vector<Addr> freeFrames;
    std::vector<FrameInfo> frameTable;
};
