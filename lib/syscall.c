#include "syscall.h"
#include "sysdef.h"

void exit(int code)
{
    syscall(SYS_EXIT, code);
    while (1);
}