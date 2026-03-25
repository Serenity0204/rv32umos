#pragma once
#include "Kernel.hpp"

class KernelInstance
{
public:
    static Kernel& instance()
    {
        static Kernel instance;
        return instance;
    }

private:
    KernelInstance() = default;
    ~KernelInstance() = default;
    KernelInstance(const KernelInstance&) = delete;
    KernelInstance& operator=(const KernelInstance&) = delete;
};

inline Kernel& kernel = KernelInstance::instance();