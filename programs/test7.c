#include "stdio.h"
#include "syscall.h"

void worker(void* arg)
{
    for (int i = 0; i < 100; i++)
    {
        putstr("   [Worker] I am alive...\n");
        // Burn cycles
        for (volatile int j = 0; j < 1000; j++);
    }
    putstr("   [Worker] I finished! (This should not print)\n");
    thread_exit(0);
}

int main()
{
    putstr("[Main] Creating worker...\n");
    thread_create(worker, (void*)0);

    putstr("[Main] Worker created. I will sleep briefly.\n");
    for (volatile int j = 0; j < 2000; j++);

    putstr("[Main] BOOM! Calling exit_group()...\n");

    // This should kill Main AND the Worker immediately
    exit(99);

    return 0;
}