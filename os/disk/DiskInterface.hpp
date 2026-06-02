#pragma once
#include "Common.hpp"
#include "Device.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>
class DiskInterface : public Device
{
public:
    virtual ~DiskInterface() = default;
    void readBlock(std::size_t blockIndex, std::vector<Byte>& buffer);
    void writeBlock(std::size_t blockIndex, const std::vector<Byte>& buffer);
    virtual std::size_t numBlocks() const = 0;

    virtual void readBlockImpl(std::size_t blockIndex, std::vector<Byte>& buffer) = 0;
    virtual void writeBlockImpl(std::size_t blockIndex, const std::vector<Byte>& buffer) = 0;

    DeviceType getType() const override;
};