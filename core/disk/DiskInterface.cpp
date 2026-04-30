#include "DiskInterface.hpp"
#include "TimeModeling.hpp"

void DiskInterface::readBlock(std::size_t blockIndex, std::vector<Byte>& buffer)
{
    TIME_COST(DISK_IO_TIME, "Disk Read");
    this->readBlockImpl(blockIndex, buffer);
}

void DiskInterface::writeBlock(std::size_t blockIndex, const std::vector<Byte>& buffer)
{
    TIME_COST(DISK_IO_TIME, "Disk Write");
    this->writeBlockImpl(blockIndex, buffer);
}