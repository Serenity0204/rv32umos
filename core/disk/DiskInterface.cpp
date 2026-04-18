#include "DiskInterface.hpp"
#include "KernelInstance.hpp"

void DiskInterface::readBlock(std::size_t blockIndex, std::vector<Byte>& buffer)
{
    kernel.scheduler->sleepCurrentThread(DISK_IO_TIME, "Disk Read");
    this->readBlockImpl(blockIndex, buffer);
}

void DiskInterface::writeBlock(std::size_t blockIndex, const std::vector<Byte>& buffer)
{
    kernel.scheduler->sleepCurrentThread(DISK_IO_TIME, "Disk Write");
    this->writeBlockImpl(blockIndex, buffer);
}