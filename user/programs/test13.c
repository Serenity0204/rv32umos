#include "stdio.h"
#include "stdlib.h"
#include "syscall.h" // Using your custom I/O

int main()
{
    putstr("[Malloc Test] Starting heap allocation test...\n");

    // ===================================================================
    // PHASE 1: Small Allocations
    // ===================================================================
    putstr("[Malloc Test] Allocating small blocks...\n");
    int* arr1 = (int*)malloc(100 * sizeof(int));
    int* arr2 = (int*)malloc(250 * sizeof(int));

    if (!arr1 || !arr2)
    {
        putstr("ERROR: malloc returned NULL for small blocks!\n");
        return 1;
    }

    // Write distinct data patterns
    for (int i = 0; i < 100; i++) arr1[i] = i;
    for (int i = 0; i < 250; i++) arr2[i] = i * 2;

    // ===================================================================
    // PHASE 2: Large Allocation (Stress Test Page Faults)
    // ===================================================================
    // 50,000 integers = 200,000 bytes (~49 Pages of 4KB)
    // This is well within your 16 MB Heap limit, but large enough to
    // heavily test your Virtual Memory Manager's Heap Growth logic!
    putstr("[Malloc Test] Allocating large block (200KB)...\n");
    int* large_arr = (int*)malloc(50000 * sizeof(int));

    if (!large_arr)
    {
        putstr("ERROR: malloc returned NULL for large block!\n");
        return 1;
    }

    // Write data to the large array (Triggering page faults page-by-page)
    for (int i = 0; i < 50000; i++) large_arr[i] = -i;

    // ===================================================================
    // PHASE 3: Verify Data Integrity
    // ===================================================================
    putstr("[Malloc Test] Verifying data integrity...\n");
    int ok = 1;

    for (int i = 0; i < 100; i++)
    {
        if (arr1[i] != i) ok = 0;
    }
    for (int i = 0; i < 250; i++)
    {
        if (arr2[i] != i * 2) ok = 0;
    }
    for (int i = 0; i < 50000; i++)
    {
        if (large_arr[i] != -i) ok = 0;
    }

    if (ok)
    {
        putstr("[Malloc Test] SUCCESS: All data matches perfectly!\n");
    }
    else
    {
        putstr("[Malloc Test] FAILURE: Data corruption or overlap detected!\n");
    }

    // ===================================================================
    // PHASE 4: Free Memory
    // ===================================================================
    putstr("[Malloc Test] Freeing memory...\n");
    free(arr1);
    free(arr2);
    free(large_arr);

    putstr("[Malloc Test] Test finished.\n");
    return 0;
}