#include "DiskInterface.hpp"

void DiskInterface::readBlock(std::size_t blockIndex, std::vector<Byte>& buffer)
{
    this->readBlockImpl(blockIndex, buffer);
}

void DiskInterface::writeBlock(std::size_t blockIndex, const std::vector<Byte>& buffer)
{
    this->writeBlockImpl(blockIndex, buffer);
}

DeviceType DiskInterface::getType() const
{
    return DeviceType::Disk;
}
