///
///  @file    display.c
///  @brief   Display mode functions.
///
///  @copyright 2019-2021 Franklin P. Johnston / Nowwith Treble Software
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

#if     defined(DISPLAY_MODE)

#include <ncurses.h>

#endif

#if     !defined(_STDIO_H)

#include <stdio.h>

#endif

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"

#if     defined(DISPLAY_MODE)

#include "ascii.h"
#include "errcodes.h"

#endif

#include "display.h"
#include "editbuf.h"
#include "eflags.h"
#include "estack.h"
#include "exec.h"
#include "page.h"
#include "term.h"


static int botdot = 0;              ///< Value of FZ flag

#if     defined(DISPLAY_MODE)

#if     defined(LONG_64)

#define DEC_FMT "%-*ld"             ///< Left-justified decimal format

#else

#define DEC_FMT "%-*d"              ///< Left-justified decimal format

#endif

static int rowbias = 0;             ///< Row adjustment

static uint n_home = 0;             ///< No. of consecutive Home keys

static uint n_end = 0;              ///< No. of consecutive End keys

///
///  @def    err_if_true
///
///  @brief  Issue error if function returns specified value.
///

#define err_if_true(func, cond) if (func == cond) error_dpy()

///
///  @struct  region
///
///  @brief   Characteristics of a screen region
///

struct region
{
    int top;                        ///< Top of region
    int bot;                        ///< Bottom of region
};

///
///  @struct  display
///
///  @brief   Display format
///

struct display
{
    int row;                        ///< Edit row
    int col;                        ///< Edit column
    int vcol;                       ///< Virtual column
    int nrows;                      ///< No. of edit rows
    struct region cmd;              ///< Command region
    struct region edit;             ///< Edit region
    struct region status;           ///< Status line
};

///
///  @var     d
///
///  @brief   Display format
///

static struct display d =
{
    .row    = 0,
    .col    = 0,
    .vcol   = 0,
    .nrows  = 0,
    .cmd    = { .top = 0, .bot = 0 },
    .edit   = { .top = 0, .bot = 0 },
    .status = { .top = 0, .bot = 0 },
};


// Local functions

static void error_dpy(void);

static void exec_commands(const char *string);

static int geteditsize(char *buf, ulong size, uint_t bytes);

static int getwidth(ulong n);

static void mark_cursor(int row, int col);

static void move_down(void);

static void move_left(void);

static void move_right(void);

static void move_up(void);

static int print_ebuf(char *buf, int width, int nbytes, int c);

static void update_status(void);

#endif


///
///  @brief    Check to see if escape sequences were enabled or disabled.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

void check_escape(uint escape)
{
    if (f.ed.escape ^ escape)           // Any change?
    {
        (void)keypad(stdscr, f.ed.escape ? (bool)TRUE : (bool)FALSE);
    }
}

#endif


///
///  @brief    Clear to end of line.
///
///  @returns  true if success, false if we couldn't.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

bool clear_eol(void)
{
    if (f.et.scope && f.e0.display)
    {
        (void)printw("\r");
        (void)clrtoeol();
        (void)refresh();

        return true;
    }

    return false;
}

#endif


///
///  @brief    Clear screen and redraw display.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void clear_dpy(void)
{

#if     defined(DISPLAY_MODE)

    term_pos = 0;

    (void)clear();

    ebuf_changed = true;

    set_scroll(w.height, w.nlines);

    refresh_dpy();

#endif

}


///
///  @brief    Reset display and issue error. When TECO starts, we either try
///            to initialize for display mode, or initialize terminal settings.
///            So if display initialization fails, we have to ensure that the
///            terminal is set up correctly.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

static void error_dpy(void)
{
    reset_dpy();
    init_term();

    throw(E_DPY);                       // Display mode initialization
}

#endif


///
///  @brief    Execute command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

