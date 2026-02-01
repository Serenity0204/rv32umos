#include "stdio.h"
#include "syscall.h"

void math_worker(void* arg)
{
    int max = (int)arg;
    putstr("   [Worker] Calculating sum from 1 to ");
    putint(max);
    putstr("...\n");

    int sum = 0;
    for (int i = 1; i <= max; i++)
    {
        sum += i;
        // busy wait to simulate hard work
        for (volatile int j = 0; j < 100; j++);
    }

    putstr("   [Worker] Finished. Returning result.\n");
    thread_exit(sum); // Return the calculation result
}

int main()
{
    putstr("[Main] Starting Join Test...\n");

    // Calculate sum(1..10) which should be 55
    int tid = thread_create(math_worker, (void*)10);

    putstr("[Main] Created thread ");
    putint(tid);
    putstr(". Waiting for it to finish...\n");

    // BLOCK here until worker is done
    int result = thread_join(tid);

    putstr("[Main] Thread joined! Return code: ");
    putint(result);
    putstr("\n");

    if (result == 55)
    {
        putstr("[Main] SUCCESS: Result is correct (55).\n");
    }
    else
    {
        putstr("[Main] FAILURE: Result is wrong.\n");
    }

    return 0;
}