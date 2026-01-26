#include "stdio.h"
#include "stdlib.h"
#include "syscall.h"

void worker(void* arg)
{
    int id = (int)arg;

    for (int i = 0; i < 3; i++)
    {
        if (id == 1)
            putstr("   [Worker] Thread 1 is running...\n");
        else
            putstr("   [Worker] Thread 2 is running...\n");

        // Busy wait loop to burn CPU cycles and force a context switch
        for (volatile int j = 0; j < 500; j++);
    }

    putstr("   [Worker] Thread Finished.\n");
    thread_exit(0);
}

int main()
{
    putstr("[Main] Starting Multi-Threading Test...\n");

    // --- Create Thread 1 ---
    // We pass the function pointer 'worker' and argument '1'
    int tid1 = thread_create(worker, (void*)1);

    if (tid1 < 0)
        putstr("[Main] Error: Failed to create Thread 1.\n");
    else
        putstr("[Main] Successfully created Thread 1.\n");

    // --- Create Thread 2 ---
    int tid2 = thread_create(worker, (void*)2);

    if (tid2 < 0)
        putstr("[Main] Error: Failed to create Thread 2.\n");
    else
        putstr("[Main] Successfully created Thread 2.\n");

    // --- Main Thread Work ---
    // The main thread continues running too!
    for (int i = 0; i < 5; i++)
    {
        putstr("[Main] Main Loop running...\n");
        for (volatile int j = 0; j < 500; j++);
    }

    putstr("[Main] Main thread exiting.\n");
    return 0;
}