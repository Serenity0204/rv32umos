#pragma once
#include "Common.hpp"
#include <vector>

class Memory
{
public:
    Memory();
    ~Memory() = default;
    Word load(Addr addr, std::size_t size);
    void store(Addr addr, std::size_t size, Word value);

    friend bool loadBinary(const std::string& filename, Memory& memory);

private:
    std::vector<Byte> memory;
};