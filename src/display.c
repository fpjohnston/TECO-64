///
///  @file    display.c
///  @brief   Display mode functions.
///
///  @copyright 2019-2022 Franklin P. Johnston / Nowwith Treble Software
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
#include <ctype.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>

#define DISPLAY_INTERNAL            ///< Enable internal definitions

#include "teco.h"
#include "ascii.h"
#include "display.h"
#include "editbuf.h"
#include "exec.h"
#include "term.h"


#define MIN_ROWS            10      ///< Minimum no. of rows for edit window


///
///  @var     d
///
///  @brief   Display window information
///

struct display d =
{
    .edit    = NULL,
    .status  = NULL,
    .cmd     = NULL,
    .fence   = NULL,
    .row     = 0,
    .col     = 0,
    .newrow  = 0,
    .newcol  = 0,
    .line    = 0,
    .cursor  = NUL,
    .oldcol  = 0,
    .minrow  = 0,
    .mincol  = 0,
    .maxrow  = 0,
    .maxcol  = 0,
    .ybias   = 0,
    .xbias   = 0,
    .nrows   = 0,
    .ncols   = 0,
};

/// @def    check(cond)
/// @brief  Wrapper to force Boolean value for check() parameter.

#if     !defined(DOXYGEN)

#define check(cond) (check)((bool)(cond))

#endif

// Local functions


static inline void (check)(bool cond);

static void init_window(WINDOW **win, int pair, int top, int bot, int col, int width);

static void init_windows(void);

static void refresh_edit(void);

static void reset_cursor(void);

static void set_cursor(void);


///
///  @brief    Issue an error if caller's function call failed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DOXYGEN)
static inline void check(bool cond)
#else
static inline void (check)(bool cond)
#endif
{
    if (!cond)
    {
        exit_dpy();                         // Turn off any display mode

        throw(E_DPY);                       // Display mode error
    }
}


///
///  @brief    Clear to end of line.
///
///  @returns  true if success, false if we couldn't.
///
////////////////////////////////////////////////////////////////////////////////

bool clear_eol(void)
{
    if (f.et.scope && f.e0.display)
    {
        wprintw(d.cmd, "\r");
        wclrtoeol(d.cmd);
        wrefresh(d.cmd);

        return true;
    }

    return false;
}


///
///  @brief    Exit display mode.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exit_dpy(void)
{
    if (f.e0.display)
    {
        f.e0.display = false;

        endwin();
        init_term();
    }
}


///
///  @brief    Find the column required for the current 'dot'.
///
///  @returns  No. of bytes to adjust 'dot'.
///
////////////////////////////////////////////////////////////////////////////////

int_t find_column(void)
{
    int_t pos = -t->pos;                // Get no. of chrs. to start of line
    int col = 0;                        // Current column in line
    int c;

    // Read characters from the start of the current line, summing the width
    // of each one, in order to calculate what column the cursor should be in.
    // We have to do this one character at a time since the widths can vary.

    while ((c = read_edit(pos)) != EOF)
    {
        int width;

        if (c == HT && !w.seeall)
        {
            width = TABSIZE - (col % TABSIZE);
        }
        else
        {
            width = keysize[c];
        }

        if (isdelim(c) || pos == 0)
        {
            break;
        }

        col += width;

        ++pos;
    }

    return col;
}


///
///  @brief    Read next character without wait (non-blocking I/O).
///
///  @returns  Character read.
///
////////////////////////////////////////////////////////////////////////////////

int get_nowait(void)
{
    if (f.e0.display)
    {
        nodelay(d.cmd, (bool)TRUE);

        int c = wgetch(d.cmd);

        nodelay(d.cmd, (bool)FALSE);

        return c;
    }
    else
    {
        return get_wait();
    }
}


///
///  @brief    Read next character (if in display mode).
///
///  @returns  Character read, or EOF if none available.
///
////////////////////////////////////////////////////////////////////////////////

int get_wait(void)
{
    int c = wgetch(d.cmd);

    if (c != ERR)
    {
        return c;
    }

    return EOF;
}


///
///  @brief    Initialize display mode.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_dpy(void)
{
    if (!f.e0.display)
    {
        f.e0.display = true;

        reset_term();                   // Reset if display mode support

        // Note that initscr() will print an error message and exit if it
        // fails to initialize, so there is no error return to check for.

        initscr();

        check( cbreak()                       == OK   );
        check( noecho()                       == OK   );
        check( nonl()                         == OK   );
        check( notimeout(stdscr, (bool)TRUE)  == OK   );
        check( idlok(stdscr,     (bool)TRUE)  == OK   );
        check( has_colors()                   == TRUE );
        check( start_color()                  == OK   );

        set_escdelay(0);
        keypad(stdscr, f.ed.escape ? (bool)TRUE : (bool)FALSE);
        reset_dpy((bool)true);
    }
}


///
///  @brief    Set up display window (technically, subwindow).
///
///  @returns  Pointer to new subwindow.
///
////////////////////////////////////////////////////////////////////////////////

