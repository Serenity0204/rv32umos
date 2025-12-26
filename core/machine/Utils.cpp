#include "Utils.hpp"

int32_t Utils::signExtend(Word val, int bits)
{
    uint32_t m = uint32_t(1) << (bits - 1);
    return int32_t((val ^ m) - m);
}