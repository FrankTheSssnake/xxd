#include <unistd.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "parse.h"


int main(int argc, char **argv)
{
    int opt;
    long long int buffer = 0;

    struct options opts = {
        .cols = 16,
        .group_size = 2,
        .decimal = 0,
        .endianness = 0,
        .capital = 0,
        .relative = 0,

        .seek = 0,
        .offset = 0,
    };

    while ((opt = getopt(argc, argv, "abCEehirduvc:g:l:n:o:s:R:")) != -1)
    {
        switch (opt)
        {
            case 'h':
                print_help();
                break;
            case 'v':
                puts("ccd 2025-12-31 by frankhissss");
                break;
            case 'd':
                opts.decimal = 1;
                break;
            case 'u':
                opts.capital = 1;
                break;
            case 'c':
                buffer = atoll(optarg);
                if (buffer > 256)
                {
                    fprintf(stderr, "ccd: invalid number of columns (max. 256)\n");
                    exit(1);
                }

                if (buffer == 0) buffer = 16;

                opts.cols = (uint16_t)buffer;
                break;
            case 'e':
                opts.endianness = 1;
                opts.group_size = 4;
                break;
            case 'g':
                buffer = atoll(optarg);
                if (buffer > 256)
                {
                    fprintf(stderr, "ccd: invalid value of group_size (max. 256)\n");
                    exit(1);
                }
                // check for endianness
                if (opts.endianness)
                {
                    uint16_t mask;
                    for (mask = 2; mask <= buffer; mask <<= 2)
                    {
                        if (mask == buffer)
                        {
                            mask = 0;
                            break;
                        }
                    }

                    if (!mask)
                    {
                        fprintf(stderr, "ccd: number of octets per group must be a power of 2 with -e.\n");
                        exit(1);
                    }
                }

                if (buffer == 0) buffer = opts.cols;

                opts.group_size = (uint16_t)buffer;
                break;
            case 'o':
                buffer = atoll(optarg);
                opts.offset = (off_t)buffer;
                break;
            case 's':
                if (optarg[0] == '+')
                {
                    opts.relative = 1;
                    optarg[0] = ' ';
                }

                buffer = atoll(optarg);
                opts.seek = (off_t)buffer;
                break;
            case 'E':
            // https://www.ibm.com/docs/en/iis/11.3.0?topic=tables-ebcdic-ascii
            case 'a':
            case 'b':
            case 'C':
            case 'i':
            case 'l':
            case 'n':
            case 'r':
            case 'R':
                fprintf(stderr, "NOT IMPLEMENTED!\n");
                exit(-1);
            default:
                fprintf(stderr, "UNSUPPORTED OPERATION!\n");
                exit(-1);
        }
    }

    int fdi = STDIN_FILENO;
    int fdo = STDOUT_FILENO;

    int remaining = argc - optind;

    if (remaining > 2) {
        fprintf(stderr, "ccd: too many arguments\n");
        exit(1);
    }

    if (remaining >= 1) {
        fdi = open(argv[optind], O_RDONLY);
        if (fdi < 0) {
            perror(argv[optind]);
            exit(2);
        }
    }

    if (remaining == 2) {
        fdo = open(argv[optind + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fdo < 0) {
            perror(argv[optind + 1]);
            exit(3);
        }
    }

    if (opts.seek != 0)
    {
        opts.offset += seek(fdi, opts.seek, opts.relative);
    }

    parse(fdi, fdo, &opts);

    return 0;
}

