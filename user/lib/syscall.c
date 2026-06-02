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
    return syscall(SYS_WRITE, fd, (int)buf, count);
}

int read(int fd, void* buf, int count)
{
    if (count < 0) return -1;
    return syscall(SYS_READ, fd, (int)buf, count);
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

int thread_join(int tid)
{
    return syscall(SYS_THREAD_JOIN, tid);
}

mutex_t mutex_create()
{
    return syscall(SYS_MUTEX_CREATE);
}

void mutex_lock(mutex_t* lock)
{
    if (lock == NULL) exit(-1);
    int status = syscall(SYS_MUTEX_LOCK, *lock);
    if (status == -1) exit(-1);
}

void mutex_unlock(mutex_t* lock)
{
    if (lock == NULL) exit(-1);
    int status = syscall(SYS_MUTEX_UNLOCK, *lock);
    if (status == -1) exit(-1);
}

int open(const char* filename)
{
    return syscall(SYS_OPEN, (int)filename);
}

int close(int fd)
{
    return syscall(SYS_CLOSE, fd);
}

int create(const char* filename, int sizeBytes)
{
    if (sizeBytes <= 0) return -1;
    return syscall(SYS_CREATE, (int)filename, sizeBytes);
}

void* sbrk(int increment)
{
    return (void*)syscall(SYS_SBRK, increment);
}

void* malloc(int size)
{
    if (size <= 0) return NULL;

    // align to 8 bytes for now
    size = (size + 7) & ~7;
    void* ptr = sbrk(size);
    return ((int)ptr == -1) ? NULL : ptr;
}

void free(void*)
{
}

int create_process(const char* filename)
{
    return syscall(SYS_CREATE_PROCESS, (int)filename);
}

int join_process(int pid, int* status)
{
    return syscall(SYS_JOIN_PROCESS, pid, (int)status);
}