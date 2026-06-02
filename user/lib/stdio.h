#ifndef STDIO_H
#define STDIO_H

#define EOF -1
#define STDIN 0
#define STDOUT 1
#define STDERR 2

void putchar(char c);
void putstr(const char* s);
void putint(int n);

int getchar();
int getline(char* buf, int max_len);

#endif