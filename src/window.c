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

#if     defined(NCURSES)

#include <ncurses.h>

#endif

#include "teco.h"
//#include "eflags.h"
#include "errors.h"
#include "exec.h"

#if     defined(NCURSES)

bool winactive = false;

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
            if (addch(c) == ERR || refresh() == ERR)
            {
                end_window();

                print_err(E_WIN);
            }
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

    if (!winactive)
    {
        // Initialize screen. Note that initscr() will print an error message
        // and exit if it fails, so there is no error return to check for.

        initscr();

        winactive = true;

        if (cbreak() == ERR ||
            noecho() == ERR ||
            nonl() == ERR ||
            notimeout(stdscr, (bool)TRUE) == ERR ||
            idlok(stdscr, (bool)TRUE) == ERR ||
            scrollok(stdscr, (bool)TRUE) == ERR ||
            keypad(stdscr, (bool)TRUE) == ERR)
        {
            end_window();

            print_err(E_WIN);
        }

        // TODO: is this needed?

        (void)wrefresh(stdscr);

        if (atexit(end_window) != 0)
        {
            end_window();

            print_err(E_WIN);
        }
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

    if (winactive)
    {
        int top = height - nscroll;
        int bot = height - 1;

        if (setscrreg(top, bot) == ERR)
        {
            end_window();

            print_err(E_WIN);
        }

        move(top - 1, 0);

        for (int i = 0; i < w.width; ++i)
        {
            addch(ACS_HLINE);
        }

        move(top, 0);

        refresh();
    }

#endif

}
