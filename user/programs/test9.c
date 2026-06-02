#include "stdio.h"
#include "syscall.h"

void bad_worker(void* arg)
{
    putstr("   [Worker] I am going to access invalid memory now, there should be no print after this...\n");

    // Attempt to write to NULL address (0x0)
    // This generates a Store Page Fault
    volatile int* bad_ptr = (int*)0x0;
    *bad_ptr = 42;

    putstr("   [Worker] I survived! (This should not print)\n");
    thread_exit(0);
}

int main()
{
    putstr("[Main] Creating crash worker...\n");
    int tid = thread_create(bad_worker, 0);

    putstr("[Main] Waiting for worker (It should crash the process, so other things should not print)...\n");

    int result = thread_join(tid);

    // If your kernel kills the whole process, WE WILL NEVER REACH HERE.
    // If your kernel only kills the thread, this will print.
    putstr("[Main] Worker finished with code: ");
    if (result == -1)
        putstr("-1 (Kernel Killed It)\n");
    else
        putstr("Something else?\n");

    return 0;
}