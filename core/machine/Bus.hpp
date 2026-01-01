#pragma once
#include "Common.hpp"

class Memory;

class Bus
{
public:
    Bus(Memory& memory_ref);
    ~Bus() = default;
    Word load(Addr addr, std::size_t size);
    void store(Addr addr, std::size_t size, Word value);

private:
    Memory& memory;
};
