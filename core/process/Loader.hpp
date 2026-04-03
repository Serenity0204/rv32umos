#pragma once
#include "KernelContext.hpp"
#include <string>

class Loader
{
public:
    Loader() = default;
    // return PID if success, else -1
    int loadELF(const std::string& filename);
};