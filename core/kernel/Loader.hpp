#pragma once
#include "KernelContext.hpp"
#include <string>

class Loader
{
public:
    Loader(KernelContext* ctx);
    bool loadELF(const std::string& filename);

private:
    KernelContext* ctx;
};