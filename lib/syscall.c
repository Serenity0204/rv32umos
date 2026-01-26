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

int read(int fd, void* buf, int count)
{
    if (count < 0) return -1;
    return syscall(SYS_READ, fd, buf, count);
}

int thread_create(void (*func)(void*), void* arg)
{
    return syscall(SYS_THREAD_CREATE, (int)func, (int)arg);
}

void thread_exit(int status)
{
    syscall(SYS_THREAD_EXIT, status);
    while (1);
}