static void init_window(WINDOW **win, int pair, int top, int bot, int col, int width)
{
    assert(win != NULL);

    if (*win != NULL && *win != stdscr)
    {
        delwin(*win);

        *win = NULL;
    }

    int nlines = 1 + bot - top;         // No. of rows in window

    if (pair == EDIT)
    {
        *win = newpad(nlines, w.maxline);
    }
    else
    {
        *win = subwin(stdscr, nlines, width, top, col);
    }

    check( *win != NULL );

    // Set default foreground and background colors for window

    wbkgd(*win, COLOR_PAIR(pair));  //lint !e835

    keypad(*win, f.ed.escape ? (bool)TRUE : (bool)FALSE);

    if (pair == EDIT)
    {
        d.minrow = top;
        d.mincol = 0;
        d.maxrow = bot;
        d.maxcol = d.ncols - 1;

        prefresh(*win, 0, d.xbias = 0, d.minrow, d.mincol, d.maxrow, d.mincol);
    }
    else
    {
        wrefresh(*win);
    }
}


///
///  @brief    Re-initialize all windows.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void init_windows(void)
{
    getmaxyx(stdscr, w.height, d.ncols);

    if (w.maxline < d.ncols)
    {
        w.maxline = d.ncols;
    }

    // Figure out what rows each window starts and ends on.

    int edit_top, edit_bot;
    int cmd_top, cmd_bot;
    int line_top = 0;
    int nrows = w.height - w.nlines;    // No. of lines for edit window

    check( nrows >= MIN_ROWS );         // Check that we have at least 10 rows

    if (f.e4.invert)                    // Command window above edit window?
    {
        edit_top = w.nlines;
        cmd_top = 0;
    }
    else                                // No, edit window is on top.
    {
        edit_top = 0;
        cmd_top = nrows;
    }

    // Check for drawing line/status between command and editing window.

    if (f.e4.fence)
    {
        --nrows;                        // One fewer lines for editing window

        if (f.e4.invert)
        {
            line_top = w.nlines;        // Position line after command window
            ++edit_top;                 // Start of editing window gets bumped
        }
        else
        {
            line_top = nrows;           // Position line after editing window
        }
    }

    edit_bot = edit_top + nrows - 1;
    cmd_bot  = cmd_top + w.nlines - 1;
    d.nrows  = 1 + edit_bot - edit_top;

    init_window(&d.edit, EDIT, edit_top, edit_bot, 0, w.maxline);

    if (f.e4.fence)
    {
        init_window(&d.fence, LINE, line_top, line_top, 0, d.ncols);
    }

    if (f.e4.status && 1 + cmd_bot - cmd_top >= STATUS_HEIGHT)
    {
        w.width = d.ncols - STATUS_WIDTH;

        init_window(&d.status, STATUS, cmd_top, cmd_bot, w.width, STATUS_WIDTH);
    }
    else
    {
        w.width = d.ncols;
    }

    init_window(&d.cmd, CMD, cmd_top, cmd_bot, 0, w.width);
    scrollok(d.cmd, (bool)TRUE);
    wsetscrreg(d.cmd, cmd_top, cmd_bot);
}


///
///  @brief    Output character to command window. We do not output CR because
///            ncurses does the following when processing LF:
///
///            1. Clear to end of line.
///            2. Go to start of next line.
///
///            So, not only is CR not necessary, but if it preceded LF, this
///            would result in the current line getting blanked.
///
///  @returns  true if character output, false if display not active.
///
////////////////////////////////////////////////////////////////////////////////

bool putc_cmd(int c)
{
    if (!f.e0.display)
    {
        return false;
    }

    if (c != CR)
    {
        waddch(d.cmd, (chtype)c);
    }

    return true;
}


///
///  @brief    See if we need to move the cursor or refresh the display.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void refresh_dpy(void)
{
    if (!f.e0.display || w.nlines == 0 || w.noscroll)
    {
        return;
    }

    if (t->dot < w.topdot || t->dot > w.botdot)
    {
        f.e0.window = true;             // Force repaint if too much changed
    }

    if (f.e0.window || f.e0.cursor)
    {
        f.e0.cursor = false;

        reset_cursor();

        // We can safely change d.row and d.col after resetting cursor

        int before = before_dot();
        int total  = before + after_dot(); // Total no. of line terminators

        if (d.newrow == -1)             // New cursor coordinates valid?
        {
            int delta = before - d.line; // Get difference from last line

            d.row += delta;
            d.col = find_column();
        }
        else
        {
            d.row = d.newrow;
            d.col = d.newcol;
        }

        // The following is a (hopefully temporary) hack to handle the situation
        // where the buffer has data but no line delimiter.

        if (total == 0)
        {
            if (t->Z != 0 && t->dot == t->Z)
            {
                d.row = 1;
                d.col = 0;
            }
            else
            {
                d.row = 0;
                d.col = find_column();
            }
        }

        // If row isn't in current window, then correct value and repaint screen

        if (d.row < 0)
        {
            d.row = 0;
            f.e0.window = true;
        }
        else if (d.row >= d.nrows)
        {
            d.row = d.nrows - 1;
            f.e0.window = true;
        }

        d.ybias = d.row;

        if (f.e0.window)
        {
            f.e0.window = false;

            refresh_edit();
        }

        if (d.col - d.xbias > d.ncols)
        {
            d.xbias = d.col - (d.ncols / 2);
        }

        set_cursor();                   // Mark the new cursor

        prefresh(d.edit, 0, d.xbias, d.minrow, d.mincol, d.maxrow, d.maxcol);
    }

    refresh_status();
    wrefresh(d.cmd);                    // Switch back to command window
}


