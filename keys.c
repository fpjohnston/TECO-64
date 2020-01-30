#include <ctype.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>


#define KEY(key) { key, #key }

#define KEY_F1          (KEY_F(1))
#define KEY_F2          (KEY_F(2))
#define KEY_F3          (KEY_F(3))
#define KEY_F4          (KEY_F(4))
#define KEY_F5          (KEY_F(5))
#define KEY_F6          (KEY_F(6))
#define KEY_F7          (KEY_F(7))
#define KEY_F8          (KEY_F(8))
#define KEY_F9          (KEY_F(9))
#define KEY_F10         (KEY_F(10))
#define KEY_F11         (KEY_F(11))
#define KEY_F12         (KEY_F(12))

#define KEY_SF1         (KEY_F(1)  + 12)
#define KEY_SF2         (KEY_F(2)  + 12)
#define KEY_SF3         (KEY_F(3)  + 12)
#define KEY_SF4         (KEY_F(4)  + 12)
#define KEY_SF5         (KEY_F(5)  + 12)
#define KEY_SF6         (KEY_F(6)  + 12)
#define KEY_SF7         (KEY_F(7)  + 12)
#define KEY_SF8         (KEY_F(8)  + 12)
#define KEY_SF9         (KEY_F(9)  + 12)
#define KEY_SF10        (KEY_F(10) + 12)
#define KEY_SF11        (KEY_F(11) + 12)
#define KEY_SF12        (KEY_F(12) + 12)

#define KEY_CF1         (KEY_F(1)  + 24)
#define KEY_CF2         (KEY_F(2)  + 24)
#define KEY_CF3         (KEY_F(3)  + 24)
#define KEY_CF4         (KEY_F(4)  + 24)
#define KEY_CF5         (KEY_F(5)  + 24)
#define KEY_CF6         (KEY_F(6)  + 24)
#define KEY_CF7         (KEY_F(7)  + 24)
#define KEY_CF8         (KEY_F(8)  + 24)
#define KEY_CF9         (KEY_F(9)  + 24)
#define KEY_CF10        (KEY_F(10) + 24)
#define KEY_CF11        (KEY_F(11) + 24)
#define KEY_CF12        (KEY_F(12) + 24)

#define KEY_CSF1        (KEY_F(1)  + 36)
#define KEY_CSF2        (KEY_F(2)  + 36)
#define KEY_CSF3        (KEY_F(3)  + 36)
#define KEY_CSF4        (KEY_F(4)  + 36)
#define KEY_CSF5        (KEY_F(5)  + 36)
#define KEY_CSF6        (KEY_F(6)  + 36)
#define KEY_CSF7        (KEY_F(7)  + 36)
#define KEY_CSF8        (KEY_F(8)  + 36)
#define KEY_CSF9        (KEY_F(9)  + 36)
#define KEY_CSF10       (KEY_F(10) + 36)
#define KEY_CSF11       (KEY_F(11) + 36)
#define KEY_CSF12       (KEY_F(12) + 36)

