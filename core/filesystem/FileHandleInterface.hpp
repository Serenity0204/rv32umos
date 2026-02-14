#pragma once
#include "Types.hpp"
#include <cstddef>
#include <vector>

class FileHandleInterface
{
public:
    virtual ~FileHandleInterface() = default;

    // Returns number of bytes read/written
    virtual int read(std::vector<Byte>& buffer, std::size_t size) = 0;
    virtual int write(const std::vector<Byte>& buffer, std::size_t size) = 0;
    virtual void close() {}
};