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
#include <math.h>
#include <limits.h>
#include <ncurses.h>

#if     !defined(_STDIO_H)

#include <stdio.h>

#endif

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"
#include "ascii.h"
#include "errcodes.h"
#include "display.h"
#include "editbuf.h"
#include "eflags.h"
#include "estack.h"
#include "exec.h"
#include "file.h"
#include "page.h"
#include "term.h"

#include "keys.h"


#if     INT_T == 64

#define FMT     "%ld"               ///< 64-bit numeric format

#else

#define FMT     "%d"                ///< 32-bit numeric format

#endif

#define DEFAULT_TABSIZE     8       ///< Default tab size

#define MIN_ROWS            10      ///< Minimum no. of rows for edit window

#define STATUS_WIDTH        22      ///< Width of status window

#define STATUS_HEIGHT        5      ///< Min. height for status window

#define CMDBUF_SIZE         80      ///< Width of temporary buffer for command

const bool esc_seq_def = true;      ///< Escape sequences enabled by default

bool update_window = false;         ///< true if screen redraw needed

static bool update_cursor = false;  ///< true if cursor changed

static char charsize[UCHAR_MAX + 1]; ///< Width of each character on display


///
///  @var     d
///
///  @brief   Display window information
///

static struct
{
    WINDOW *edit;                   ///< Edit window
    WINDOW *status;                 ///< Status window
    WINDOW *cmd;                    ///< Command window
    WINDOW *line;                   ///< Partition line
    int row;                        ///< Current row for 'dot'
    int col;                        ///< Current column for 'dot'
    bool updown;                    ///< true if last cmd was up/down arrow
    int oldline;                    ///< Previous line no. for 'dot'
    int oldcol;                     ///< Previous column for 'dot'
    int minrow;                     ///< Min. row for edit window
    int mincol;                     ///< Min. column for edit window
    int maxrow;                     ///< Max. row for edit window
    int maxcol;                     ///< Max. column for edit window
    int ybias;                      ///< Vertical bias for edit window
    int xbias;                      ///< Horizontal bias for edit window
    int nrows;                      ///< No. of rows in edit window
} d =
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

static int_t count_chrs(int_t pos, int maxcol);

static void exec_down(int key);

static void exec_end(int key);

static void exec_home(int key);

static void exec_left(int key);

static void exec_right(int key);

static void exec_up(int key);

static int_t find_column(void);

static void init_window(WINDOW **win, int pair, int top, int bot, int col, int width);

static void init_windows(void);

static bool iseol(void);

static void mark_dot(void);

static void putc_edit(int c);

static void refresh_edit(void);

static void refresh_status(void);

static void reset_dpy(void);

static void status_line(int line, const char *header, const char *data);


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
///  @brief    Check to see if escape sequences were enabled or disabled.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void check_escape(bool escape)
{
    keypad(d.cmd, escape ? (bool)TRUE : (bool)FALSE);
}


///
///  @brief    Check for input characters that require special processing for
///            ncurses. This codes is placed here so that the caller doesn't
///            need to know about the KEY_xxx macros.
///
///  @returns  Next input character to process, or EOF if caller needs to check
///            for more input.
///
////////////////////////////////////////////////////////////////////////////////

