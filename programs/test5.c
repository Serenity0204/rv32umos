#include "stdio.h"
#include "stdlib.h"

int main()
{
    putstr("=== Segfault Test ===\n");

    // 1. Calculate an address that is definitely invalid.
    // Code starts at 0x00000000 (Low memory)
    // Stack starts at 0x07800000 (High memory)
    // Address 0x04000000 (64MB) is right in the middle (The Void).
    volatile int* invalid_addr = (int*)0x04000000;

    putstr("Attempting to write to invalid address\n");

    // 2. Perform the illegal access
    // This should trigger handlePageFault -> Check Stack (Fail) -> Check Segments (Fail) -> SegFault
    *invalid_addr = 42;

    // 3. We should never reach here
    putstr("ERROR: System did not crash! Memory protection failed.\n");
    return 0;
}