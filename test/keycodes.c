#include <ctype.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ascii.h"
#include "keys.h"

#define countof(array) (sizeof(array) / sizeof(array[0]))


int main(void)
{
    printf("type keys (or key combinations), one at a time\r\n");
    printf("type accent grave (`) to exit\r\n");
    printf("type enter to begin: ");
    (void)getc(stdin);

    initscr();
    noecho();
    keypad(stdscr, TRUE);

    int c;

    while ((c = getch()) != '`')
    {
        if (c == ERR)
        {
            printf("no character\r\n");
            sleep(1);

            continue;
        }

        printf("[%d] (0%o): ", c, c);

        if (c >= KEY_MIN)
        {
            if (c < countof(keys) && keys[c].kname != NULL)
            {
                printf("%s", keys[c].kname);
            }
            else
            {
                printf("(unknown)");
            }
        }
        else if (c == 0x7F)
        {
            printf("<DEL>");
        }
        else if (iscntrl(c))
        {
            printf("<^%c>", c + 'A' - 1);
        }
        else if (isprint(c))
        {
            putchar(c);
        }

        printf("\r\n");

        fflush(stdout);
    }

    echo();
    endwin();
    
    return EXIT_SUCCESS;
}
