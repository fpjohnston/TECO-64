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
#include <unistd.h>

#include <sys/ioctl.h>

#include <ncurses.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "textbuf.h"
#include "window.h"

///
///  @def    err_if_true
///
///  @brief  Issue error if function returns specified value.
///

#define err_if_true(func, cond) if (func == cond) error_win()

///
///  @def    err_if_false
///
///  @brief  Issue error if function does not return specified value.
///

#define err_if_false(func, cond) if (func != cond) error_win()


#define C_FG COLOR_BLUE                 ///< Default foreground color
#define C_BG (COLOR_WHITE | A_BOLD)     ///< Default background color

#define CMD             0               ///< Command window colors
#define TEXT            1               ///< Text window colors
#define LINE            2               ///< Line separator colors

#define BRIGHT_WHITE    16              ///< Bright white background

#if     defined(SCOPE)

///
///  @struct  region
///
///  @brief   Characteristics of a screen region
///

struct region
{
    int top;                            ///< Top of region
    int bot;                            ///< Bottom of region
    int fg;                             ///< Foreground color
    int bg;                             ///< Background color
};

///
///  @struct  display
///
///  @brief   Display format
///

struct display
{
    int row;
    int col;
    struct region text;
    struct region cmd;
    struct region line;
};

static struct display d;                ///< Display format


// Local functions

static void error_win(void);

static void repaint(int row, int col, int pos);

static void updown_win(int key);

#endif


///
///  @brief    Clear screen and redraw window.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void clear_win(void)
{

#if     defined(SCOPE)

    (void)clear();

    set_scroll(w.height, w.nscroll);

    refresh_win();

#endif

}


///
///  @brief    Reset window and issue error. When TECO starts, we either try to
///            initialize for a window display, or initialize terminal settings.
///            So if window initialization fails, we have to ensure that the
///            terminal is set up correctly.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(SCOPE)

static void error_win(void)
{
    reset_win();
    init_term();

    print_err(E_WIN);                   // Window initialization
}

#endif


///
///  @brief    Get next character.
///
///  @returns  Character read, or -1 if no character available.
///
////////////////////////////////////////////////////////////////////////////////

