#include "InMemoryDisk.hpp"
#include "Stats.hpp"
#include <algorithm>

InMemoryDisk::InMemoryDisk(std::size_t size) : totalBlocks(size)
{
    this->data.resize(size * BLOCK_SIZE, 0);
}

void InMemoryDisk::readBlock(std::size_t blockIndex, std::vector<Byte>& buffer)
{
    if (blockIndex >= this->totalBlocks) return;
    if (buffer.size() != BLOCK_SIZE) return;
    STATS.incDiskReads();
    // read a block into the buffer
    Addr startOffset = blockIndex * BLOCK_SIZE;
    Addr endOffset = startOffset + BLOCK_SIZE;
    std::copy(this->data.begin() + startOffset, this->data.begin() + endOffset, buffer.begin());
}

void InMemoryDisk::writeBlock(std::size_t blockIndex, const std::vector<Byte>& buffer)
{
    if (blockIndex >= this->totalBlocks) return;
    if (buffer.size() != BLOCK_SIZE) return;
    STATS.incDiskWrites();
    // write the buffer into the disk
    Addr offset = blockIndex * BLOCK_SIZE;
    std::copy(buffer.begin(), buffer.end(), this->data.begin() + offset);
}

std::size_t InMemoryDisk::numBlocks() const
{
    return this->totalBlocks;
}