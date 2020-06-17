#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int count = 1;
    int length = 50;
    int ff = 0;

    if (--argc)
    {
        count = strtol(argv[1], NULL, 10);

        if (count <= 0)
        {
            printf("count cannot be <= 0\n");

            return EXIT_FAILURE;
        }

        if (--argc)
        {
            ff = strtol(argv[2], NULL, 10);

            if (ff < 0)
            {
                printf("ff cannot be < 0\n");

                return EXIT_FAILURE;
            }

            if (--argc)
            {
                length = strtol(argv[3], NULL, 10);

                if (length != 50 && length != 64)
                {
                    printf("length must be 50 or 64\n");

                    return EXIT_FAILURE;
                }
            }
        }
    }

    for (int i = 1; i <= count; ++i)
    {
        int nbytes = printf("Line %05d: ", i) + 1;

        printf("%.*s\n", length - nbytes, "abcdefghijklmnopqrstuvwxyz "
               "0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ");

        if (ff != 0 && !(i % ff))
        {
            putchar('\f');
        }
    }

    return EXIT_SUCCESS;
}
