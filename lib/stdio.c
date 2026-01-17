#include "stdio.h"
#include "syscall.h"

void putchar(char c)
{
    write(STDOUT, &c, 1);
}

void putstr(const char* s)
{
    int len = 0;
    while (s[len] != '\0') len++;
    write(STDOUT, (void*)s, len);
}

int getchar()
{
    char c;

    int bytes_read = read(STDIN, &c, 1);

    if (bytes_read == 1) return (unsigned char)c;
    return EOF;
}

int getline(char* buf, int max_len)
{

    if (max_len <= 0) return 0;

    // Request the full buffer size in ONE system call.
    int bytes_read = read(STDIN, buf, max_len - 1);

    if (bytes_read <= 0)
    {
        buf[0] = '\0';
        return 0;
    }

    buf[bytes_read] = '\0';

    return bytes_read;
}