static struct keys
{
    int value;
    const char *string;
} keys[] =
{
    KEY(KEY_BREAK),
    KEY(KEY_SRESET),
    KEY(KEY_RESET),
    KEY(KEY_DOWN),
    KEY(KEY_UP),
    KEY(KEY_LEFT),
    KEY(KEY_RIGHT),
    KEY(KEY_HOME),
    KEY(KEY_BACKSPACE),
    KEY(KEY_F0),
    KEY(KEY_F1),
    KEY(KEY_F2),
    KEY(KEY_F3),
    KEY(KEY_F4),
    KEY(KEY_F5),
    KEY(KEY_F6),
    KEY(KEY_F7),
    KEY(KEY_F8),
    KEY(KEY_F9),
    KEY(KEY_F10),
    KEY(KEY_F11),
    KEY(KEY_F12),
    KEY(KEY_SF1),
    KEY(KEY_SF2),
    KEY(KEY_SF3),
    KEY(KEY_SF4),
    KEY(KEY_SF5),
    KEY(KEY_SF6),
    KEY(KEY_SF7),
    KEY(KEY_SF8),
    KEY(KEY_SF9),
    KEY(KEY_SF10),
    KEY(KEY_SF11),
    KEY(KEY_SF12),
    KEY(KEY_CF1),
    KEY(KEY_CF2),
    KEY(KEY_CF3),
    KEY(KEY_CF4),
    KEY(KEY_CF5),
    KEY(KEY_CF6),
    KEY(KEY_CF7),
    KEY(KEY_CF8),
    KEY(KEY_CF9),
    KEY(KEY_CF10),
    KEY(KEY_CF11),
    KEY(KEY_CF12),
    KEY(KEY_CSF1),
    KEY(KEY_CSF2),
    KEY(KEY_CSF3),
    KEY(KEY_CSF4),
    KEY(KEY_CSF5),
    KEY(KEY_CSF6),
    KEY(KEY_CSF7),
    KEY(KEY_CSF8),
    KEY(KEY_CSF9),
    KEY(KEY_CSF10),
    KEY(KEY_CSF11),
    KEY(KEY_CSF12),
    KEY(KEY_DL),
    KEY(KEY_IL),
    KEY(KEY_DC),
    KEY(KEY_IC),
    KEY(KEY_EIC),
    KEY(KEY_CLEAR),
    KEY(KEY_EOS),
    KEY(KEY_EOL),
    KEY(KEY_SF),
    KEY(KEY_SR),
    KEY(KEY_NPAGE),
    KEY(KEY_PPAGE),
    KEY(KEY_STAB),
    KEY(KEY_CTAB),
    KEY(KEY_CATAB),
    KEY(KEY_ENTER),
    KEY(KEY_PRINT),
    KEY(KEY_LL),
    KEY(KEY_A1),
    KEY(KEY_A3),
    KEY(KEY_B2),
    KEY(KEY_C1),
    KEY(KEY_C3),
    KEY(KEY_BTAB),
    KEY(KEY_BEG),
    KEY(KEY_CANCEL),
    KEY(KEY_CLOSE),
    KEY(KEY_COMMAND),
    KEY(KEY_COPY),
    KEY(KEY_CREATE),
    KEY(KEY_END),
    KEY(KEY_EXIT),
    KEY(KEY_FIND),
    KEY(KEY_HELP),
    KEY(KEY_MARK),
    KEY(KEY_MESSAGE),
    KEY(KEY_MOVE),
    KEY(KEY_NEXT),
    KEY(KEY_OPEN),
    KEY(KEY_OPTIONS),
    KEY(KEY_PREVIOUS),
    KEY(KEY_REDO),
    KEY(KEY_REFERENCE),
    KEY(KEY_REFRESH),
    KEY(KEY_REPLACE),
    KEY(KEY_RESTART),
    KEY(KEY_RESUME),
    KEY(KEY_SAVE),
    KEY(KEY_SBEG),
    KEY(KEY_SCANCEL),
    KEY(KEY_SCOMMAND),
    KEY(KEY_SCOPY),
    KEY(KEY_SCREATE),
    KEY(KEY_SDC),
    KEY(KEY_SDL),
    KEY(KEY_SELECT),
    KEY(KEY_SEND),
    KEY(KEY_SEOL),
    KEY(KEY_SEXIT),
    KEY(KEY_SFIND),
    KEY(KEY_SHELP),
    KEY(KEY_SHOME),
    KEY(KEY_SIC),
    KEY(KEY_SLEFT),
    KEY(KEY_SMESSAGE),
    KEY(KEY_SMOVE),
    KEY(KEY_SNEXT),
    KEY(KEY_SOPTIONS),
    KEY(KEY_SPREVIOUS),
    KEY(KEY_SPRINT),
    KEY(KEY_SREDO),
    KEY(KEY_SREPLACE),
    KEY(KEY_SRIGHT),
    KEY(KEY_SRSUME),
    KEY(KEY_SSAVE),
    KEY(KEY_SSUSPEND),
    KEY(KEY_SUNDO),
    KEY(KEY_SUSPEND),
    KEY(KEY_UNDO),
    KEY(KEY_MOUSE),
    KEY(KEY_RESIZE),
    KEY(KEY_EVENT),
};

#define countof(array) (sizeof(array) / sizeof(array[0]))

void print_key(int c)
{
    for (int i = 0; i < countof(keys); ++i)
    {
        if (c == keys[i].value)
        {
            printf("%s (%d)\r\n", keys[i].string, c);

            return;
        }
    }

    printf("[%02x]", c);
}

bool winactive = false;

void stopwin(void)
{
    if (winactive)
    {
        winactive = false;
        endwin();
    }
}

int main(void)
{
    initscr();
    cbreak();
    noecho();
    nonl();
    notimeout(stdscr, TRUE);
    idlok(stdscr, TRUE);
    scrollok(stdscr, TRUE);
    keypad(stdscr, TRUE);

    winactive = true;

    if (atexit(stopwin) != 0)
    {
        stopwin();

//        print_err(E_WIN);
        printf("atexit() failure\n");
        abort();
    }

    int c;

    while ((c = getch()) != 96)
    {
        if (c >= KEY_MIN && c <= KEY_MAX)
        {
            print_key(c);
        }
        else if (c == 10)
        {
            putchar(13);
            putchar(c);
        }
        else if (isprint(c))
        {
            putchar(c);
        }
        else
        {
            printf("[%02x]", c);
        }

        fflush(stdout);
    }

    return EXIT_SUCCESS;
}
