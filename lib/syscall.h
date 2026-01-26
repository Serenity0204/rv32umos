#ifndef SYSCALL_H
#define SYSCALL_H

// System Call IDs
#define SYS_EXIT 0
#define SYS_WRITE 1
#define SYS_READ 2
#define SYS_THREAD_CREATE 3
#define SYS_THREAD_EXIT 4

void exit(int code);
int write(int fd, void* buf, int count);
int read(int fd, void* buf, int count);
int thread_create(void (*func)(void*), void* arg);
void thread_exit(int status);
#endif