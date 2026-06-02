#pragma once
#include "Interrupt.hpp"

class ScopedCriticalSection
{
private:
    bool previousStatus;

public:
    ScopedCriticalSection()
    {
        // disable interrupt and save the previous state
        this->previousStatus = Interrupt::disable();
    }

    ~ScopedCriticalSection()
    {
        // restore the interrupt to exactly how we found them
        Interrupt::restore(this->previousStatus);
    }
};