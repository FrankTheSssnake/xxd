#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "parse.h"

void parse(int fdi, int fdo)
{
    int offset = 0x0;
    uint8_t cols = 16;

    uint8_t size = cols;

    unsigned char *buf = malloc(size * sizeof(char));
    if (buf == NULL)
    {
        fprintf(stderr, "Memory Allocation Failed.\n");
        exit(-1);
    }

    ssize_t n;

    while ((n = read(fdi, buf, (int)size)) > 0)
    {
        printf("%.8X: ", offset);

        int i;

        for (i = 0; i < n; ++i)
        {
            printf("%.2x", buf[i]);
            if (i % 2 == 1) printf(" ");
        }

        for (; i < size; ++i)
        {
            printf("  ");
            if (i % 2 == 1) printf(" ");
        }

        printf(" ");

        for (i = 0; i < n; ++i)
        {
            printf("%c", buf[i]);
        }

        printf("\n");

        offset += n;
    }
}