int check_key(int c)
{
    if (c == KEY_BACKSPACE)
    {
        return DEL;
    }
    else if (c == KEY_RESIZE)
    {
        if (f.e0.display)
        {
            getmaxyx(stdscr, w.height, w.width);

            clear_dpy();

            print_prompt();
        }

        return EOF;
    }
    else
    {
        return c;
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
///  @brief    Count the number of characters required to get to saved column.
///
///  @returns  No. of bytes to adjust 'dot'.
///
////////////////////////////////////////////////////////////////////////////////

static int_t count_chrs(int_t pos, int maxcol)
{
    int col = 0;                        // Current column in line
    int c;

    while ((c = getchar_ebuf(pos)) != EOF)
    {
        int width = charsize[c];

        if (width == -1)
        {
            width = TABSIZE - (col % TABSIZE);
        }

        if (isdelim(c) || c == CR || col + width > maxcol)
        {
            break;
        }

        col += width;
        ++pos;
    }

    return pos;
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
///  @brief    Move cursor down.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_down(int key)
{
    int pos = getdelta_ebuf(1);         // Go to start of next line

    if (d.oldcol < d.col)
    {
        d.oldcol = d.col;
    }

    if (key == KEY_C_DOWN)
    {
        d.updown = true;
    }
    else if (d.ybias < d.nrows - 1)
    {
        ++d.ybias;
    }

    unmark_dot();

    t.dot += count_chrs(pos, d.oldcol);
}


///
///  @brief    Process End key, as follows:
///
///            <End>      - Moves to the last column in the window, then repeats
///                         until at end of line. Then moves to the end of the
///                         window, and finally to the end of the buffer.
///
///            <Ctrl/End> - Moves to the last column of the line, then to the
///                         end of the buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_end(int key)
{
    unmark_dot();

    // Here to process End and Ctrl/End keys

    if (iseol())
    {
        if (t.dot >= w.botdot - 1)      // Go to end of buffer
        {
            if (key == KEY_C_END && t.dot + 1 < t.Z)
            {
                t.dot += getdelta_ebuf(d.nrows + 1);

                if (t.dot < t.Z)
                {
                    --t.dot;

                    return;
                }
            }

            d.xbias = 0;
            d.ybias = 0;
            t.dot = t.Z;
        }
        else                            // Go to end of window
        {
            d.ybias = d.nrows - 1;
            t.dot = w.botdot - 1;
        }
    }
    else if (t.dot != t.Z)
    {
        int_t len = getdelta_ebuf(1) - 1;

        if (key == KEY_END)
        {
            t.dot += len;               // Go to end of line
        }
        else
        {
            if (d.col < d.maxcol)
            {
                d.col = d.maxcol;
            }
            else
            {
                d.col += d.maxcol + 1;
                d.xbias += d.maxcol + 1;
            }

            t.dot += count_chrs(getdelta_ebuf(0), d.col);
        }
    }
}


///
///  @brief    Process Home key, as follows:
///
///            <Home>      - Moves to the first column in the window, then
///                          repeats until at the start of the line. Then moves
///                          to the top of the window, and finally to the start
///                          of the buffer.
///
///            <Ctrl/Home> - Moves to the first column on the line, then to the
///                          start of the buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_home(int key)
{
    unmark_dot();

    // Here to process Home and Ctrl/Home keys

    if (d.xbias != 0 && key == KEY_C_HOME) // Shift to the left
    {
        if (d.col == d.xbias && (d.xbias -= w.width) < 0)
        {
            d.xbias = 0;
        }

        d.col = d.xbias;
        t.dot += count_chrs(getdelta_ebuf(0), d.col);
    }
    else if (d.col != 0)                // Go to start of line
    {
        d.col = 0;
        d.xbias = 0;
        t.dot += getdelta_ebuf(0);
    }
    else if (t.dot != w.topdot)
    {                                   // Go to top of window
        d.row = 0;
        d.ybias = 0;
        t.dot = w.topdot;
    }
    else                                // Go to top of buffer
    {
        d.row = 0;
        d.xbias = 0;
        d.ybias = 0;
        t.dot = t.B;
    }
}


///
///  @brief    Execute ncurses keys such as F1, Home, or PgDn. This includes
///            user-defined keys which depend on ncurses.
///
///  @returns  EOF if ncurses key, else same key that was passed in so that the
///            caller can process it.
///
////////////////////////////////////////////////////////////////////////////////

int exec_key(int key)
{
    if (!f.e0.display)                  // If display is off,
    {
        return key;                     //  just return whatever we got
    }

    d.updown = false;

    if (exec_soft(key))                 // User-defined (i.e., soft) key?
    {
        d.ybias = d.nrows - 1;
    }
    else if (key == KEY_HOME || key == KEY_C_HOME)
    {
        exec_home(key);
    }
    else if (key == KEY_END || key == KEY_C_END)
    {
        exec_end(key);
    }
    else                                // Neither Home nor End
    {
        if (key == KEY_UP || key == KEY_C_UP)
        {
            exec_up(key);
            refresh_dpy();

            return EOF;
        }
        else if (key == KEY_DOWN || key == KEY_C_DOWN)
        {
            exec_down(key); 
            refresh_dpy();

            return EOF;
        }
        else if (key == KEY_LEFT || key == KEY_C_LEFT)
        {
            exec_left(key);
        }
        else if (key == KEY_RIGHT || key == KEY_C_RIGHT)
        {
            exec_right(key);
        }
        else                            // Not an arrow key
        {
            char cmd[CMDBUF_SIZE];

            if (key == KEY_PPAGE)
            {
                d.ybias = 0;

                snprintf(cmd, sizeof(cmd), "%dL", -d.nrows);
                exec_str(cmd);
            }
            else if (key == KEY_NPAGE)
            {
                d.ybias = 0;

                snprintf(cmd, sizeof(cmd), "%dL", d.nrows);
                exec_str(cmd);
            }
            else if (key == KEY_C_PGUP)
            {
                int n =  d.nrows / 2;

                d.ybias = (d.nrows - (d.row + n)) % d.nrows;

                snprintf(cmd, sizeof(cmd), "-%uL", n);
                exec_str(cmd);
            }
            else if (key == KEY_C_PGDN)
            {
                int n = d.nrows / 2;

                d.ybias = (n + d.row) % d.nrows;

                snprintf(cmd, sizeof(cmd), "%uL", n);
                exec_str(cmd);
            }
            else if (key == LF || key == ESC ||
                     (key == ACCENT && f.et.accent) || key == f.ee)
            {
                if (w.nlines == 0 || w.noscroll)
                {
                    exec_str(".-Z \"N L T '");
                }
                else
                {
                    exec_str("L");
                }
            }
            else if (key == BS)
            {
                if (w.nlines == 0 || w.noscroll)
                {
                    exec_str(".-B \"N -L T '");
                }
                else
                {
                    exec_str("-L");
                }
            }
            else                        // Not a special key
            {
                return key;
            }
        }
    }

    d.oldcol = 0;

    refresh_dpy();

    return EOF;
}


///
///  @brief    Move cursor to the left.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_left(int key)
{
    if (t.dot > t.B)
    {
        unmark_dot();

        int c = getchar_ebuf(-1);       // Get previous character

        if (isdelim(c))
        {
            if (d.ybias > 0)
            {
                --d.ybias;
            }
        }

        if (d.col == 0)
        {
            d.col = -getdelta_ebuf(0);  // Go to end of line
            d.xbias = d.col - w.width;
        }
        else if (key == KEY_C_LEFT)
        {
            --d.xbias;
        }
        else if (d.col <= d.xbias)
        {
            d.xbias -= w.width;
        }

        if (d.xbias < 0)
        {
            d.xbias = 0;
        }

        --t.dot;
    }
}


///
///  @brief    Move cursor to the right.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_right(int key)
{
    if (t.dot < t.Z)
    {
        unmark_dot();

        int c = getchar_ebuf(0);        // Get next character

        if (isdelim(c))
        {
            d.col = 0;

            if (d.ybias < d.nrows - 1)
            {
                ++d.ybias;
            }
        }

        if (d.col == 0)
        {
            d.xbias = 0;
        }
        else if (key == KEY_C_RIGHT)
        {
            ++d.xbias;
        }
        else if (d.col >= d.xbias + w.width - 1)
        {
            d.xbias += w.width;
        }

        if (d.xbias > w.width)
        {
            d.xbias = w.width;
        }

        ++t.dot;
    }
}


///
///  @brief    Move cursor up.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_up(int key)
{
    int pos = getdelta_ebuf(-1);        // Go to start of previous line

    if (d.oldcol < d.col)
    {
        d.oldcol = d.col;
    }

    if (key == KEY_C_UP)
    {
        d.updown = true;
    }
    else if (d.ybias > 0)
    {
        --d.ybias;
    }

    unmark_dot();

    t.dot += count_chrs(pos, d.oldcol);
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
        int width = charsize[c];

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
///  @brief    Get tabsize.
///
///  @returns  Tab size.
///
////////////////////////////////////////////////////////////////////////////////

int get_tab(void)
{
    return TABSIZE;
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
///  @brief    Initialize charsize[] array with the length of each character
///            on the display. The values will depend not only on the specific
///            character, but also on whether SEEALL mode is in effect, and
///            whether 8-bit characters can be displayed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_charsize(void)
{
    for (int c = 0; c <= UCHAR_MAX; ++c)
    {
        chtype ch = (chtype)c;

        if (isprint(c))                 // Printing chr. [32-126]
        {
            charsize[c] = 1;
        }
        else if (c >= BS && c <= CR)    // Special control chrs.
        {
            if (w.seeall)
            {
                charsize[c] = (char)strlen(unctrl(ch));
            }
            else if (c == HT)
            {
                charsize[c] = -1;         // Special flag for tabs
            }
            else
            {
                charsize[c] = 0;
            }
        }
        else if (iscntrl(c))            // General control chrs.
        {
            charsize[c] = (char)strlen(unctrl(ch));
        }
        else if (f.et.eightbit)         // 8-bit chrs. w/ parity bit
        {
            charsize[c] = (char)strlen(unctrl(ch));
        }
        else                            // 8-bit chrs. w/o parity bit
        {
            charsize[c] = (char)strlen(table_8bit[c & 0x7f]);
        }
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
    init_charsize();

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
///  @brief    See if we're at the end of a line.
///
///  @returns  true if at end of line, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool iseol(void)
{
    int c = getchar_ebuf(0);            // Get next character

    if (c == CR && getchar_ebuf(1) == LF)
    {
        return true;
    }
    else if (isdelim(c))
    {
        return true;
    }
    else
    {
        return false;
    }
}


///
///  @brief    Save coordinates of 'dot' and mark its position. Note that the
///            end of file marker uses the alternate character set.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void mark_dot(void)
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
        int width = charsize[c];

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

        mark_dot();                     // Mark the new cursor
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
///  @brief    Refresh status window and divider line.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void refresh_status(void)
{
    if (f.e4.status)
    {
        char buf[STATUS_WIDTH + 1];
        int nrows, line = 0;

        //lint -save -e438 -e550
        int unused __attribute__((unused));
        //lint -restore

        getmaxyx(d.status, nrows, unused);

        // Output current position and no. of characters in edit buffer

        snprintf(buf, sizeof(buf), FMT "/" FMT, t.dot, t.Z);
        status_line(line++, "./Z", buf);

        // Output row and column for display

        int row = getlines_ebuf(-1);

        if (t.dot == t.Z)
        {
            status_line(line++, "Row/Col", "EOF");
        }
        else
        {
            int n = snprintf(buf, sizeof(buf), FMT, row + 1);

            snprintf(buf + n, sizeof(buf) - (size_t)(uint)n, "/" FMT, d.col + 1);
            status_line(line++, "Row/Col", buf);
        }

        // Output no. of lines in file

        snprintf(buf, sizeof(buf), FMT, getlines_ebuf(0));
        status_line(line++, "Lines", buf);

        // Output memory size

        uint_t memsize = getsize_ebuf();

        if (memsize >= GB)
        {
            snprintf(buf, sizeof(buf), "%uG", memsize / GB);
        }
        else if (memsize >= MB)
        {
            snprintf(buf, sizeof(buf), "%uM", memsize / MB);
        }
        else if (memsize >= KB)
        {
            snprintf(buf, sizeof(buf), "%uK", memsize / KB);
        }

        status_line(line++, "Memory", buf);

        // Output page number

        snprintf(buf, sizeof(buf), "%d", page_count());
        status_line(line++, "Page", buf);

        // Output vertical line to divide command window from status window

        mvwvline(d.status, 0, 0, ACS_VLINE | COLOR_PAIR(LINE), nrows); //lint !e835

        wrefresh(d.status);
    }                                   //lint !e438 !e550

    if (f.e4.line)
    {
        whline(d.line, ACS_HLINE, w.width);

        // If we have a status window, then connect its vertical line to the
        // horizontal line we just printed, using a top tee character if the
        // the edit window is on top of the command window, or a bottom tee
        // character if the command window is on top.

        if (f.e4.status)
        {
            chtype ch = (f.e4.invert) ? ACS_BTEE : ACS_TTEE;

            mvwaddch(d.line, 0, w.width - STATUS_WIDTH, ch);
        }

        wrefresh(d.line);
    }
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
    const char *s = unctrl((chtype)c);  // Get string representation of chr.
    size_t len = strlen(s);

    while (len-- > 0)
    {
        waddch(d.cmd, '\b');
    }

    wclrtobot(d.cmd);
}


///
///  @brief    Set parity. Note that the window argument to meta() is always
///            ignored, so we just pass it as a NULL.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void set_parity(bool parity)
{
    if (f.e0.display)
    {
        meta(NULL, parity ? (bool)TRUE : (bool)FALSE);
        update_window = true;
    }
}


///
///  @brief    Set tabsize.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void set_tab(int n)
{
    if (n != TABSIZE)                   // Nothing to do if no change
    {
        set_tabsize(n == 0 ? DEFAULT_TABSIZE : n);
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


///
///  @brief    Update line in status window.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void status_line(int line, const char *header, const char *data)
{
    char buf[STATUS_WIDTH - 1];
    int nbytes = snprintf(buf, sizeof(buf), " %s:", header);
    size_t rem = sizeof(buf) - (size_t)(uint)nbytes;

    snprintf(buf + nbytes, rem, "%*s ", (int)rem - 1, data);

    mvwprintw(d.status, line, 1, buf);
}


///
///  @brief    Mark 'dot' as having changed, and prepare to move it to new
///            coordinates. Note that when we are called, 'dot' has not yet
///            been changed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void unmark_dot(void)
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
        int width = charsize[c];

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
