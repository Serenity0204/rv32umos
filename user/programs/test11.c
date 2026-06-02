#include "stdio.h"
#include "stdlib.h"
#include "syscall.h"

int main()
{
    // 1. Create a new file of size 1KB
    int ret = create("test.txt", 1024);

    if (ret < 0)
    {
        putstr("Error creating file (Disk full or already exists).\n");
        return 1;
    }

    // 2. Open it
    int fd = open("test.txt");

    if (fd < 0)
    {
        putstr("Failed to open file!\n");
        return 1;
    }

    // 3. Write to it (Write to RAM Disk)
    char msg[] = "Hello World From Doing File I/O!\n";
    unsigned int str_len = strlen(msg);
    write(fd, msg, str_len);

    // 4. Seek back to start (You need lseek syscall for this, or close/reopen)
    close(fd);

    fd = open("test.txt");
    // 5. Read back (Read from RAM Disk)
    char buf[100];
    int bytes_read = read(fd, buf, str_len);
    if (bytes_read > 0) buf[bytes_read] = '\0';

    // 6. Print to Console (Write to Host Stdout)
    putstr(buf); // Should print "Hello Disk!"
    putstr("\n");

    return 0;
}