///
///  @brief    Refresh edit window.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void refresh_edit(void)
{
    int_t pos = len_edit((int_t)-d.ybias);
    int row = -1;
    int col __attribute__((unused));
    int c;

    wclear(d.edit);

    w.topdot = t->dot + pos;            // First character output in window

    while ((c = read_edit(pos)) != EOF)
    {
        ++pos;

        getyx(d.edit, row, col);

        chtype ch = (chtype)c;

        if (isprint(c))                 // Printing chr. [32-126]
        {
            waddch(d.edit, ch);
        }
        else if (iscntrl(c))            // Control chr. [0-31, 127]
        {
            switch (c)
            {
                case HT:
                    if (w.seeall)
                    {
                        waddstr(d.edit, unctrl(ch));
                    }
                    else
                    {
                        waddch(d.edit, ch);
                    }

                    break;

                case BS:
                case VT:
                case FF:
                case LF:
                case CR:
                    if (w.seeall)
                    {
                        waddstr(d.edit, unctrl(ch));
                    }

                    break;

                default:
                    waddch(d.edit, ch);

                    break;
            }
        }
        else                            // 8-bit chr. [128-255]
        {
            if (w.seeall)
            {
                waddstr(d.edit, table_8bit[c & 0x7f]);
            }
            else
            {
                waddstr(d.edit, unctrl(ch));
            }
        }

        if (isdelim(c))                 // Found a delimiter (LF, VT, FF)?
        {
            if (row == d.maxrow)        // If at end of last row, then done
            {
                break;
            }

            waddch(d.edit, '\n');       // Else output newline
        }
    }

    w.botdot = t->dot + pos;            // Last character output in window

    if (row < d.maxrow)                 // Should we print EOF marker?
    {
        mvwaddch(d.edit, row + 1, 0, ACS_DIAMOND);
        wmove(d.edit, row + 1, 0);
    }
}                                       //lint !e438 !e550


///
///  @brief    Un-highlight cursor.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void reset_cursor(void)
{
    int c = d.cursor;

    if (c == HT && !w.seeall)
    {
        int width = TABSIZE - (d.col % TABSIZE);
        chtype ch = mvwinch(d.edit, d.row, d.col) & ~A_REVERSE;

        mvwchgat(d.edit, d.row, d.col, width, ch, EDIT, NULL);
    }
    else
    {
        int width;

        if (c == EOF)
        {
            width = 1;
        }
        else
        {
            width = keysize[c] ?: 1;
        }

        for (int col = d.col; col < d.col + width; ++col)
        {
            chtype ch = mvwinch(d.edit, d.row, col) & ~A_REVERSE;

            mvwchgat(d.edit, d.row, col, 1, ch, EDIT, NULL);
        }
    }
}


///
///  @brief    Reset display.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_dpy(bool all)
{
    if (!f.e0.display)
    {
        return;
    }

    if (all)
    {
        clear();
        init_keys();

        if (w.nlines == 0 || w.noscroll)
        {
            d.cmd = stdscr;

            refresh();

            return;
        }
        else
        {
            init_windows();
        }
    }

    d.ybias = d.xbias = 0;

    f.e0.window = true;

    refresh_dpy();
}


///
///  @brief    Save coordinates of 'dot' and mark its position. Note that the
///            end of file marker uses the alternate character set.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void set_cursor(void)
{
    int c = d.cursor = read_edit(0);    // Save cursor character (for reset)

    if (c == HT && !w.seeall)
    {
        int width = TABSIZE - (d.col % TABSIZE);
        chtype ch = mvwinch(d.edit, d.row, d.col) | A_REVERSE;

        mvwchgat(d.edit, d.row, d.col, width, ch, EDIT, NULL);
    }
    else
    {
        int width;

        if (c == EOF)
        {
            width = 1;
        }
        else
        {
            width = keysize[c] ?: 1;
        }

        for (int col = d.col; col < d.col + width; ++col)
        {
            chtype ch = mvwinch(d.edit, d.row, col) | A_REVERSE;

            mvwchgat(d.edit, d.row, col, 1, ch, EDIT, NULL);
        }
    }

    d.line = before_dot();              // Save current line number
    d.newrow = d.newcol = -1;           // Reinitialize new cursor coordinates
}
