#include "syscall.h"
#include "sysdef.h"

void exit(int code)
{
    syscall(SYS_EXIT, code);
    while (1);
}

int write(int fd, void* buf, int count)
{
    if (count < 0) return -1;
    return syscall(SYS_WRITE, fd, buf, count);
}