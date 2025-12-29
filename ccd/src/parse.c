#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "parse.h"


// Lookup tables
const char *LOWER = "0123456789abcdef";
const char *UPPER = "0123456789ABCDEF";
const char *LOOKUP;


// function pointers to be resolved depending on the flags
size_t (*fmt_offset)(char*, off_t);
size_t (*fmt_data)(char*, char*, ssize_t, size_t, uint16_t);


// write `offset` into `line` as decimal
size_t fmt_offset_dec(char *line, off_t offset)
{
    for (int8_t pos = 7; pos >= 0; --pos)
    {
        line[pos] = LOOKUP[offset % 10];
        offset /= 10;
    }

    return (size_t)8;
}

// write `offset` into `line` as hex
size_t fmt_offset_hex(char *line, off_t offset)
{
    for (int8_t pos = 7; pos >= 0; --pos)
    {
        line[pos] = LOOKUP[offset & 0xF];
        offset >>= 4;
    }

    return (size_t)8;
}


// write data into `line` in little endian
size_t fmt_data_little(char *line, char *buf, ssize_t n, size_t pos, uint16_t group_size)
{
    uint16_t group_count = 0;

    for (size_t i = group_size - 1; i < n; --i)
    {
        char b = buf[i];

        line[pos++] = LOOKUP[b >> 4];
        line[pos++] = LOOKUP[b & 0x0F];

        if (++group_count == group_size)
        {
            // TODO
            // if this goes over n, the loops ends immediately leaving some bytes unwritten
            i += (2 * group_size);
            line[pos++] = ' ';
            group_count = 0;
        }
    }

    return pos;
}

// write data into `line` in big endian
size_t fmt_data_big(char *line, char *buf, ssize_t n, size_t pos, uint16_t group_size)
{
    uint16_t group_count = 0;

    for (size_t i = 0; i < n; ++i)
    {
        char b = buf[i];

        line[pos++] = LOOKUP[b >> 4];
        line[pos++] = LOOKUP[b & 0x0F];

        if (++group_count == group_size)
        {
            line[pos++] = ' ';
            group_count = 0;
        }
    }

    return pos;
}


void parse_opts(struct options *opts)
{
    fmt_offset = opts->decimal ? fmt_offset_dec : fmt_offset_hex;

    fmt_data = opts->endianness ? fmt_data_little : fmt_data_big;

    LOOKUP = opts->capital ? UPPER : LOWER;
}


void parse(int fdi, int fdo, struct options *opts)
{
    // set globals
    parse_opts(opts);

    uint8_t size = opts->cols;

    // `buf` to read data into
    char *buf = malloc(size * sizeof(char));
    if (buf == NULL)
    {
        perror("Memory Allocation Failed.\n");
        exit(1);
    }

    // length       = offset (8 characters wide) + colon + 2 whitespaces (beginning/end) + `ceil(size / group_size)`                                    + ascii (size) + data (2*size)
    size_t length   = 11                                                                 + ((size / opts->group_size) + (size % opts->group_size != 0)) + (3 * size);

    char *line = malloc((length + 2) * sizeof(char)); // +2 for LF and null characters
    if (line == NULL)
    {
        perror("Memory Allocation Failed.\n");
        exit(1);
    }
    line[length] = '\n';
    line[length + 1] = '\0';

    // seek to given `offset`
    off_t offset = opts->offset;

    ssize_t n;
    size_t pos;

    while ((n = read(fdi, buf, (int)size)) > 0)
    {
        if (n < size)
        {
            memset(line, ' ', length);
        }

        // write `offset` in `line`
        pos = fmt_offset(line, offset);
        line[pos++] = ':';
        line[pos++] = ' ';

        pos = fmt_data(line, buf, n, pos, opts->group_size);

        // fill the remaining cols with spaces
        while (pos < length - size)
        {
            line[pos++] = ' ';
        }

        // write the ascii data into `line`
        for (size_t i = 0; i < n; ++i)
        {
            char b = buf[i];
            line[pos++] = b >= 32 ? b : '.';
        }

        // write `line` to output
        write(fdo, line, length + 1);

        offset += (size_t)n;
    }
}

