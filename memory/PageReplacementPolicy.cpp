#include "PageReplacementPolicy.hpp"

void FIFOPolicy::onAllocate(size_t ppn)
{
    this->queue.push(ppn);
}

int FIFOPolicy::findVictim(const std::vector<FrameInfo>& frameTable, const std::vector<Process*>&)
{
    while (!this->queue.empty())
    {
        std::size_t victim = this->queue.front();
        this->queue.pop();
        if (frameTable[victim].allocated) return static_cast<int>(victim);
    }
    return -1;
}