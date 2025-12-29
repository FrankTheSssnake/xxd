#ifndef PARSE_H
#define PARSE_H

#include <stdint.h>
#include <unistd.h>

struct options {
    uint16_t cols;
    uint16_t group_size;
    uint8_t decimal;
    uint8_t endianness;
    uint8_t capital;
    uint8_t relative;

    off_t seek;
    off_t offset;
};

void parse(int, int, struct options *);

#endif
