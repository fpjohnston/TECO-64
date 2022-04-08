///
///  @file    key_cmd.c
///  @brief   Execute commands involving special ncurses keys.
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
#include <limits.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>

#define DISPLAY_INTERNAL            ///< Enable internal definitions

#include "teco.h"
#include "ascii.h"
#include "display.h"
#include "editbuf.h"
#include "exec.h"
#include "keys.h"
#include "term.h"


#define CMDBUF_SIZE         80      ///< Width of temporary buffer for command

#define DEFAULT_TABSIZE     8       ///< Default tab size

const bool esc_seq_def = true;      ///< Escape sequences enabled by default

char keysize[UCHAR_MAX + 1];        ///< No. of chrs. required to display each key


// Local functions

static int_t count_chrs(int_t pos, int maxcol);

static void exec_down(int key);

static void exec_end(int key);

static void exec_home(int key);

static void exec_left(int key);

static void exec_right(int key);

static void exec_up(int key);

static bool iseol(void);


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
        reset_dpy((bool)true);

        return EOF;
    }
    else
    {
        return c;
    }
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
        int width = keysize[c];

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

    reset_cursor();

    int_t delta = count_chrs(pos, d.oldcol);

    add_dot(delta);
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
    reset_cursor();

    // Here to process End and Ctrl/End keys

    if (iseol())
    {
        if (t->dot >= w.botdot - 1)      // Go to end of buffer
        {
            if (key == KEY_C_END && t->dot + 1 < t->Z)
            {
                int_t delta = getdelta_ebuf(d.nrows + 1);

                add_dot(delta);

                if (t->dot < t->Z)
                {
                    dec_dot();

                    return;
                }
            }

            d.xbias = 0;
            d.ybias = 0;

            end_dot();
        }
        else                            // Go to end of window
        {
            d.ybias = d.nrows - 1;

            set_dot(w.botdot - 1);
        }
    }
    else if (t->dot != t->Z)
    {
        int_t len = getdelta_ebuf(1) - 1;

        if (key == KEY_END)
        {
            add_dot(len);               // Go to end of line
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

            int_t delta = count_chrs(getdelta_ebuf(0), d.col);

            add_dot(delta);
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
    reset_cursor();

    // Here to process Home and Ctrl/Home keys

    if (d.xbias != 0 && key == KEY_C_HOME) // Shift to the left
    {
        if (d.col == d.xbias && (d.xbias -= d.ncols) < 0)
        {
            d.xbias = 0;
        }

        d.col = d.xbias;

        int_t delta = count_chrs(getdelta_ebuf(0), d.col);

        add_dot(delta);
    }
    else if (d.col != 0)                // Go to start of line
    {
        d.col = 0;
        d.xbias = 0;

        int_t delta = getdelta_ebuf(0);

        add_dot(delta);
    }
    else if (t->dot != w.topdot)
    {                                   // Go to top of window
        d.row = 0;
        d.ybias = 0;

        set_dot(w.topdot);
    }
    else                                // Go to top of buffer
    {
        d.row = 0;
        d.xbias = 0;
        d.ybias = 0;

        start_dot();
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
    if (t->dot > t->B)
    {
        reset_cursor();

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
            d.xbias = d.col - d.ncols;
        }
        else if (key == KEY_C_LEFT)
        {
            --d.xbias;
        }
        else if (d.col <= d.xbias)
        {
            d.xbias -= d.ncols;
        }

        if (d.xbias < 0)
        {
            d.xbias = 0;
        }

        dec_dot();
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
    if (t->dot < t->Z)
    {
        reset_cursor();

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
        else if (d.col >= d.xbias + d.ncols - 1)
        {
            d.xbias += d.ncols;
        }

        if (d.xbias > d.ncols)
        {
            d.xbias = d.ncols;
        }

        inc_dot();
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

    reset_cursor();

    int_t delta = count_chrs(pos, d.oldcol);

    add_dot(delta);
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
///  @brief    Initialize keysize[] array with the length of each character
///            on the display. The values will depend not only on the specific
///            character, but also on whether SEEALL mode is in effect, and
///            whether 8-bit characters can be displayed.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_keys(void)
{
    for (int c = 0; c <= UCHAR_MAX; ++c)
    {
        chtype ch = (chtype)c;

        if (isprint(c))                 // Printing chr. [32-126]
        {
            keysize[c] = 1;
        }
        else if (c >= BS && c <= CR)    // Special control chrs.
        {
            if (w.seeall)
            {
                keysize[c] = (char)strlen(unctrl(ch));
            }
            else if (c == HT)
            {
                keysize[c] = -1;         // Special flag for tabs
            }
            else
            {
                keysize[c] = 0;
            }
        }
        else if (iscntrl(c))            // General control chrs.
        {
            keysize[c] = (char)strlen(unctrl(ch));
        }
        else if (f.et.eightbit)         // 8-bit chrs. w/ parity bit
        {
            keysize[c] = (char)strlen(unctrl(ch));
        }
        else                            // 8-bit chrs. w/o parity bit
        {
            keysize[c] = (char)strlen(table_8bit[c & 0x7f]);
        }
    }
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
///  @brief    Rubout character on display.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void rubout_key(int c)
{
    int len = keysize[c];               // Get width of key
    int row;
    int col;

    getyx(d.cmd, row, col);

    if (len > col)
    {
        len = col;
    }

    wmove(d.cmd, row, col - len);
    wclrtobot(d.cmd);
}


///
///  @brief    Tell ncurses when 7/8-bit setting changes.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void set_bits(bool parity)
{
    if (f.e0.display)
    {
        meta(NULL, parity ? (bool)TRUE : (bool)FALSE);

        f.e0.window = true;             // Window update is pending
    }
}


///
///  @brief    Tell ncurses when escape setting changes.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void set_escape(bool escape)
{
    keypad(d.cmd, escape ? (bool)TRUE : (bool)FALSE);
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
