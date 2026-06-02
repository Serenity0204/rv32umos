#include "stdio.h"
#include "stdlib.h"
#include "syscall.h"

int main()
{
    putstr("[PARENT] Starting Flat Process Test...\n");
    putstr("[PARENT] Calling create_process(\"child.elf\")...\n");

    // Spawn the new program natively!
    int child_pid = create_process("programs/test14.elf");

    if (child_pid < 0)
    {
        putstr("[ERROR] Failed to create process! Is child.elf on the disk?\n");
        return 1;
    }

    putstr("[PARENT] Successfully spawned child!\n");
    putstr("[PARENT] Calling join_process() and blocking until it finishes...\n");

    int status = 0;

    // Block safely until the child exits
    int result = join_process(child_pid, &status);

    putstr("[PARENT] Woke up! join_process() returned.\n");

    // Verify the results
    if (result == 1)
    {
        putstr("[PARENT] Successfully joined the child!\n");

        if (status == 99)
        {
            putstr("[PARENT] SUCCESS: Child returned exit code 99!\n");
        }
        else
        {
            putstr("[PARENT] FAILURE: Expected 99, but got a different code.\n");
        }
    }
    else
    {
        putstr("[PARENT] FAILURE: join_process returned an error!\n");
    }

    putstr("[PARENT] Test Complete. Exiting.\n");
    return 123;
}