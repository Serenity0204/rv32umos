#ifndef SYSCALL_H
#define SYSCALL_H

// System Call IDs
#define SYS_EXIT 0
#define SYS_WRITE 1

void exit(int code);
int write(int fd, void* buf, int count);

#endif