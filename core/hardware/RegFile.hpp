#pragma once

#include "Common.hpp"
#include <array>

#include <cstddef>

class RegFile
{
public:
    RegFile();
    ~RegFile() = default;
    Word read(std::size_t index) const;
    void write(std::size_t index, Word value);
    Word operator[](std::size_t index) const;
    void reset();

private:
    std::array<Word, 32> regs;
};
