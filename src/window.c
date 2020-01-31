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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if     defined(NCURSES)

#include <ncurses.h>

#endif

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "textbuf.h"
#include "window.h"


#define BRIGHT_WHITE 16

#if     defined(NCURSES)

static bool winactive = false;

static int text_top;

#endif


///
///  @brief    Output character to window.
///
///  @returns  true if character output, false if window not active.
///
////////////////////////////////////////////////////////////////////////////////

bool wdisplay(int c)
{

#if     defined(NCURSES)

    if (winactive)
    {
        if (c != 13)
        {
            if (addch((uint)c) == ERR || refresh() == ERR)
            {
                end_window();

                print_err(E_WIN);
            }

            (void)refresh();
        }

        return true;
    }

#endif
    
    return false;
}


///
///  @brief    Terminate window display.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void end_window(void)
{

#if     defined(NCURSES)

    if (winactive)
    {
        winactive = false;

        (void)endwin();
    }

#endif

}


///
///  @brief    Get window size.
///
///  @returns  true if got window size, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool get_winsize(int *x, int *y)
{

#if     defined(NCURSES)

    assert(x != NULL);
    assert(y != NULL);
    
    if (winactive)
    {
        getmaxyx(stdscr, *y, *x);

        return true;
    }

#endif
    
    return false;
}


///
///  @brief    Initialize for window display.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_window(void)
{

#if     defined(NCURSES)

    if (f.e0.window && !winactive)
    {
        // Note that initscr() will print an error message and exit if it
        // fails to initialize, so there is no error return to check for.

        (void)initscr();

        winactive = true;

        if (cbreak() == ERR ||
            noecho() == ERR ||
            nonl() == ERR ||
            notimeout(stdscr, (bool)TRUE) == ERR ||
            idlok(stdscr, (bool)TRUE) == ERR ||
            scrollok(stdscr, (bool)TRUE) == ERR ||
            keypad(stdscr, (bool)TRUE) == ERR ||
            !has_colors() ||
            start_color() == ERR ||
            assume_default_colors(COLOR_BLUE, COLOR_WHITE | A_BOLD))
        {
            end_window();

            print_err(E_WIN);
        }

        short color_bg = COLOR_WHITE;

        if (can_change_color() && COLORS >= 16)
        {
            (void)init_color(BRIGHT_WHITE, 1000, 1000, 1000);

            color_bg = BRIGHT_WHITE;
        }

        (void)init_pair(1, COLOR_GREEN, color_bg);

        if (atexit(end_window) != 0)
        {
            end_window();

            print_err(E_WIN);
        }
    }

#endif

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

    if (w.nscroll != 0)
    {
        int row, col, c;

        getyx(stdscr, row, col);

        (void)move(text_top, 0);
        (void)attron(A_BOLD);
        (void)attrset(COLOR_PAIR(1));   //lint !e835

        int pos = getdelta_tbuf(0);     // Start of current line
        int dot = abs(pos);             // Position within first line
        int nrows = w.height - w.nscroll;

        if (f.ez.winline)
        {
            --nrows;
        }

        assert(nrows > 0);

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

        while (nrows-- > 0)
        {
            (void)addch('\n');
        }

        (void)move(text_top, dot);
        c = inch() & A_CHARTEXT;        //lint !e835
        (void)delch();
        (void)insch((uint)c | A_REVERSE);

        (void)move(row, col);
        (void)attroff(A_BOLD);
        (void)attrset(COLOR_PAIR(0));   //lint !e835 !e845
    }

    (void)refresh();

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

    if (winactive)
    {
        if (f.e1.bottext)
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

        if (setscrreg(cmd_top, cmd_bot) == ERR)
        {
            end_window();

            print_err(E_WIN);
        }

        if (f.ez.winline)
        {
            int line_row;

            if (f.e1.bottext)
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

        (void)move(cmd_top, 0);
    }

#endif

}
