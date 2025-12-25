#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "help.h"
#include "parse.h"


int main(int argc, char **argv)
{
    int opt;

    while ((opt = getopt(argc, argv, "abCEehirduvc:g:l:n:o:s:R:")) != -1)
    {
        switch (opt)
        {
            case 'h':
                print_help();
                break;
            case 'a':
            case 'b':
            case 'C':
            case 'c':
            case 'E':
            case 'e':
            case 'g':
            case 'i':
            case 'l':
            case 'n':
            case 'o':
            case 'r':
            case 'd':
            case 's':
            case 'u':
            case 'R':
            case 'v':
                puts("NOT IMPLEMENTED!\n");
                break;
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
        exit(-1);
    }

    if (remaining >= 1) {
        fdi = open(argv[optind], O_RDONLY);
        if (fdi < 0) {
            perror(argv[optind]);
            exit(-1);
        }
    }

    if (remaining == 2) {
        fdo = open(argv[optind + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fdo < 0) {
            perror(argv[optind + 1]);
            exit(-1);
        }
    }

    parse(fdi, fdo);

    return 0;
}

