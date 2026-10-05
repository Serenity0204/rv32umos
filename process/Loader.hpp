#pragma once
#include <string>

class Loader
{
public:
    Loader() = default;
    // return PID if success, else -1
    static int loadELF(const std::string& filename);
};