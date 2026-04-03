#pragma once
#include "DiskInterface.hpp"
#include <vector>

class SwapManager
{
public:
    SwapManager(DiskInterface* disk, std::size_t diskNumBlocks, std::size_t swapNumBlocks);

    int swapOut(const std::vector<Byte>& buffer);
    void swapIn(std::vector<Byte>& buffer, int slot);

private:
    DiskInterface* disk;
    std::size_t swapStartBlock;
    std::size_t swapSize;
    std::vector<bool> allocationMap;
};