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

#define DISPLAY_INTERNAL

#include "teco.h"
#include "ascii.h"
#include "display.h"
#include "editbuf.h"
#include "exec.h"
#include "term.h"


#define MIN_ROWS            10      ///< Minimum no. of rows for edit window

bool update_window = false;         ///< true if screen redraw needed

static bool update_cursor = false;  ///< true if cursor changed


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
    .line    = NULL,
    .row     = 0,
    .col     = 0,
    .oldline = 0,
    .oldcol  = 0,
    .minrow  = 0,
    .mincol  = 0,
    .maxrow  = 0,
    .maxcol  = 0,
    .ybias   = 0,
    .xbias   = 0,
    .nrows   = 0,
};

/// @def    check_error(truth)
/// @brief  Wrapper to force Boolean value for check_error() parameter.

#if     !defined(DOXYGEN)

#define check_error(error) (check_error)((bool)(error))

#endif

// Local functions

static void (check_error)(bool error);

static int_t find_column(void);

static void init_window(WINDOW **win, int pair, int top, int bot, int col, int width);

static void init_windows(void);

static void putc_edit(int c);

static void refresh_edit(void);

static void reset_dpy(void);

static void set_cursor(void);


///
///  @brief    Issue an error if caller's function call failed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DOXYGEN)
static void check_error(bool error)
#else
static void (check_error)(bool error)
#endif
{
    if (error)
    {
        reset_dpy();
        init_term();

        throw(E_DPY);                       // Display mode error
    }
}


