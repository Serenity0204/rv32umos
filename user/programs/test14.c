#include "stdio.h"
#include "stdlib.h"
#include "syscall.h"

int main()
{
    putstr("[CHILD] Hello! I am the spawned process.\n");
    exit(99);
    return 0;
}