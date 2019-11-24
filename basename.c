#define _GNU_SOURCE

#include <libgen.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc <= 2)
        return 0;

    char *file = basename(argv[2]);
    char *dir = dirname(argv[1]);

    printf("dir = %s, file = %s\n", dir, file);

    return 0;
}

