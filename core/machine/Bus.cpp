#include "Bus.hpp"
#include "Memory.hpp"

Bus::Bus(Memory& memory_ref) : memory(memory_ref) {}

Word Bus::load(Addr addr, std::size_t size)
{
    return this->memory.load(addr, size);
}

void Bus::store(Addr addr, std::size_t size, Word value)
{
    this->memory.store(addr, size, value);
}