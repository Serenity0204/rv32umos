#include "SwapManager.hpp"

SwapManager::SwapManager(DiskInterface* disk, std::size_t diskNumBlocks, std::size_t swapNumBlocks)
    : disk(disk), swapStartBlock(diskNumBlocks - swapNumBlocks), swapSize(swapNumBlocks)
{
    this->allocationMap.resize(this->swapSize, false);
}

int SwapManager::swapOut(const std::vector<Byte>& buffer)
{
    for (std::size_t i = 0; i < this->allocationMap.size(); ++i)
    {
        if (!this->allocationMap[i])
        {
            std::size_t realBlockIndex = this->swapStartBlock + i;
            this->disk->writeBlock(realBlockIndex, buffer);
            this->allocationMap[i] = true;
            return static_cast<int>(i);
        }
    }
    return -1;
}

void SwapManager::swapIn(std::vector<Byte>& buffer, int slot)
{
    if (slot < 0 || static_cast<std::size_t>(slot) >= this->swapSize) return;
    std::size_t realBlockIndex = this->swapStartBlock + static_cast<std::size_t>(slot);
    this->disk->readBlock(realBlockIndex, buffer);
    this->allocationMap[slot] = false;
}