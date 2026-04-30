#pragma once
#include "KernelAlias.hpp"
#include <string>

#define TIME_COST(delayMs, reason)                            \
    do                                                        \
    {                                                         \
        K_SCHEDULER->sleepCurrentThread((delayMs), (reason)); \
    } while (0)
