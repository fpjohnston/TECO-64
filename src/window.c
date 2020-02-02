///
///  @file    window.c
///  @brief   Window functions.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
///
///  Permission is hereby granted, free of charge, to any person obtaining a
///  copy of this software and associated documentation files (the "Software"),
///  to deal in the Software without restriction, including without limitation
///  the rights to use, copy, modify, merge, publish, distribute, sublicense,
///  and/or sell copies of the Software, and to permit persons to whom the
///  Software is furnished to do so, subject to the following conditions:
///
///  The above copyright notice and this permission notice shall be included in
///  all copies or substantial portions of the Software.
///
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIA-
///  BILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
///  THE SOFTWARE.
///
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ioctl.h>

#if     defined(NCURSES)

#include <ncurses.h>

#endif

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "textbuf.h"
#include "window.h"


#define CMD             0               ///< Command window colors
#define TEXT            1               ///< Text window colors
#define LINE            2               ///< Line separator colors

#define BRIGHT_WHITE    16              ///< Bright white background

#if     defined(NCURSES)

static int text_top;

#endif


///
///  @brief    Clear screen and redraw window.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void clear_win(void)
{
    (void)clear();

    set_scroll(w.height, w.nscroll);

    refresh_win();
}


///
///  @brief    Get next character.
///
///  @returns  Character read, or -1 if no character available.
///
////////////////////////////////////////////////////////////////////////////////

int getchar_win(bool wait)
{
    int c = 0;                          // Ensure that high bits are clear

#if     defined(NCURSES)

    if (f.e0.winact)
    {
        if (!wait)
        {
            (void)nodelay(stdscr, (bool)TRUE);
        }

        c = getch();

        if (!wait)
        {
            (void)nodelay(stdscr, (bool)FALSE);
        }

        if (c == KEY_BACKSPACE)
        {
            return DEL;
        }

        return c;
    }

#endif

    // If windows support isn't compiled in, or it's not currently active, then
    // read a character through alternate means. Note that if windows are inactive,
    // getch() always returns immediately, which is why we usually call read() to
    // get a single character.

    if (!wait)
    {
        return getch();
    }
    else if (read(fileno(stdin), &c, 1uL) == -1)
    {
        return -1;
    }
    else
    {
        return c;
    }
}


///
///  @brief    Get window size.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void getsize_win(void)
{

#if     defined(NCURSES)

    if (f.e0.winact)
    {
        getmaxyx(stdscr, w.height, w.width);

        return;
    }

#endif

    struct winsize ts;

    if (ioctl(fileno(stdin), (ulong)TIOCGWINSZ, &ts) == -1)
    {
        fatal_err(errno, E_SYS, NULL);
    }

    w.width  = ts.ws_col;
    w.height = ts.ws_row;
}


///
///  @brief    Initialize for window display.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_win(void)
{

#if     defined(NCURSES)

    if (!f.e0.winact)
    {
        f.e0.winact = true;

        static bool end_set = false;

        if (!end_set)
        {
            end_set = true;

            if (atexit(reset_win) != 0)
            {
                print_err(E_WIN);
            }
        }

        // Note that initscr() will print an error message and exit if it
        // fails to initialize, so there is no error return to check for.

        (void)initscr();

        if (cbreak()                                                == ERR ||
            noecho()                                                == ERR ||
            nonl()                                                  == ERR ||
            notimeout(stdscr, (bool)TRUE)                           == ERR ||
            idlok(stdscr, (bool)TRUE)                               == ERR ||
            scrollok(stdscr, (bool)TRUE)                            == ERR ||
            keypad(stdscr, (bool)TRUE)                              == ERR ||
            !has_colors()                                                  ||
            start_color()                                           == ERR ||
            assume_default_colors(COLOR_BLUE, COLOR_WHITE | A_BOLD) == ERR)
        {
            print_err(E_WIN);
        }

        ESCDELAY = 0;

        short color_bg = COLOR_WHITE;

        if (can_change_color() && COLORS >= 16)
        {
            (void)init_color(BRIGHT_WHITE, 1000, 1000, 1000);

            color_bg = BRIGHT_WHITE;
        }

        (void)init_pair(TEXT, COLOR_GREEN, color_bg);
        (void)init_pair(LINE, COLOR_RED, color_bg);
        (void)attrset(COLOR_PAIR(CMD)); //lint !e835 !e845
    }

#endif

}


///
///  @brief    Output character to window. We do not output CR because ncurses
///            does the following when processing LF:
///
///            1. Clear to end of line.
///            2. Go to start of next line.
///
///            So, not only is CR not necessary, but if it preceded LF, this
///            would result in the current line getting blanked.
///
///  @returns  true if character output, false if window not active.
///
////////////////////////////////////////////////////////////////////////////////

bool putc_win(int c)
{

#if     defined(NCURSES)

    if (f.e0.winact)
    {
        if (c != CR)
        {
            (void)addch((uint)c);
            (void)refresh();
        }

        return true;
    }

#endif

    return false;
}