static void exec_commands(const char *commands)
{
    assert(commands != NULL);

    uint nbytes = (uint)strlen(commands);
    char text[nbytes + 1];

    strcpy(text, commands);

    tbuffer buf;

    buf.data = text;
    buf.size = nbytes;
    buf.len  = buf.size;
    buf.pos  = 0;

    bool saved_exec = f.e0.exec;

    // The reason for the next line is that we are called from readkey_dpy(),
    // which in turn is called when we are processing character input. So the
    // execution flag isn't on at this point, but we need to temporarily force
    // it in order to process an immediate-mode command string initiated by a
    // special key such as Page Up or Page Down.

    f.e0.exec = true;                   // Force execution

    exec_macro(&buf, NULL);

    f.e0.exec = saved_exec;             // Restore previous state

    refresh_dpy();
}

#endif


///
///  @brief    Reset display mode prior to exiting from TECO.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

void exit_dpy(void)
{
    reset_dpy();
}

#endif


///
///  @brief    Get next character.
///
///  @returns  Character read, or -1 if no character available.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

int getchar_dpy(bool wait)

#else

int getchar_dpy(bool unused)

#endif

{
    int c = 0;                          // Ensure that high bits are clear

#if     defined(DISPLAY_MODE)

    if (f.e0.display)
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

        if (c == KEY_RESIZE)
        {
            set_nrows();
            clear_dpy();
            print_prompt();

            return EOF;
        }
        else if (c == KEY_BACKSPACE)
        {
            return DEL;
        }

        return c;
    }

    // If display mode support isn't compiled in, or it's not currently
    // active, then read a character through alternate means. Note that
    // if display mode is active, getch() always returns immediately,
    // which is why we usually call read() to get a single character.

    if (!wait)
    {
        return getch();
    }

#endif

    if (read(fileno(stdin), &c, 1uL) == -1)
    {
        return EOF;
    }
    else
    {
        return c;
    }
}


///
///  @brief    Get size of edit buffer.
///
///  @returns  No. of bytes written to buffer.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

static int geteditsize(char *buf, ulong size, uint_t bytes)
{
    assert(buf != NULL);

    const uint gbytes = (uint)(bytes / GB);
    const uint mbytes = (uint)(bytes / MB);
    const uint kbytes = (uint)(bytes / KB);

    if (gbytes != 0)
    {
        return snprintf(buf, size, "%uG", gbytes);
    }
    else if (mbytes != 0)
    {
        return snprintf(buf, size, "%uM", mbytes);
    }
    else if (kbytes != 0)
    {
        return snprintf(buf, size, "%uK", kbytes);
    }
    else
    {
        return snprintf(buf, size, "%u", (uint)bytes);
    }
}

#endif


///
///  @brief    Get width of unsigned number (basically, log10() without using
///            a floating-point library function).
///
///  @returns  No. of bytes written to buffer.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

static int getwidth(ulong bytes)
{
    char buf[12];

    return snprintf(buf, sizeof(buf), "%lu", bytes);
}

#endif


///
///  @brief    Initialize for display mode.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

void init_dpy(void)
{
    if (!f.e0.display)
    {
        f.e0.display = true;

        // Note that initscr() will print an error message and exit if it
        // fails to initialize, so there is no error return to check for.

        (void)initscr();

        err_if_true(cbreak(),                       ERR);
        err_if_true(noecho(),                       ERR);
        err_if_true(nonl(),                         ERR);
        err_if_true(notimeout(stdscr, (bool)TRUE),  ERR);
        err_if_true(idlok(stdscr,     (bool)TRUE),  ERR);
        err_if_true(scrollok(stdscr,  (bool)TRUE),  ERR);
        err_if_true(has_colors(),                   FALSE);
        err_if_true(start_color(),                  ERR);

        err_if_true(keypad(stdscr, f.ed.escape ? (bool)TRUE : (bool)FALSE), ERR);

        reset_colors();
        (void)set_escdelay(0);

        (void)attrset(COLOR_PAIR(CMD)); //lint !e835 !e845

        set_nrows();
    }
}

#endif


///
///  @brief    Mark or unmark cursor at current position.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

