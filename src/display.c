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

#define DIAMOND  (A_ALTCHARSET | 0x60) ///< End of file marker

#define STATUS_WIDTH        22      ///< Width of status window

#define STATUS_HEIGHT        5      ///< Min. height for status window

#define EOF_ROWS             4      ///< No. of rows following EOF

#define HOLD        (bool)true      ///< Hold cursor in current row/column

#define NO_HOLD     (bool)false     ///< Don't hold cursor in current row/column

const bool esc_seq_def = true;      ///< Escape sequences enabled by default

static bool ebuf_changed = false;   ///< true if edit buffer modified

static uint n_home = 0;             ///< No. of consecutive Home keys

static uint n_end = 0;              ///< No. of consecutive End keys


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
    WINDOW *line;                   ///< Dividing line
    int row;                        ///< Current row for 'dot'
    int col;                        ///< Current column for 'dot'
    int prev;                       ///< Previous column for 'dot'
    int minrow;                     ///< Min. row for edit window
    int mincol;                     ///< Min. column for edit window
    int maxrow;                     ///< Max. row for edit window
    int maxcol;                     ///< Max. column for edit window
    int vbias;                      ///< Vertical bias
    int hbias;                      ///< Horizontal bias
    int nrows;                      ///< No. of rows in edit window
} d =
{
    .edit   = NULL,
    .status = NULL,
    .cmd    = NULL,
    .line   = NULL,
    .row    = 0,
    .col    = 0,
    .prev   = 0,
    .minrow = 0,
    .mincol = 0,
    .maxrow = 0,
    .maxcol = 0,
    .vbias  = 0,
    .hbias  = 0,
    .nrows  = 0,
};


/// @def    check_error(truth)
/// @brief  Wrapper to force Boolean value for check_error() parameter.

#if     !defined(DOXYGEN)

#define check_error(error) (check_error)((bool)(error))

#endif

// Local functions

static void (check_error)(bool error);

static int_t adjust_dot(void);

static void init_window(WINDOW **win, int pair, int top, int bot, int col, int width);

static void init_windows(void);
static void mark_dot(void);

static void move_down(bool hold);

static void move_left(bool hold);

static void move_right(bool hold);

static void move_up(bool hold);

static void putc_edit(int c);

static void refresh_edit(void);

static void refresh_status(void);

static void reset_dpy(void);

static void resize_key(void);

static void status_line(int line, const char *header, const char *data);

static void unmark_dot(void);


///
///  @brief    Get adjustment for 'dot'.
///
///  @returns  No. of bytes to adjust 'dot'.
///
////////////////////////////////////////////////////////////////////////////////

static int_t adjust_dot(void)
{
    int col = 0;                        // Current column in line
    int pos = 0;                        // Position relative to 'dot'
    int c;

    while ((c = getchar_ebuf(pos)) != EOF)
    {
        chtype ch = (chtype)c;
        int width;

        if (isprint(c))                 // Printing chr. [32-126]
        {
            width = 1;
        }
        else if (c >= BS && c <= CR)    // Special control chrs.
        {
            if (w.seeall)
            {
                width = (int)strlen(unctrl(ch));
            }
            else if (c == HT)
            {
                width = TABSIZE - (col % 8);
            }
            else
            {
                width = 0;
            }
        }
        else if (iscntrl(c))            // General control chrs.
        {
            if (w.seeall)
            {
                width = (int)strlen(unctrl(ch));
            }
            else
            {
                width = 1;
            }
        }
        else if (f.et.eightbit)         // 8-bit chrs. w/ parity bit
        {
            width = (int)strlen(unctrl(ch));
        }
        else                            // 8-bit chrs. w/o parity bit
        {
            width = (int)strlen(table_8bit[c & 0x7f]);
        }

        if (isdelim(c) || c == CR || col + width > d.prev)
        {
            break;
        }

        col += width;
        ++pos;
    }

    return pos;
}


///
///  @brief    Check for special display input characters.
///
///  @returns  Next input character to process.
///
////////////////////////////////////////////////////////////////////////////////