int getchar_win(bool wait)
{
    int c = 0;                          // Ensure that high bits are clear

#if     defined(SCOPE)

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

#if     defined(SCOPE)

    if (f.e0.winact)
    {
        getmaxyx(stdscr, w.height, w.width);

        return;
    }

#endif

    struct winsize ts;

    if (ioctl(fileno(stdin), (ulong)TIOCGWINSZ, &ts) == -1)
    {
        print_err(E_SYS);
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

#if     defined(SCOPE)

    if (!f.e0.winact)
    {
        f.e0.winact = true;

        static bool end_set = false;

        if (!end_set)
        {
            end_set = true;

            err_if_false(atexit(reset_win), 0);
        }

        // Note that initscr() will print an error message and exit if it
        // fails to initialize, so there is no error return to check for.

        (void)initscr();

        err_if_true(cbreak(),                          ERR);
        err_if_true(noecho(),                          ERR);
        err_if_true(nonl(),                            ERR);
        err_if_true(notimeout(stdscr, (bool)TRUE),     ERR);
        err_if_true(idlok(stdscr,     (bool)TRUE),     ERR);
        err_if_true(scrollok(stdscr,  (bool)TRUE),     ERR);
        err_if_true(keypad(stdscr,    (bool)TRUE),     ERR);
        err_if_true(has_colors(),                      FALSE);
        err_if_true(start_color(),                     ERR);
        err_if_true(assume_default_colors(C_FG, C_BG), ERR);

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

#else

    print_err(E_NOW);                   // Window support not enabled

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

#if     defined(SCOPE)

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

#if     defined(SCOPE)

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

bool readkey_win(int key)
{

#if     defined(SCOPE)

    if (key < KEY_MIN || key > KEY_MAX)
    {
        return false;
    }

    if (key == KEY_UP || key == KEY_DOWN)
    {
        updown_win(key);

        return true;
    }
    else if (key != KEY_LEFT && key != KEY_RIGHT)
    {
        return true;
    }

    int dot = (int)t.dot + (key == KEY_LEFT ? -1 : 1);

    setpos_tbuf(dot);
    refresh_win();

    return true;

#else

    return false;

#endif

}


///
///  @brief    Repaint screen.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(SCOPE)

static void repaint(int row, int col, int pos)
{
    int saved_row, saved_col;

    getyx(stdscr, saved_row, saved_col); // Save position in command window

    (void)move(d.text.top, 0);      // Switch to text window
    (void)attrset(COLOR_PAIR(TEXT)); //lint !e835

    int c;
    int nrows = w.height - (w.nscroll + (f.e1.winline ? 1 : 0));

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

    // Blank out the rest of the lines if nothing to display

    while (nrows-- > 0)
    {
        (void)addch('\n');
    }

    // Highlight our current position in text window

    (void)move(d.text.top + row, col);

    d.row = row;
    d.col = col;

    c = (int)inch();

    (void)delch();
    (void)insch((uint)c | A_REVERSE);

    // Restore position in command window

    (void)move(saved_row, saved_col);
    (void)attrset(COLOR_PAIR(CMD)); //lint !e835 !e845

    (void)refresh();
}

#endif


///
///  @brief    Refresh screen.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void refresh_win(void)
{

#if     defined(SCOPE)

    if (f.e0.winact && w.nscroll != 0)
    {
        int nrows = w.height - (w.nscroll + (f.e1.winline ? 1 : 0));

        assert(nrows > 0);

        int line = getlines_tbuf(-1);   // Line number within buffer
        int row  = line % nrows;        // Relative row within screen
        int col  = -getdelta_tbuf(0);   // Offset within row
        int pos  = getdelta_tbuf(-row); // First character to output

        repaint(row, col, pos);
    }

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

#if     defined(SCOPE)

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

#if     defined(SCOPE)

    if (f.e0.winact && w.nscroll != 0)
    {
        if (f.e1.cmdtop)
        {
            d.cmd.top  = 0;
            d.cmd.bot  = nscroll - 1;
            d.text.top = nscroll;
        }
        else
        {
            d.cmd.top  = height - nscroll;
            d.cmd.bot  = height - 1;
            d.text.top = 0;
        }

        (void)setscrreg(d.cmd.top, d.cmd.bot);

        d.line.top = d.line.bot = -1;

        if (f.e1.winline)
        {
            if (f.e1.cmdtop)
            {
                d.line.top = d.line.bot = d.cmd.bot + 1;
                ++d.text.top;
            }
            else
            {
                d.line.top = d.line.bot = d.cmd.top - 1;
            }

            // Draw line between text window and command window

            (void)move(d.line.top, 0);
            (void)attrset(COLOR_PAIR(LINE)); //lint !e835

            for (int i = 0; i < w.width; ++i)
            {
                (void)addch(ACS_HLINE);
            }
        }

        (void)move(d.cmd.top, 0);

        for (int i = d.cmd.top; i <= d.cmd.bot; ++i)
        {
            (void)addch('\n');
        }

        (void)attrset(COLOR_PAIR(CMD)); //lint !e835 !e845

        (void)move(d.cmd.top, 0);

        (void)refresh();
    }

#endif

}

static void updown_win(int key)
{
    int line = getlines_tbuf(-1);       // Get current line number
    int row = d.row;
    int col = d.col;
    int pos;
    int len;
    int dot = t.dot;

    if (key == KEY_UP)
    {
        if (line == 0)                  // On first line?
        {
            return;
        }

        if (row > 0)
        {
            --row;
        }

        pos = 0;

        int prev = -getdelta_tbuf(-1);  // Distance to start of previous

        len = prev - col;               // Length of previous line

        dot -= prev;
    }
    else
    {
        if (line == getlines_tbuf(0))   // On last line?
        {
            return;
        }

        pos = getdelta_tbuf(-row);

        if (row < 47)
        {
            ++row;
        }

        int next = getdelta_tbuf(1);    // Start of next line

        len = getdelta_tbuf(2) - next;  // Length of next line

        dot += next;
    }

    if (len < col)
    {
        dot += len - 1;
    }
    else
    {
        dot += col;
    }

    if (dot > t.Z)                      // Make sure we stay within buffer
    {
        dot = t.Z;
    }

    setpos_tbuf(dot);

    repaint(row, col, pos);
}