static void mark_cursor(int row, int col)
{
    // Save current position

    int saved_row, saved_col;
    uint c;

    getyx(stdscr, saved_row, saved_col);

    (void)attrset(COLOR_PAIR(EDIT));    //lint !e835

    // Go to old cursor position

    (void)move(d.edit.top + d.row, d.col);

    c = inch() & A_CHARTEXT;            //lint !e835

    (void)delch();
    (void)insch(c);

    // Go to new cursor position

    d.row = row;
    d.col = col;

    (void)move(d.edit.top + d.row, d.col);

    c = inch() | A_REVERSE;

    (void)delch();
    (void)insch(c);

    // Restore old position and color

    (void)attrset(COLOR_PAIR(CMD));     //lint !e835 !e845
    (void)move(d.edit.top + saved_row, saved_col);
}

#endif


///
///  @brief    Move cursor down.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

static void move_down(void)
{
    int_t line = getlines_ebuf((int_t)-1); // Get current line number
    int row = d.row;
    int col = d.col;

    if (line == getlines_ebuf((int_t)0)) // On last line?
    {
        return;
    }

    if (row == d.nrows - 1)
    {
        ++rowbias;
    }

    ++row;

    int next = (int)getdelta_ebuf((int_t)1); // Start of next line
    int len = (int)getdelta_ebuf((int_t)2) - next; // Length of next line
    int_t dot = t.dot + next;

    if (col < d.vcol)
    {
        col = d.vcol;                   // Use virtual column if we can
    }

    if (len < col)
    {
        dot += len - 1;
        col = len - 1;
    }
    else
    {
        dot += col;
    }

    if (dot > t.Z)                      // Make sure we stay within buffer
    {
        dot = t.Z;
    }

    mark_cursor(row, col);

    setpos_ebuf(dot);

    dot_changed = false;                // Force this off for down arrow

    update_status();
    refresh_dpy();

    if (d.vcol < d.col)
    {
        d.vcol = d.col;                 // Update virtual column if needed
    }
}

#endif


///
///  @brief    Move cursor left.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

static void move_left(void)
{
    int_t dot = t.dot - 1;

    if (dot >= t.B)
    {
        int line = (int)getlines_ebuf((int_t)-1);
        int row = (line - rowbias) % d.nrows;

        setpos_ebuf(dot);

        if (row == 0 && line != getlines_ebuf((int_t)-1))
        {
            --rowbias;
        }

        refresh_dpy();

        d.vcol = d.col;                 // Update virtual column
    }
}

#endif


///
///  @brief    Move cursor right.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

static void move_right(void)
{
    int_t dot = t.dot + 1;

    if (dot <= t.Z)
    {
        int line = (int)getlines_ebuf((int_t)-1);
        int row = (line - rowbias) % d.nrows;

        setpos_ebuf(dot);

        if (row == d.nrows - 1 && line != getlines_ebuf((int_t)-1))
        {
            ++rowbias;
        }

        refresh_dpy();

        d.vcol = d.col;                 // Update virtual column
    }
}

#endif


///
///  @brief    Move cursor up.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

static void move_up(void)
{
    int line = (int)getlines_ebuf((int_t)-1); // Get current line number
    int row = d.row;
    int col = d.col;

    if (line == 0)                      // On first line?
    {
        return;                         // Yes, nothing to do
    }

    if (row == 0)
    {
        --rowbias;
    }

    --row;

    int prev = (int)-getdelta_ebuf((int_t)-1); // Distance to start of previous
    int len = prev - col;               // Length of previous line
    int_t dot = t.dot - (int_t)prev;

    if (col < d.vcol)
    {
        col = d.vcol;                   // Use virtual column if we can
    }

    if (len < col)
    {
        dot += len - 1;
        col = len - 1;
    }
    else
    {
        dot += col;
    }

    mark_cursor(row, col);

    setpos_ebuf(dot);

    dot_changed = false;                // Force this off for up arrow

    update_status();
    refresh_dpy();

    if (d.vcol < d.col)
    {
        d.vcol = d.col;                 // Update virtual column if needed
    }
}

#endif


///
///  @brief    Output formatted description of edit buffer character.
///
///  @returns  No. of characters written to buffer.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