int check_dpy_chr(int c, bool wait)
{
    if (c == KEY_BACKSPACE)
    {
        return DEL;
    }
    else if (c == KEY_RESIZE)
    {
        resize_key();

        return getc_term(wait);         // Recurse to get next character
    }
    else
    {
        return c;
    }
}


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
///  @brief    Clear screen and redraw display.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void clear_dpy(void)
{
    term_pos = 0;                       // Nothing output to terminal yet

    init_windows();
    mark_ebuf();
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

        prefresh(*win, 0, d.hbias = 0, d.minrow, d.mincol, d.maxrow, d.mincol);
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

    d.vbias = d.hbias = 0;

    clear();
    refresh();

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
        int n = 1;                      // Most chrs. are 1 byte wide
        int c = getchar_ebuf(0);

        if (iscntrl(c))
        {
            if (w.seeall || (c < BS || c > CR))
            {
                n = (int)strlen(unctrl((chtype)c));
            }
        }
        else if (c > DEL)
        {
            if (f.et.eightbit)
            {
                n = (int)strlen(unctrl((chtype)c));
            }
            else
            {
                n = (int)strlen(table_8bit[c & 0x7f]);
            }
        }

        mvwchgat(d.edit, d.row, d.col, n, A_REVERSE, EDIT, NULL);
    }
}


///
///  @brief    Mark edit buffer as having changed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void mark_ebuf(void)
{
    ebuf_changed = true;
}


///
///  @brief    Move cursor down.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void move_down(bool hold)
{
    t.dot += getdelta_ebuf(1);      // Go to start of next line

    if (d.prev < d.col)
    {
        d.prev = d.col;
    }

    if (hold)
    {
        ;
    }
    else
    {
        if (d.vbias < d.nrows - 1)
        {
            ++d.vbias;
        }
    }

    t.dot += adjust_dot();
}


///
///  @brief    Move cursor to the left.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void move_left(bool hold)
{
    if (t.dot > t.B)
    {
        --t.dot;

        int c = getchar_ebuf(0);        // Get previous character

        if (isdelim(c))
        {
            if (d.vbias > 0)
            {
                --d.vbias;
            }
        }

        if (d.col == 0)
        {
            d.col = -getdelta_ebuf(0);  // Go to end of line
            d.hbias = d.col - w.width;
        }
        else if (hold)
        {
            --d.hbias;
        }
        else if (d.col <= d.hbias)
        {
            d.hbias -= w.width;
        }

        if (d.hbias < 0)
        {
            d.hbias = 0;
        }
    }
}


///
///  @brief    Move cursor to the right.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void move_right(bool hold)
{
    if (t.dot < t.Z)
    {
        int c = getchar_ebuf(0);        // Get next character

        ++t.dot;

        if (isdelim(c))
        {
            d.col = 0;

            if (d.vbias < d.nrows - 1)
            {
                ++d.vbias;
            }
        }

        if (d.col == 0)
        {
            d.hbias = 0;
        }
        else if (hold)
        {
            ++d.hbias;
        }
        else if (d.col >= d.hbias + w.width - 1)
        {
            d.hbias += w.width;
        }

        if (d.hbias > w.width)
        {
            d.hbias = w.width;
        }
    }
}