///
///  @brief    Clear screen and redraw display.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void clear_dpy(void)
{
    term_pos = 0;                       // Nothing output to terminal yet

    init_windows();

    update_window = true;

    refresh_dpy();
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
///  @brief    Check for ending display mode.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void end_dpy(void)
{
    if (f.e0.display)
    {
        reset_dpy();
        init_term();
    }
}


///
///  @brief    Reset display mode prior to exiting from TECO.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exit_dpy(void)
{
    reset_dpy();
}


///
///  @brief    Find the column required for the current 'dot'.
///
///  @returns  No. of bytes to adjust 'dot'.
///
////////////////////////////////////////////////////////////////////////////////

static int_t find_column(void)
{
    int_t pos = getdelta_ebuf(0);       // Get -(no. of chrs. before 'dot')
    int col = 0;                        // Current column in line
    int c;

    while ((c = getchar_ebuf(pos)) != EOF)
    {
        int width = keysize[c];

        if (width == -1)
        {
            width = TABSIZE - (col % TABSIZE);
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

    check_error(*win == NULL);

    // Set default foreground and background colors for window

    wbkgd(*win, COLOR_PAIR(pair));  //lint !e835

    keypad(*win, f.ed.escape ? (bool)TRUE : (bool)FALSE);

    if (pair == EDIT)
    {
        d.minrow = top;
        d.mincol = 0;
        d.maxrow = bot;
        d.maxcol = w.width - 1;

        prefresh(*win, 0, d.xbias = 0, d.minrow, d.mincol, d.maxrow, d.mincol);
    }
    else
    {
        wrefresh(*win);
    }
}


///
///  @brief    Set up display windows (technically, subwindows).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void init_windows(void)
{
    if (!f.e0.display)
    {
        return;
    }

    if (w.nlines == 0 || w.noscroll)    // If editing window is not set up,
    {
        d.cmd = stdscr;                 //  then use entire window for commands

        return;
    }

    d.ybias = d.xbias = 0;

    clear();
    refresh();
    init_keys();

    // Figure out what rows each window starts and ends on.

    int edit_top, edit_bot;
    int cmd_top, cmd_bot;
    int line_top = 0;
    int nrows = w.height - w.nlines;    // No. of lines for edit window

    check_error(nrows < MIN_ROWS);      // Verify that we have at least 10 rows

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

    if (f.e4.line)
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

    d.nrows = 1 + edit_bot - edit_top;

    init_window(&d.edit, EDIT, edit_top, edit_bot, 0, w.maxline);

    if (f.e4.line)
    {
        init_window(&d.line, LINE, line_top, line_top, 0, w.width);
    }

    if (f.e4.status && 1 + cmd_bot - cmd_top >= STATUS_HEIGHT)
    {
        init_window(&d.cmd, CMD, cmd_top, cmd_bot, 0, w.width - STATUS_WIDTH);
        init_window(&d.status, STATUS, cmd_top, cmd_bot, w.width - STATUS_WIDTH,
                    STATUS_WIDTH);
    }
    else
    {
        init_window(&d.cmd, CMD, cmd_top, cmd_bot, 0, w.width);
    }

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
///  @brief    Output character to edit window.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void putc_edit(int c)
{
    chtype ch = (chtype)c;

    if (isprint(c))                     // Printing chr. [32-126]
    {
        waddch(d.edit, ch);
    }
    else if (iscntrl(c))                // Control chr. [0-31, 127]
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
    else                                // 8-bit chr. [128-255]
    {
        if (f.et.eightbit)
        {
            waddstr(d.edit, unctrl(ch));
        }
        else
        {
            waddstr(d.edit, table_8bit[c & 0x7f]);
        }
    }
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

    if (t.dot < w.topdot || t.dot > w.botdot)
    {
        update_window = true;           // Force repaint if too much changed
    }

    if (update_window || update_cursor)
    {
        update_cursor = false;

        d.col = find_column();

        int nlines = getlines_ebuf(-1);
        int delta = nlines - d.oldline;

        d.oldline = nlines;

        if (!d.updown)
        {
            d.row += delta;
        }

        if (d.row < 0)
        {
            d.row = 0;

            update_window = true;
        }
        else if (d.row >= d.nrows)
        {
            d.row = d.nrows - 1;

            update_window = true;
        }

        d.ybias = d.row;

        if (update_window)
        {
            update_window = false;

            refresh_edit();
        }

        if (d.col - d.xbias > w.width)
        {
            d.xbias = d.col - (w.width / 2);
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
    int_t pos = getdelta_ebuf((int_t)-d.ybias);
    int row = -1;
    int col __attribute__((unused));
    int c;

    wclear(d.edit);

    w.topdot = t.dot + pos;             // First character output in window

    while ((c = getchar_ebuf(pos)) != EOF)
    {
        getyx(d.edit, row, col);
        putc_edit(c);

        ++pos;

        if (isdelim(c))                 // Found a delimiter (LF, VT, FF)?
        {
            if (row == d.maxrow)        // If at end of last row, then done
            {
                break;
            }

            waddch(d.edit, '\n');       // Else output newline
        }
    }

    w.botdot = t.dot + pos;             // Last character output in window

    if (row < d.maxrow)                 // Should we print EOF marker?
    {
        mvwaddch(d.edit, row + 1, 0, ACS_DIAMOND);
        wmove(d.edit, row + 1, 0);
    }
}                                       //lint !e438 !e550


///
///  @brief    Un-highlight cursor, and say that we need a cursor update.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_cursor(void)
{
    wmove(d.edit, d.row, d.col);

    // Un-highlight the character at the current position

    if (t.dot == t.Z)
    {
        mvwchgat(d.edit, d.row, d.col, 1, ACS_DIAMOND, EDIT, NULL);
    }
    else
    {
        int c = getchar_ebuf(0);
        int width = keysize[c];

        if (width == -1)
        {
            width = TABSIZE - (d.col % TABSIZE);
        }
        else if (width == 0)
        {
            ++width;
        }

        mvwchgat(d.edit, d.row, d.col, width, 0, EDIT, NULL);
    }

    d.oldline = getlines_ebuf(-1);      // Save current line number

    update_cursor = true;               // Flag need to update cursor
}


///
///  @brief    Terminate display mode.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void reset_dpy(void)
{
    if (f.e0.display)
    {
        f.e0.display = false;

        endwin();
    }
}


///
///  @brief    Rubout character on display.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void rubout_dpy(int c)
{
    char len = keysize[c];              // Get width of key

    // TODO

    while (len-- > 0)
    {
        waddch(d.cmd, '\b');
    }

    wclrtobot(d.cmd);
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
    if (d.row >= d.nrows)
    {
        d.row = d.nrows - 1;
    }
    else if (d.row < 0)
    {
        d.row = 0;
    }

    if (t.dot == t.Z)
    {
        mvwchgat(d.edit, d.row, d.col, 1, A_ALTCHARSET | A_REVERSE, EDIT, NULL);
    }
    else
    {
        int c = getchar_ebuf(0);
        int width = keysize[c];

        if (width == -1)
        {
            width = TABSIZE - (d.col % TABSIZE);
        }
        else if (width == 0)
        {
            ++width;
        }

        mvwchgat(d.edit, d.row, d.col, width, A_REVERSE, EDIT, NULL);
    }
}


///
///  @brief    Initialize for display mode.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void start_dpy(void)
{
    if (!f.e0.display)
    {
        f.e0.display = true;

        reset_term();                   // Reset if display mode support

        // Note that initscr() will print an error message and exit if it
        // fails to initialize, so there is no error return to check for.

        initscr();

        check_error( cbreak()                       == ERR   );
        check_error( noecho()                       == ERR   );
        check_error( nonl()                         == ERR   );
        check_error( notimeout(stdscr, (bool)TRUE)  == ERR   );
        check_error( idlok(stdscr,     (bool)TRUE)  == ERR   );
        check_error( has_colors()                   == FALSE );
        check_error( start_color()                  == ERR   );

        reset_colors();
        set_escdelay(0);

        getmaxyx(stdscr, w.height, w.width);

        if (w.maxline < w.width)
        {
            w.maxline = w.width;
        }

        init_windows();
        color_dpy();
        clear_dpy();
    }
}
