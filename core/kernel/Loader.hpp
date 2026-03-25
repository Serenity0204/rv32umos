#pragma once
#include "KernelContext.hpp"
#include <string>

class Loader
{
public:
    Loader() = default;
    bool loadELF(const std::string& filename);
};