///
///  @brief    Output string to window.
///
///  @returns  true if character output, false if window not active.
///
////////////////////////////////////////////////////////////////////////////////

bool puts_win(const char *buf)
{

#if     defined(NCURSES)

    if (f.e0.winact)
    {
        (void)addstr(buf);
        refresh_win();

        return true;
    }

#endif

    return false;

}


///
///  @brief    Read window key.
///
///  @returns  true if window key, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool readkey_win(int c)
{
    if (c < KEY_MIN || c > KEY_MAX)
    {
        return false;
    }

    int dot = (int)t.dot;
    int Z   = (int)t.Z;

    if (c == KEY_UP)
    {
        int cur  = -getdelta_tbuf(0);   // Offset in current line
        int prev = -getdelta_tbuf(-1);  // Distance to start of previous
        int len  = prev - cur;          // Length of previous line

        dot -= prev;

        if (len < cur)
        {
            dot += len - 1;
        }
        else
        {
            dot += cur;
        }
    }
    else if (c == KEY_DOWN)
    {
        int cur  = -getdelta_tbuf(0);   // Offset in current line
        int next = getdelta_tbuf(1);    // Start of next line
        int len  = getdelta_tbuf(2) - next; // Length of next line

        dot += next;

        if (len < cur)
        {
            dot += len - 1;
        }
        else
        {
            dot += cur;
        }

        if (dot > Z)                    // Make sure we stay within buffer
        {
            dot = Z;
        }
    }
    else if (c == KEY_LEFT)
    {
        --dot;
    }
    else if (c == KEY_RIGHT)
    {
        ++dot;
    }
    else
    {
        return true;
    }

    setpos_tbuf(dot);
    refresh_win();

    return true;
}


///
///  @brief    Refresh screen.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void refresh_win(void)
{

#if     defined(NCURSES)

    if (f.e0.winact && w.nscroll != 0)
    {
        int cmdrow, cmdcol;

        getyx(stdscr, cmdrow, cmdcol);  // Save position in command window

        (void)move(text_top, 0);        // Switch to text window
        (void)attron(A_BOLD);
        (void)attrset(COLOR_PAIR(TEXT)); //lint !e835

        int nrows = w.height - w.nscroll; // No. of rows on screen

        if (!f.e1.noline)
        {
            --nrows;
        }

        assert(nrows > 0);

        int line = getlines_tbuf(-1);   // Current (relative) line number
        int row = line % nrows;         // Relative row within screen
        int pos = getdelta_tbuf(-row);  // Position of 1st chr. in 1st row
        int c;

        while ((c = getchar_tbuf(pos++)) != -1)
        {
            if (c != CR)
            {
                (void)addch((uint)c);
            }

            if (isdelim(c) && --nrows == 0)
            {
                break;
            }
        }

        // Blank out the rest of the lines if nothing to display

        while (nrows-- > 0)
        {
            (void)addch('\n');
        }

        // Highlight our current position in text window

        int dot = abs(getdelta_tbuf(0));

        (void)move(row + text_top, dot);
        c = inch() & A_CHARTEXT;        //lint !e835
        (void)delch();
        (void)insch((uint)c | A_REVERSE);

        // Restore position in command window

        (void)move(cmdrow, cmdcol);
        (void)attroff(A_BOLD);
        (void)attrset(COLOR_PAIR(CMD)); //lint !e835 !e845
    }

    (void)refresh();

#endif

}


///
///  @brief    Terminate window display.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_win(void)
{

#if     defined(NCURSES)

    if (f.e0.winact)
    {
        f.e0.winact = false;

        (void)endwin();
    }

#endif

}


///
///  @brief    Set scrolling region.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void set_scroll(int height, int nscroll)
{

#if     defined(NCURSES)

    int cmd_top, cmd_bot;

    if (f.e0.winact)
    {
        if (f.e1.cmdtop)
        {
            cmd_top  = 0;
            cmd_bot  = nscroll - 1;
            text_top = nscroll;
        }
        else
        {
            cmd_top  = height - nscroll;
            cmd_bot  = height - 1;
            text_top = 0;
        }

        (void)setscrreg(cmd_top, cmd_bot);

        if (!f.e1.noline)
        {
            int line_row;

            if (f.e1.cmdtop)
            {
                line_row = cmd_bot + 1;
                ++text_top;
            }
            else
            {
                line_row = cmd_top - 1;
            }

            // Draw line between text window and command window

            (void)move(line_row, 0);
            (void)attron(A_BOLD);
            (void)attrset(COLOR_PAIR(LINE)); //lint !e835

            for (int i = 0; i < w.width; ++i)
            {
                (void)addch(ACS_HLINE);
            }
        }

        (void)move(cmd_top, 0);

        for (int i = cmd_top; i <= cmd_bot; ++i)
        {
            (void)addch('\n');
        }

        (void)refresh();

        (void)attron(A_BOLD);
        (void)attrset(COLOR_PAIR(CMD)); //lint !e835 !e845

        (void)move(cmd_top, 0);
    }

#endif

}
