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


#if     INT_T == 64

#define FMT     "%ld"

#else

#define FMT     "%d"

#endif

#define DEFAULT_TABSIZE     8       ///< Default tab size

#define MIN_ROWS            10      ///< Minimum no. of rows for edit window

#define DIAMOND  (A_ALTCHARSET | 0x60) ///< End of file marker

#define STATUS_WIDTH        22      ///< Width of status window

#define STATUS_HEIGHT        5      ///< Min. height for status window

const bool esc_seq_def = true;      ///< Escape sequences enabled by default

static bool dot_changed = false;    ///< true if 'dot' modified

static bool ebuf_changed = false;   ///< true if edit buffer modified

static uint n_home = 0;             ///< No. of consecutive Home keys

static uint n_end = 0;              ///< No. of consecutive End keys


///
///  @struct  display
///
///  @brief   Display window information
///

static struct display
{
    WINDOW *edit;                   ///< Edit window
    WINDOW *status;                 ///< Status window
    WINDOW *cmd;                    ///< Command window
    WINDOW *line;                   ///< Dividing line
    int row;                        ///< y coordinate for 'dot'
    int col;                        ///< x coordinate for 'dot'
    int bias;                       ///< Row bias within edit window
    int nrows;                      ///< No. of rows in edit window
    int vcol;                       ///< Virtual column in edit window
} d =
{
    .edit   = NULL,
    .status = NULL,
    .cmd    = NULL,
    .line   = NULL,
    .row    = 0,
    .col    = 0,
    .bias   = 0,
    .nrows  = 0,
    .vcol   = 0,
};


/// @def    check_error(truth)
/// @brief  Wrapper to force Boolean value for check_error() parameter.

#if     !defined(DOXYGEN)

#define check_error(error) (check_error)((bool)(error))

#endif

// Local functions

static void (check_error)(bool error);

static int addlines(int pos, int nrows);

static void get_dot(int *row, int *col);

static void init_window(WINDOW **win, int pair, int top, int bot, int col, int width);

static void move_down(void);

static void move_left(void);

static void move_right(void);

static void move_up(void);

static void putc_edit(int c);

static void reset_dpy(void);

static void resize_key(void);

static void set_dot(int row, int col);

static void status_line(int line, const char *header, const char *data);

static void update_status(void);


#define DEBUG_DPY       1               // TODO: remove for production build

#if     defined(DEBUG_DPY)

static FILE *dpy_fp = NULL;

static void (bugprint)(const char *func, ...);

//lint -save -e652

#define bugprint(...) (bugprint)(__func__, __VA_ARGS__)

//lint -restore

#else

#define bugprint(func, ...)

#endif


///
///  @brief    Print debug message.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DEBUG_DPY)

static void (bugprint)(
    const char *func,                   ///< Function name
    ...)                                ///< Remaining arguments for printf()
{
    assert(func != NULL);               // Make sure we have a function name
    assert(dpy_fp != NULL);             // Debug file should be open

    fprintf(dpy_fp, "%s: ", func);      // Print error preamble

    va_list argptr;

    //lint -esym(530,argptr)

    va_start(argptr, func);

    //lint -esym(534,vfprintf)

    const char *format = va_arg(argptr, const char *);

    vfprintf(dpy_fp, format, argptr);   // Now add the message text

    va_end(argptr);
}

#endif


///
///  @brief    Output one or more lines to the edit window.
///
///  @returns  New position.
///
////////////////////////////////////////////////////////////////////////////////