///
///  @brief    Move cursor up.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void move_up(bool hold)
{
    t.dot += getdelta_ebuf(-1);         // Go to start of previous line

    if (d.prev < d.col)
    {
        d.prev = d.col;
    }

    if (!hold)
    {
        if (d.vbias > 0)
        {
            --d.vbias;
        }
    }

    t.dot += adjust_dot();
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
///  @brief    Read display key.
///
///  @returns  Character to process, or EOF if already processed.
///
////////////////////////////////////////////////////////////////////////////////

int readkey_dpy(int key)
{
    if (!f.e0.display)                  // If display is off,
    {
        return key;                     //  just return whatever we got
    }
    else if (exec_key(key))             // User-defined key?
    {
        n_home = n_end = 0;             // Yes. Reset Home and End counters.
    }
    else if (key == KEY_HOME)
    {
        n_end = 0;

        if (++n_home == 1)
        {
            t.dot += getdelta_ebuf(0);
        }
        else if (n_home == 2)
        {
            t.dot = w.topdot;
            d.vbias = 0;
        }
        else
        {
            n_home = 0;
            t.dot = t.B;
        }

        setdot_dpy();
    }
    else if (key == KEY_END)
    {
        n_home = 0;

        if (++n_end == 1)
        {
            t.dot += getdelta_ebuf(1);

            if (t.dot > t.B)
            {
                --t.dot;
            }
        }
        else if (n_end == 2)
        {
            t.dot = w.botdot;
            d.vbias = d.nrows - 1;
        }
        else
        {
            n_end = 0;
            t.dot = t.Z;
        }

        setdot_dpy();
    }
    else                                // Neither Home nor End
    {
        n_home = n_end = 0;             // Make sure we restart the counts

        if (key == KEY_UP)
        {
            move_up(NO_HOLD);
            mark_ebuf();
        }
        else if (key == KEY_DOWN)
        {
            move_down(NO_HOLD);
            mark_ebuf();
        }
        else if (key == KEY_LEFT)
        {
            move_left(NO_HOLD);
            setdot_dpy();
        }
        else if (key == KEY_RIGHT)
        {
            move_right(NO_HOLD);
            setdot_dpy();
        }
        else if (key == KEY_C_UP)
        {
            move_up(HOLD);
            mark_ebuf();
        }
        else if (key == KEY_C_DOWN)
        {
            move_down(HOLD);
            mark_ebuf();
        }
        else if (key == KEY_C_LEFT)
        {
            move_left(HOLD);
            setdot_dpy();
        }
        else if (key == KEY_C_RIGHT)
        {
            move_right(HOLD);
            setdot_dpy();
        }
        else                            // Not an arrow key
        {
            if (key == KEY_PPAGE)
            {
                exec_str("-(2:W) L");
            }
            else if (key == KEY_NPAGE)
            {
                exec_str("(2:W) L");
            }
            else if (key == CR || key == LF || key == ESC ||
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

            setdot_dpy();
        }
    }

    refresh_dpy();

    return EOF;
}


///
///  @brief    Refresh screen.
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

    if (ebuf_changed)
    {
        ebuf_changed = false;

        refresh_edit();

        if (n_home != 0 || n_end != 0)
        {
            d.hbias = (d.col / w.width) * w.width;
        }

        prefresh(d.edit, 0, d.hbias, d.minrow, d.mincol, d.maxrow, d.maxcol);
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
    unmark_dot();

    int next = getlines_ebuf(1);        // No. of lines remaining

    if (d.vbias + next < d.nrows - EOF_ROWS)
    {
        d.vbias = d.nrows - EOF_ROWS;
    }

    int_t pos = getdelta_ebuf((int_t)-d.vbias);
    int c;
    int row = 0;

    wclear(d.edit);

    w.topdot = t.dot + pos;

    while (pos < t.Z && (c = getchar_ebuf(pos)) != EOF)
    {
        if (pos == 0)
        {
            getyx(d.edit, d.row, d.col);
        }

        putc_edit(c);

        // Always output an LF after a line delimiter

        if (isdelim(c))                 // Found a delimiter (LF, VT, FF)?
        {
            waddch(d.edit, '\n');

            if (++row == d.nrows)
            {
                break;
            }
        }

        ++pos;
    }

    if (t.dot + pos >= t.Z)             // Should we print EOF marker?
    {
        waddch(d.edit, DIAMOND);
        waddch(d.edit, BS);

        if (t.dot >= t.Z)               // Are we actually at EOF marker?
        {
            getyx(d.edit, d.row, d.col);
        }
    }

    w.botdot = t.dot + pos;

    mark_dot();                         // Mark the new cursor
}


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

        if (t.dot >= t.Z)
        {
            status_line(line++, "Row/Col", "EOF");
        }
        else
        {
            int n = snprintf(buf, sizeof(buf), FMT, d.row + 1);

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
///  @brief    Finish window resize when KEY_RESIZE key read.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void resize_key(void)
{
    if (f.e0.display)
    {
        getmaxyx(stdscr, w.height, w.width);

        clear_dpy();

        print_prompt();
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
///  @brief    Mark 'dot' as having changed, and force a screen redraw.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void setdot_dpy(void)
{
    ebuf_changed = true;

    d.prev = 0;
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
        ebuf_changed = true;
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

        prefresh(d.edit, 0, d.hbias, d.minrow, d.mincol, d.maxrow, d.maxcol);
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
///  @brief    Get coordinates of 'dot' and un-mark its position.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void unmark_dot(void)
{
    wmove(d.edit, d.row, d.col);

    // Un-highlight the character at the current position

    if (t.dot == t.Z)
    {
        mvwchgat(d.edit, d.row, d.col, 1, A_ALTCHARSET, EDIT, NULL);
    }
    else
    {
        int n = 1;
        int c = getchar_ebuf(0);

        if (iscntrl(c) && (c < BS || c > CR))
        {
            ++n;
        }
        else if (c > DEL)
        {
            if (f.et.eightbit)
            {
                n = (int)strlen(unctrl((chtype)c));
            }
            else
            {
                n = (int)strlen(table_8bit[c & 0x7f]);
            }
        }

        mvwchgat(d.edit, d.row, d.col, n, 0, EDIT, NULL);
    }
}
