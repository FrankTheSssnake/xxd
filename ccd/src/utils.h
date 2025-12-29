#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>
#include <stdint.h>

void print_help();

off_t seek(int fdi, off_t, uint8_t);

#endif
