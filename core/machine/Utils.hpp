#pragma once
#include "Common.hpp"
#include <cstdint>
#include <sstream>

namespace Utils
{
    int32_t signExtend(uint32_t val, int bits);
    uint32_t extractBits(uint32_t value, int start, int end);
    [[noreturn]] void illegal(Word instr, const char* why);
    std::string toHex(uint32_t val);
};