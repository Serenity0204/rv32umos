#include "stdio.h"
#include "syscall.h"

volatile int counter = 0;
mutex_t lock;

void worker(void* arg)
{
    int id = (int)arg;

    // Run only 100 times.
    for (int i = 0; i < 100; i++)
    {
        mutex_lock(&lock);

        // --- CRITICAL SECTION ---
        int temp = counter;

        // Tiny delay to force the lock to be held
        // across timer ticks (provokes conflicts).
        for (volatile int j = 0; j < 20; j++);

        counter = temp + 1;
        // ------------------------

        mutex_unlock(&lock);
    }
    thread_exit(0);
}

int main()
{
    putstr("[Main] Starting Short Mutex Test (Target: 300)...\n");

    lock = mutex_create();

    // Spawn 3 threads
    int t1 = thread_create(worker, (void*)1);
    int t2 = thread_create(worker, (void*)2);
    int t3 = thread_create(worker, (void*)3);

    // Wait for them
    thread_join(t1);
    thread_join(t2);
    thread_join(t3);

    putstr("[Main] Final Counter: ");
    putint(counter);
    putstr("\n");

    if (counter == 300)
    {
        putstr("[Main] SUCCESS.\n");
    }
    else
    {
        putstr("[Main] FAILURE. Expected 300.\n");
    }

    return 0;
}