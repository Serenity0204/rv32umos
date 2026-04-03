#pragma once
#include "Common.hpp"
#include "Memory.hpp"

class Bus
{
public:
    Bus() = default;
    ~Bus() = default;
    Word load(Addr addr, std::size_t size);
    void store(Addr addr, std::size_t size, Word value);

private:
    Memory memory;
};
