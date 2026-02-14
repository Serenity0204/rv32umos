#pragma once
#include "Common.hpp"
#include "Types.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

class DiskInterface
{
public:
    virtual ~DiskInterface() = default;
    virtual void readBlock(uint32_t blockIndex, std::vector<Byte>& buffer) = 0;
    virtual void writeBlock(uint32_t blockIndex, const std::vector<Byte>& buffer) = 0;
    virtual std::size_t numBlocks() const = 0;
};