static int addlines(int pos, int nrows)
{
    int row = 0;
    int c;

    while ((c = getchar_ebuf(pos)) != EOF)
    {
        // Save window coordinates of current position in edit buffer

        if (pos++ == 0)
        {
            getyx(d.edit, d.row, d.col);
        }

        putc_edit(c);

        if (c == LF)
        {
            if (++row == nrows)
            {
                break;
            }

            waddch(d.edit, (chtype)c);
        }
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
    term_pos = 0;

    init_windows();

    ebuf_changed = true;

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

#if     defined(DEBUG_DPY)

    if (dpy_fp != NULL)
    {
        bugprint("TECO display log closed\n");

        fclose(dpy_fp);

        dpy_fp = NULL;
    }

#endif

}


///
///  @brief    Get coordinates of 'dot' and un-mark its position.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void get_dot(int *row, int *col)
{
    int y_pos, x_pos;

    getyx(d.edit, y_pos, x_pos);

    // Un-highlight the character at the current position

    if (t.dot == t.Z)
    {
        mvwchgat(d.edit, y_pos, x_pos, 1, A_ALTCHARSET, EDIT, NULL);
    }
    else
    {
        int n = 1;
        int c = getchar_ebuf(0);

        if (iscntrl(c) && (c < BS || c > CR))
        {
            ++n;
        }

        mvwchgat(d.edit, y_pos, x_pos, n, 0, EDIT, NULL);
    }

    if (row != NULL)
    {
        *row = y_pos;
    }

    if (col != NULL)
    {
        *col = x_pos;
    }
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

    if (*win != NULL)
    {
        delwin(*win);
    }

    int nlines = 1 + bot - top;         // No. of rows in region

    *win = subwin(stdscr, nlines, width, top, col);

    check_error(*win == NULL);

    // Set default foreground and background colors for region

    wbkgd(*win, COLOR_PAIR(pair));  //lint !e835

    scrollok(*win, (bool)TRUE);

    wsetscrreg(*win, top, bot);

    keypad(*win, f.ed.escape ? (bool)TRUE : (bool)FALSE);

    wclear(*win);
    wrefresh(*win);
}


///
///  @brief    Set up display windows (technically, subwindows).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_windows(void)
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

    init_window(&d.edit, EDIT, edit_top, edit_bot, 0, w.width);

    if (f.e4.line)
    {
        init_window(&d.line, LINE, line_top, line_top, 0, w.width);
    }

    if (f.e4.status && 1 + cmd_bot - cmd_top >= STATUS_HEIGHT)
    {
        init_window(&d.status, STATUS, cmd_top, cmd_bot, w.width - STATUS_WIDTH,
                    STATUS_WIDTH);

        init_window(&d.cmd, CMD, cmd_top, cmd_bot, 0, w.width - STATUS_WIDTH);
    }
    else
    {
        init_window(&d.cmd, CMD, cmd_top, cmd_bot, 0, w.width);
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

static void move_down(void)
{
    // TODO: handle virtual columns.
    // TODO: handle hardware tabs in lines.

    int row, col;

    get_dot(&row, &col);

    if (t.dot == t.Z)
    {
        int attr = A_ALTCHARSET | A_REVERSE | A_BLINK;
        int len = -getdelta_ebuf(0);    // Get length of current line

        mvwchgat(d.edit, row, col + len, 1, attr, EDIT, NULL); //lint !e732
    }
    else
    {
        t.dot += getdelta_ebuf(1);      // Go to start of next line

        if (d.bias < d.nrows - 1)
        {
            ++d.bias;
        }
        else
        {
            wscrl(d.edit, 1);
            wmove(d.edit, d.nrows - 1, 0);

            (void)addlines(0, 1);
        }

        set_dot(row + 1, 0);
    }

    wrefresh(d.edit);
    wrefresh(d.cmd);
}


///
///  @brief    Move cursor to the left.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void move_left(void)
{
    if (t.dot == t.B)
    {
        int attr = A_REVERSE | A_BLINK;

        mvwchgat(d.edit, 0, 0, 1, attr, EDIT, NULL); //lint !e732
    }
    else
    {
        int row, col;

        get_dot(&row, &col);

        --t.dot;

        if (getchar_ebuf(0) == LF)
        {
            col = -getdelta_ebuf(0);

            if (d.bias > 0)
            {
                --d.bias;
                --row;
            }
            else
            {
                wscrl(d.edit, -1);
                wmove(d.edit, 0, 0);

                (void)addlines(-col, 1);

                ++col;
            }
        }

        set_dot(row, col - 1);
    }

    wrefresh(d.edit);
    wrefresh(d.cmd);
}


///
///  @brief    Move cursor to the right.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void move_right(void)
{
    if (t.dot == t.Z || getchar_ebuf(0) == LF)
    {
        move_down();

        return;
    }

    if (t.dot == t.Z)                   // Already at EOF?
    {
        mvwchgat(d.edit, d.row, d.col, 1, A_ALTCHARSET, EDIT, NULL);
    }
    else
    {
        ++t.dot;

        mvwchgat(d.edit, d.row, d.col, 1, 0, EDIT, NULL);
    }

    set_dot(d.row, d.col + 1);

    wrefresh(d.edit);
    wrefresh(d.cmd);
}


///
///  @brief    Move cursor up.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void move_up(void)
{
    // TODO: handle virtual columns.
    // TODO: handle hardware tabs in lines.

    if (t.dot == t.B)
    {
        int attr = A_REVERSE | A_BLINK;

        mvwchgat(d.edit, 0, 0, 1, attr, EDIT, NULL); //lint !e732
    }
    else
    {
        int row, col;

        get_dot(&row, &col);

        t.dot += getdelta_ebuf(-1);

        if (d.bias > 0)
        {
            --d.bias;
        }
        else
        {
            wscrl(d.edit, -1);
            wmove(d.edit, 0, 0);

            (void)addlines(0, 1);
        }

        set_dot(row - 1, 0);
    }

    wrefresh(d.edit);
    wrefresh(d.cmd);
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
        (void)waddch(d.cmd, (chtype)c);
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
                    wprintw(d.edit, "%s", unctrl(ch));
                }

                waddch(d.edit, ch);

                break;

            case BS:
            case VT:
            case FF:
            case LF:
            case CR:
                if (w.seeall)
                {
                    wprintw(d.edit, "%s", unctrl(ch));
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
            wprintw(d.edit, "%s", unctrl(ch));
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

        if (++n_home == 1)              // Beginning of line
        {
            exec_str("0 L");
        }
        else if (n_home == 2)           // Beginning of window
        {
            exec_str("F0 J");
        }
        else                            // Beginning of file
        {
            exec_str("0 J");
        }

        ebuf_changed = true;
    }
    else if (key == KEY_END)
    {
        n_home = 0;

        if (++n_end == 1)               // End of line
        {
            // We effectively execute "LR" to get to the end of a line that
            // ends with LF, and execute "L2R" for a line that ends with CR/LF.
            // The commands below, which include a test to see if the character
            // before the LF is a CR, take care of this regardless of the file
            // format.

            exec_str("L (-2 A - 13)\"E 2 R | R '");
        }
        else if (n_end == 2)            // End of window
        {
            exec_str("(FZ - 1) J");
        }
        else                            // End of file
        {
            exec_str("Z J");
        }

        ebuf_changed = true;
    }
    else                                // Neither Home nor End
    {
        n_home = n_end = 0;             // Make sure we restart the counts

        if (key == KEY_UP)
        {
            move_up();
        }
        else if (key == KEY_DOWN)
        {
            move_down();
        }
        else if (key == KEY_LEFT)
        {
            move_left();

            dot_changed = true;
        }
        else if (key == KEY_RIGHT)
        {
            move_right();

            dot_changed = true;
        }
        else                            // Not an arrow key
        {
            ebuf_changed = true;        // Repaint the edit window

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
    if (!f.e0.display)
    {
        return;
    }

    if (f.e4.line || f.e4.status)
    {
        update_status();
    }

    if (w.nlines != 0 && !w.noscroll && ebuf_changed)
    {
        ebuf_changed = false;

        int line = (int)getlines_ebuf(-1);  // Line number within buffer

        if (line == 0)
        {
            d.bias = 0;
        }

        int row  = (line - d.bias) % d.nrows; // Relative row within screen
        int_t pos  = getdelta_ebuf((int_t)-row); // First character to output

        w.topdot = t.dot + pos;

        wmove(d.edit, 0, 0);
        wclear(d.edit);

        pos = addlines(pos, d.nrows);

        w.botdot = t.dot + pos;

        if (getchar_ebuf(pos) == EOF)
        {
            waddch(d.edit, DIAMOND);
            waddch(d.edit, BS);
        }

        if (d.row == 0 && d.col == 0)
        {
            set_dot(0, 0);
        }
        else
        {
            int row, col;

            getyx(d.edit, row, col);

            set_dot(row, col);
        }

        wrefresh(d.edit);
    }

    wrefresh(d.cmd);                // Switch back to command window
}


///
///  @brief    Reset region colors to defaults.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_colors(void)
{
    if (can_change_color())             // Make colors as bright as possible
    {
        init_color(COLOR_BLACK,        0,      0,      0);
        init_color(COLOR_RED,     SATMAX,      0,      0);
        init_color(COLOR_GREEN,        0, SATMAX,      0);
        init_color(COLOR_YELLOW,  SATMAX, SATMAX,      0);
        init_color(COLOR_BLUE,         0,      0, SATMAX);
        init_color(COLOR_MAGENTA, SATMAX,      0, SATMAX);
        init_color(COLOR_CYAN,         0, SATMAX, SATMAX);
        init_color(COLOR_WHITE,   SATMAX, SATMAX, SATMAX);
    }

    assume_default_colors(COLOR_BLACK, COLOR_WHITE);

    init_pair(CMD,    COLOR_BLACK, COLOR_WHITE);
    init_pair(EDIT,   COLOR_BLACK, COLOR_WHITE);
    init_pair(STATUS, COLOR_BLACK, COLOR_WHITE);
    init_pair(LINE,   COLOR_BLACK, COLOR_WHITE);
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
        init_windows();
        clear_dpy();
        print_prompt();
        echo_tbuf((uint_t)0);
    }
}


///
///  @brief    Resize window when user changes its size.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void resize_signal(void)
{
    if (f.e0.display)
    {
        resizeterm(w.height, w.width);
        getmaxyx(stdscr, w.height, w.width);
        init_windows();
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
///  @brief    Save coordinates of 'dot' and mark its position. Note that the
///            end of file marker uses the alternate character set.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void set_dot(int row, int col)
{
    if (row >= d.nrows)
    {
        row = d.nrows - 1;
    }
    else if (row < 0)
    {
        row = 0;
    }

    d.row = row;
    d.col = col;

    if (t.dot == t.Z)
    {
        mvwchgat(d.edit, d.row, d.col, 1, A_ALTCHARSET | A_REVERSE, EDIT, NULL);
    }
    else
    {
        int n = 1;
        int c = getchar_ebuf(0);

        if (iscntrl(c) && (c < BS || c > CR))
        {
            ++n;
        }

        mvwchgat(d.edit, d.row, d.col, n, A_REVERSE, EDIT, NULL);
    }
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
    if (n != TABSIZE)                   // Making a change?
    {
        if (n == 0)
        {
            n = DEFAULT_TABSIZE;        // Use default size
        }

        set_tabsize(n);                 // Yes, so set new tab size
        touchwin(d.edit);           // And force edit window update
        wrefresh(d.edit);
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

#if     defined(DEBUG_DPY)

        if (dpy_fp == NULL)
        {
            dpy_fp = fopen("teco.dpy", "w+");

            assert(dpy_fp != NULL);

            setvbuf(dpy_fp, NULL, _IONBF, 0uL);

            bugprint("TECO display log opened\n");
        }

#endif

        reset_term();                   // Reset if display mode support

        f.e0.display = true;

#if 0                               // TODO: add soft labels?
        if (f.e4.labels)
        {
            slk_init(2);
        }
#endif

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

#if 0                               // TODO: add soft labels?
        slk_set(1,  "F1",  1);
        slk_set(2,  "F2",  1);
        slk_set(3,  "F3",  1);
        slk_set(4,  "F4",  1);
        slk_set(5,  "F5",  1);
        slk_set(6,  "F6",  1);
        slk_set(7,  "F7",  1);
        slk_set(8,  "F8",  1);
        slk_set(9,  "F9",  1);
        slk_set(10, "F10", 1);
        slk_set(11, "F11", 1);
        slk_set(12, "F12", 1);

        slk_refresh();
#endif

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
    char buf[STATUS_WIDTH - 2];

    int nbytes = snprintf(buf, sizeof(buf), "%s:", header);
    int rem = (int)(sizeof(buf) - nbytes);

    snprintf(buf + nbytes, rem, "%*s", rem - 1, data);

    mvwprintw(d.status, line, 2, buf);
}


///
///  @brief    Update status window and divider line.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void update_status(void)
{
    if (f.e4.status)
    {
        char buf[STATUS_WIDTH + 1];

        // Output current position and no. of characters in edit buffer

        snprintf(buf, sizeof(buf), FMT "/" FMT, t.dot, t.Z);
        status_line(0, "./Z", buf);

        // Output row and column for display

        int row = (int)getlines_ebuf(-1) + 1;
        int col = (int)-getdelta_ebuf((int_t)0) + 1;

        if (t.dot >= t.Z)
        {
            status_line(1, "pos", "EOF");
        }
        else
        {
            snprintf(buf, sizeof(buf), FMT "/" FMT, row, col);
            status_line(1, "pos", buf);
        }

        // Output no. of lines in file

        snprintf(buf, sizeof(buf), FMT, getlines_ebuf(0));
        status_line(2, "lines", buf);

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

        status_line(3, "memory", buf);

        // Output page number

        snprintf(buf, sizeof(buf), "%d", page_count());
        status_line(4, "page", buf);

        // Output vertical line to divide command window from status window

        //lint -save -e438 -e550
        int nrows;
        int unused __attribute__((unused));
        //line -restore

        getmaxyx(d.status, nrows, unused);
        mvwvline(d.status, 0, 0, ACS_VLINE, nrows);

        wrefresh(d.status);
    }

    if (f.e4.line)
    {
        whline(d.line, ACS_HLINE, w.width);

        // If we have a status window, then connect its vertical
        // line to our horizontal line, using a top tee character.

        if (f.e4.status)
        {
            mvwaddch(d.line, 0, w.width - STATUS_WIDTH, ACS_TTEE);
        }

        wrefresh(d.line);
    }
}