static int print_ebuf(char *buf, int width, int nbytes, int c)
{
    assert(buf != NULL);

    size_t size = (uint)(width - nbytes);

    buf += nbytes;

    if (c == EOF)
    {
        return snprintf(buf, size, "----");
    } 
    else if (isprint(c))
    {
        return snprintf(buf, size, "'%c' ", c);
    }
    else if (c > NUL && c < SPACE)
    {
        return snprintf(buf, size, "'^%c'", c + '@');
    }
    else
    {
        return snprintf(buf, size, "% 4d", c);
    }
}

#endif


///
///  @brief    Output character to display. We do not output CR because ncurses
///            does the following when processing LF:
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

#if     defined(DISPLAY_MODE)

bool putc_dpy(int c)
{
    if (f.e0.display)
    {
        if (c != CR)
        {
            (void)addch((uint)c);
        }

        return true;
    }

    return false;
}

#endif


///
///  @brief    Read display key.
///
///  @returns  Character to process, or EOF if already processed.
///
////////////////////////////////////////////////////////////////////////////////

int readkey_dpy(int key)
{

#if     defined(DISPLAY_MODE)

    if (!f.e0.display)
    {
        return key;
    }
    else if (exec_key(key))
    {
        ;
    }
    else if (key == KEY_HOME)
    {
        n_end = 0;

        if (++n_home == 1)              // Beginning of line
        {
            exec_commands("0L");
        }
        else if (n_home == 2)           // Beginning of window
        {
            exec_commands("F0J");
        }
        else                            // Beginning of file
        {
            exec_commands("0J");
        }

        return EOF;
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

            exec_commands("L (-2A-13)\"E 2R | R '");
        }
        else if (n_end == 2)            // End of window
        {
            exec_commands("(FZ-1)J");
        }
        else                            // End of file
        {
            exec_commands("ZJ");
        }

        return EOF;
    }
    else if (key == KEY_PPAGE)
    {
        exec_commands("-(2:W)L");
    }
    else if (key == KEY_NPAGE)
    {
        exec_commands("(2:W)L");
    }
    else if (key == KEY_UP)
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
    }
    else if (key == KEY_RIGHT)
    {
        move_right();
    }
    else if (key == CR || key == LF || key == ESC ||
            (key == ACCENT && f.et.accent) || key == f.ee)
    {
        if (w.nlines == 0 || w.noscroll)
        {
            exec_commands(".-Z \"N L T '");
        }
        else
        {
            exec_commands("L");
        }
    }
    else if (key == BS || key == DEL)
    {
        if (w.nlines == 0 || w.noscroll)
        {
            exec_commands(".-B \"N -L T '");
        }
        else
        {
            exec_commands("-L");
        }
    }
    else
    {
        n_home = n_end = 0;

        return key;
    }

    n_home = n_end = 0;

    return EOF;

#else

    return key;

#endif

}


///
///  @brief    Refresh screen.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void refresh_dpy(void)
{

#if     defined(DISPLAY_MODE)

    if (!f.e0.display || w.nlines == 0 || w.noscroll)
    {
        return;
    }

    int line = (int)getlines_ebuf((int_t)-1); // Line number within buffer

    if (line == 0)
    {
        rowbias = 0;
    }

    int row  = (line - rowbias) % d.nrows; // Relative row within screen
    int_t pos  = getdelta_ebuf((int_t)-row); // First character to output

    if (ebuf_changed)
    {
        ebuf_changed = false;

        if (dot_changed)
        {
            dot_changed = false;

            d.vcol = 0;
        }

        int saved_row, saved_col;

        getyx(stdscr, saved_row, saved_col); // Save position in command region

        (void)move(d.edit.top, 0);      // Switch to edit region

        getyx(stdscr, d.row, d.col);    // Get starting position

        (void)attrset(COLOR_PAIR(EDIT)); //lint !e835

        int c;
        int nrows = d.nrows;

        // Erase the current edit region

        while (nrows-- > 0)
        {
            (void)addch('\n');
        }

        nrows = 0;

        (void)move(d.edit.top, 0);      // Back to the top

        uint line_pos = 0;              // Line position
        bool filled = false;            // Is edit region full?

        w.topdot = botdot = (int)(t.dot + pos);

        while ((c = getchar_ebuf(pos)) != EOF)
        {
            if (pos++ <= 0)
            {
                getyx(stdscr, d.row, d.col);
            }

            if (c == CR)
            {
                ++botdot;

                continue;
            }

            if (isdelim(c))
            {
                ++botdot;

                if (++nrows == d.nrows)
                {
                    filled = true;

                    break;
                }

                (void)move(d.edit.top + nrows, 0);

                line_pos = 0;
            }
            else
            {
                line_pos += (uint)strlen(unctrl((uint)c));

                if (line_pos > (uint)w.width)
                {
                    if (f.et.truncate)
                    {
                        ++botdot;

                        continue;
                    }

                    if (++nrows == d.nrows)
                    {
                        filled = true;

                        break;
                    }

                    (void)move(d.edit.top + nrows, 0);

                    line_pos = 0;
                }

                ++botdot;

                (void)addch((uint)c);
            }
        }

        // If at end of edit buffer, adjust cursor

        if (pos == 0)
        {
            getyx(stdscr, d.row, d.col);
        }

        // If at end of buffer, and if room for it, add marker

        if (!filled && getchar_ebuf(pos) == EOF)
        {
            (void)addch(A_ALTCHARSET | 0x60);
        }

        // Highlight our current position in edit region

        (void)move(d.row, d.col);

        c = (int)inch();

        (void)delch();
        (void)insch((uint)c | A_REVERSE);

        // Restore position in command region

        (void)move(saved_row, saved_col);
        (void)attrset(COLOR_PAIR(CMD)); //lint !e835 !e845
    }

    update_status();

    (void)refresh();

#endif

}


