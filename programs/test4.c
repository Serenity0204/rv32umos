#include "stdio.h"
#include "stdlib.h"

int main()
{
    char buffer[128];

    while (1)
    {
        putstr("Mini Shell> ");

        int len = getline(buffer, 128);

        if (len <= 0) break;

        if (buffer[0] == 'e' && buffer[1] == 'x' && buffer[2] == 'i' && buffer[3] == 't' && buffer[4] == '\n')
        {
            putstr("Bye!\n");
            break;
        }

        putstr("Echo: ");
        putstr(buffer);
    }

    return 2026;
}