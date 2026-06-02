#include "stdio.h"
#include "stdlib.h"
#include "syscall.h"

// 2 Megabytes of integers (512 Pages)
// Because this is global, the ELF loader maps it to the .bss segment.
#define NUM_INTS (512 * 1024)
int large_array[NUM_INTS];

int main()
{
    putstr("Starting heavy memory allocation (2MB)...\n");

    // PHASE 1: Write to memory
    // We stride by 1024 integers (4096 bytes) to touch exactly one new page per loop.
    // This writes to the page, making it "Dirty".
    for (int i = 0; i < NUM_INTS; i += 1024)
    {
        large_array[i] = i; // Will trigger Page Fault -> RAM Full -> Swap Out
    }

    putstr("Finished writing! RAM is full and swapping occurred.\n");
    putstr("Starting verification (Reading back)...\n");

    // PHASE 2: Read from memory
    // Because the pages were swapped out, accessing them again will trigger
    // Swap-In faults, bringing the data back from the disk into RAM.
    for (int i = 0; i < NUM_INTS; i += 1024)
    {
        if (large_array[i] != i)
        {
            putstr("ERROR: Data corruption detected during Swap-In!\n");
            return 1;
        }
    }

    putstr("Verification successful! Swap mechanism is working perfectly.\n");
    return 0;
}