///
///  @brief    Reset region colors to defaults.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_colors(void)
{

#if     defined(DISPLAY_MODE)

    if (can_change_color())             // Make colors as bright as possible
    {
        (void)init_color(COLOR_BLACK,        0,      0,      0);
        (void)init_color(COLOR_RED,     SATMAX,      0,      0);
        (void)init_color(COLOR_GREEN,        0, SATMAX,      0);
        (void)init_color(COLOR_YELLOW,  SATMAX, SATMAX,      0);
        (void)init_color(COLOR_BLUE,         0,      0, SATMAX);
        (void)init_color(COLOR_MAGENTA, SATMAX,      0, SATMAX);
        (void)init_color(COLOR_CYAN,         0, SATMAX, SATMAX);
        (void)init_color(COLOR_WHITE,   SATMAX, SATMAX, SATMAX);
    }

    (void)assume_default_colors(COLOR_BLACK, COLOR_WHITE);

    (void)init_pair(CMD,    COLOR_BLACK, COLOR_WHITE);
    (void)init_pair(EDIT,   COLOR_BLACK, COLOR_WHITE);
    (void)init_pair(STATUS, COLOR_WHITE, COLOR_BLACK);

#endif

}


///
///  @brief    Terminate display mode.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

void reset_dpy(void)
{
    if (f.e0.display)
    {
        f.e0.display = false;

        (void)endwin();
    }
}

#endif


///
///  @brief    Resize window for display mode.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

void resize_dpy(void)
{
    if (f.e0.display)
    {
        (void)resizeterm(w.height, w.width);
        getmaxyx(stdscr, w.height, w.width);
    }
}

#endif


///
///  @brief    Scan "F0" command: return position for top left corner.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_F0(struct cmd *unused)
{
    push_x((int_t)w.topdot, X_OPERAND);

    return true;
}


///
///  @brief    Scan "FH" command: equivalent to F0,FZ.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_FH(struct cmd *cmd)
{
    assert(cmd != NULL);

    cmd->n_set = true;
    cmd->n_arg = w.topdot;
    cmd->h     = true;

    push_x((int_t)botdot, X_OPERAND);

    return true;
}


///
///  @brief    Scan "FZ" command: return position for bottom right corner.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_FZ(struct cmd *unused)
{
    push_x((int_t)botdot, X_OPERAND);

    return true;
}


///
///  @brief    Set maximum no. of rows.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void set_nrows(void)
{

#if     defined(DISPLAY_MODE)

    d.nrows = w.height - w.nlines;

    if (f.e4.line)
    {
        --d.nrows;
    }

    assert(d.nrows > 0);                // Verify that we have at least 1 row

#endif

}


