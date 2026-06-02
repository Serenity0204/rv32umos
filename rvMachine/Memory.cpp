#include "Memory.hpp"
#include <assert.h>
#include <fstream>
#include <iostream>

bool loadBinary(const std::string& filename, Memory& memory)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) return false;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size < 0 || static_cast<std::size_t>(size) > memory.memory.size()) return false;

    file.read(reinterpret_cast<char*>(memory.memory.data()), size);
    return file.good();
}

Memory::Memory()
{
    this->memory.resize(MEMORY_SIZE);
}

Word Memory::load(Addr addr, std::size_t size)
{
    assert(!(addr < MEMORY_BASE || addr >= MEMORY_BASE + MEMORY_SIZE));
    Addr index = addr - MEMORY_BASE;
    Word value = 0;
    for (std::size_t i = 0; i < size; ++i)
    {
        Word word = static_cast<Word>(this->memory[index + i]) << (i * 8);
        value |= word;
    }
    return value;
}

void Memory::store(Addr addr, std::size_t size, Word value)
{
    assert(!(addr < MEMORY_BASE || addr >= MEMORY_BASE + MEMORY_SIZE));
    Addr index = addr - MEMORY_BASE;
    for (std::size_t i = 0; i < size; ++i)
    {
        Word word = (value >> (i * 8)) & 0xFF;
        this->memory[index + i] = word;
    }
}