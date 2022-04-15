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
    d.newrow = d.row;
    d.newcol = d.col;

    if (d.oldcol < d.newcol)
    {
        d.oldcol = d.newcol;
    }

    int pos = t->len - t->pos;          // Go to start of next line
    int_t delta = count_chrs(pos, d.oldcol);

    move_dot(delta);

    if (key == KEY_C_DOWN)
    {
        d.updown = true;
    }
    else if (d.newrow < d.nrows - 1)
    {
        ++d.newrow;
    }

    d.newcol = find_column();
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
    if (t->dot == t->Z)
    {
        return;
    }
            
    d.newrow = d.row;
    d.newcol = d.col;

    // Here to process End and Ctrl-End keys

    bool eol = (isdelim(t->c) || (t->c == CR && t->nextc == LF));
    
    if (!eol)                           // If not at end of line
    {
        int_t delta = t->len - (t->pos + 1);

        if (key == KEY_C_END)
        {
            int_t col = find_column();

            if (col < d.maxcol)
            {
                delta = count_chrs(t->pos, d.maxcol);
            }
        }

        move_dot(delta);
    }
    else if (t->dot < w.botdot - 1)     // If not at end of window
    {
        int_t line = after_dot() - 1;

        d.newrow += line;

        set_dot(w.botdot - 1);
    }
    else                                // Check for end of buffer
    {
        if (key == KEY_END || (t->dot == t->Z - 1 && isdelim(t->c)))
        {
            set_dot(t->Z);

            ++d.newrow;
            d.xbias = 0;
        }
        else
        {
            int_t delta = len_edit(d.nrows);

            move_dot(delta);

            delta = t->len - (t->pos + 1);

            move_dot(delta);

            d.newrow += d.nrows - 1;
        }
    }

    if (d.newrow > d.nrows - 1)
    {
        d.newrow = d.nrows - 1;
    }

    d.newcol = find_column();

    // Make sure horizontal bias is correct for column

    while (d.newcol > d.xbias + d.maxcol)
    {
        d.xbias += d.maxcol + 1;
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
    if (t->dot == t->B)
    {
        return;
    }
            
    d.newrow = d.row;
    d.newcol = d.col;

    // Here to process Home and Ctrl/Home keys

    if (d.xbias != 0 && key == KEY_C_HOME) // Shift to the left
    {
        if (d.newcol == d.xbias && (d.xbias -= d.ncols) < 0)
        {
            d.xbias = 0;
        }

        d.newrow = d.row;
        d.newcol = d.xbias;

        int_t delta = count_chrs(-t->pos, d.newcol);

        move_dot(delta);
    }
    else if (d.newcol != 0)             // Go to start of line
    {
        d.newcol = 0;
        d.xbias = 0;

        move_dot(-t->pos);
    }
    else if (t->dot != w.topdot)
    {                                   // Go to top of window
        d.newrow = 0;
        d.xbias = 0;

        set_dot(w.topdot);
    }
    else                               // Go to top of buffer
    {
        int line = before_dot();

        d.newrow = 0;
        d.xbias = 0;

        if (key == KEY_HOME || line <= d.nrows)
        {
            set_dot(t->B);
        }
        else
        {
            int_t delta = len_edit(-d.nrows);

            move_dot(delta);
        }
    }

    d.newcol = find_column();
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
    if (t->dot == t->B)
    {
        return;
    }

    d.newrow = d.row;
    d.newcol = d.col;

    set_dot(t->dot - 1);

    if (isdelim(t->c))                  // Did we just back up over a delimiter?
    {
        --d.newrow;                     // Yes, we're on the previous row
    }

    d.newcol = find_column();

    if (d.newcol == 0)                  // If we're at first column,
    {
        d.newcol = -t->pos;             //  go to end of previous line
        d.xbias = 0;
    }
    else if (key == KEY_C_LEFT)         // If Ctrl-Left,
    {
        --d.xbias;                      //  then shift window left 1 chr.
    }
    else if (d.newcol <= d.xbias)       // If column outside of window,
    {
        d.xbias -= d.ncols;             //  then shift window left
    }

    if (d.xbias > d.ncols)              // Keep horizontal bias reasonable
    {
        d.xbias = 0;
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
    if (t->dot == t->Z)
    {
        return;
    }

    d.newrow = d.row;
    d.newcol = d.col;

    set_dot(t->dot + 1);

    if (isdelim(t->lastc))              // Did we just advance over a delimiter?
    {
        ++d.newrow;                     // Yes, we're on the next row
    }

    d.newcol = find_column();

    if (d.newcol == 0)                  // If we returned to first column,
    {
        d.xbias = 0;                    //  then remove any horizontal bias
    }
    else if (key == KEY_C_RIGHT)        // If Ctrl-Right,
    {
        ++d.xbias;                      //  then shift window right 1 chr.
    }
    else if (d.newcol >= d.xbias + d.ncols - 1) // If column outside of window,
    {
        d.xbias += d.ncols;             //  then shift window right
    }

    if (d.xbias > d.ncols)              // Keep horizontal bias reasonable
    {
        d.xbias = d.ncols;
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
    int pos = len_edit(-1);             // Go to start of previous line

    d.newrow = d.row;
    d.newcol = d.col;

    if (d.oldcol < d.newcol)
    {
        d.oldcol = d.newcol;
    }

    int_t delta = count_chrs(pos, d.oldcol);

    move_dot(delta);

    if (key == KEY_C_UP)
    {
        d.updown = true;
    }
    else if (d.newrow > 0)
    {
        --d.newrow;
    }

    d.newcol = find_column();
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
        if (!w.seeall || isascii(c))    // SEEALL mode or 7-bit character
        {
            if (w.seeall || c < BS || c > CR)
            {
                keysize[c] = (char)strlen(unctrl((chtype)c));
            }
            else                        // If special chrs. w/ SEEALL mode,
            {
                keysize[c] = 1;         //  then pretend they're 1 chr. wide
            }
        }
        else                            // 8-bit char. w/ SEEALL mode
        {
            keysize[c] = (char)strlen(table_8bit[c & 0x7f]);
        }
    }

    f.e0.window = true;                 // Window refresh needed
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

        f.e0.window = true;             // Window refresh needed
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