///
///  @brief    Set scrolling region.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

void set_scroll(int height, int nlines)

#else

void set_scroll(int unused1, int unused2)

#endif

{

#if     defined(DISPLAY_MODE)

    if (f.e0.display && w.nlines != 0 && !w.noscroll)
    {
        if (f.e4.invert)
        {
            d.cmd.top  = 0;
            d.cmd.bot  = nlines - 1;
            d.edit.top = nlines;
        }
        else
        {
            d.cmd.top  = height - nlines;
            d.cmd.bot  = height - 1;
            d.edit.top = 0;
        }

        (void)setscrreg(d.cmd.top, d.cmd.bot);

        d.status.top = d.status.bot = -1;

        if (f.e4.line)
        {
            if (f.e4.invert)
            {
                d.status.top = d.status.bot = d.cmd.bot + 1;
                ++d.edit.top;
            }
            else
            {
                d.status.top = d.status.bot = d.cmd.top - 1;
            }

            update_status();
        }

        (void)move(d.cmd.top, 0);

        for (int i = d.cmd.top; i <= d.cmd.bot; ++i)
        {
            (void)addch('\n');
        }

        (void)attrset(COLOR_PAIR(CMD)); //lint !e835 !e845

        (void)move(d.cmd.top, 0);

        (void)refresh();

        set_nrows();
    }

#endif

}


///
///  @brief    Update status line.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DISPLAY_MODE)

static void update_status(void)
{
    if (!f.e4.line)
    {
        return;
    }

    // Draw line between edit region and command region

    int saved_row, saved_col;

    getyx(stdscr, saved_row, saved_col);

    (void)move(d.status.top, 0);
    (void)attrset(COLOR_PAIR(STATUS));  //lint !e835

    if (f.e4.status)
    {
        char status[w.width];

        memset(status, SPACE, (size_t)(uint)w.width);

        // Add some file status to the left side of the status line

        int row     = (int)getlines_ebuf((int_t)-1);
        int nrows   = (int)getlines_ebuf((int_t)0);
        int col     = (int)-getdelta_ebuf((int_t)0);
        int width   = getwidth((ulong)(uint)t.Z);
        int nbytes  = snprintf(status, sizeof(status), ".=" DEC_FMT " (",
                               width, t.dot);
        size_t size = sizeof(status);   // Remaining bytes available in line

        nbytes += print_ebuf(status, w.width, nbytes, getchar_ebuf((int_t)-1));
        nbytes += snprintf(status + nbytes, size, ",");
        nbytes += print_ebuf(status, w.width, nbytes, getchar_ebuf((int_t)0));

        size = sizeof(status) - (uint)nbytes;

        nbytes += snprintf(status + nbytes, size, ")  Z=" DEC_FMT " ",
                           width, t.Z);
        width = getwidth((ulong)(uint)nrows);

        if (t.dot >= t.Z)
        {
            nbytes += snprintf(status + nbytes, size,
                               "row=%-*d   <EOF>   ", width, row + 1);
        }
        else
        {
            nbytes += snprintf(status + nbytes, size,
                               "row=%-*d  col=%-3d  ", width, row + 1, col + 1);
        }

        size = sizeof(status) - (uint)nbytes;

        nbytes += snprintf(status + nbytes, size, "nrows=%-*d  mem=", width,
                           nrows);

        size = sizeof(status) - (uint)nbytes;

        nbytes += geteditsize(status + nbytes, size, getsize_ebuf());

        status[nbytes] = SPACE;         // Replace NUL character with space

        // Now add in page number on right side.

        char buf[w.width];

        nbytes = sprintf(buf, "Page %u", page_count);

        memcpy(status + w.width - nbytes, buf, (size_t)(uint)nbytes);

        for (int i = 0; i < w.width; ++i)
        {
            int c = status[i];

            (void)addch((uint)c);
        }
    }
    else
    {
        for (int i = 0; i < w.width; ++i)
        {
            (void)addch(ACS_HLINE);
        }
    }

    (void)move(saved_row, saved_col);
    (void)attrset(COLOR_PAIR(CMD));     //lint !e835 !e845
}

#endif
