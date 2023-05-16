///
///  @file    status.c
///  @brief   Display functions that support the status window.
///
///  @copyright 2019-2023 Franklin P. Johnston / Nowwith Treble Software
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

#define DISPLAY_INTERNAL            ///< Enable internal definitions

#include "teco.h"
#include "display.h"
#include "editbuf.h"
#include "eflags.h"
#include "page.h"
#include "term.h"


#if     INT_T == 64

#define FMT     "%ld"               ///< 64-bit format for snprintf()

#else

#define FMT     "%d"                ///< 32-bit format for snprintf()

#endif

/// @def    check_line(line,maxline)
/// @brief  Verifies that status window has room for another line.

#define check_line(line, maxline) if (line == maxline) return;

// Local functions

static void print_status(int nrows);

static void status_line(int line, const char *header, const char *data);


///
///  @brief    Print status information.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void print_status(int maxline)
{
    char buf[STATUS_WIDTH + 1];
    int line = 0;

    // Output current character value

    const char *chr;

    if (t->dot == t->Z)
    {
        snprintf(buf, sizeof(buf), "EOF");
    }
    else
    {
        if (isascii(t->c) || !w.seeall)
        {
            chr = unctrl((chtype)t->c);
        }
        else
        {
            chr = table_8bit[t->c & 0x7f];
        }

        snprintf(buf, sizeof(buf), FMT "=%s", t->c, chr);
    }

    status_line(line++, "character", buf);
    check_line(line, maxline);

    // Output current position ('dot') and total no. of chrs.

    snprintf(buf, sizeof(buf), FMT "," FMT, t->dot, t->Z);
    status_line(line++, "dot,Z", buf);
    check_line(line, maxline);

    // Output current line no. and total no. of lines.

    int before = before_dot();
    int total  = before + after_dot();

    // The following is a (hopefully temporary) hack to handle the situation
    // where the buffer has data but no line delimiter.

    if (total == 0 && t->Z != 0)
    {
        total = 1;                      // Must have at least 1 line if data
    }

    if (t->dot < t->Z)
    {
        if (t->Z != 0)
        {
            ++before;
        }

        snprintf(buf, sizeof(buf), FMT "/" FMT, before, total);
    }
    else
    {
        snprintf(buf, sizeof(buf), "EOF/" FMT, total);
    }

    status_line(line++, "line", buf);
    check_line(line, maxline);

    // Output current position in line and length of line

    snprintf(buf, sizeof(buf), FMT "/" FMT, t->pos, t->len);

    status_line(line++, "offset", buf);
    check_line(line, maxline);

    // Output current column and maximum allowed column

    snprintf(buf, sizeof(buf), FMT "/" FMT, d.col, w.maxline);
    status_line(line++, "column", buf);
    check_line(line, maxline);

    // Output page count

    snprintf(buf, sizeof(buf), FMT, page_count());
    status_line(line++, "page", buf);
    check_line(line, maxline);

    // Output memory size

    uint_t memsize = t->size;
    const char *memtype;

    if (memsize >= GB)
    {
        memsize /= GB;
        memtype = "GB";
    }
    else if (memsize >= MB)
    {
        memsize /= MB;
        memtype = "MB";
    }
    else
    {
        memsize /= KB;
        memtype = "KB";
    }

    // Output memory size

    snprintf(buf, sizeof(buf), FMT " %s", memsize, memtype);
    status_line(line++, "memory", buf);
    check_line(line, maxline);
}


///
///  @brief    Refresh status window and divider line.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void refresh_status(void)
{
    if (f.e4.status)
    {
        int nrows;

        //lint -save -e438 -e550
        int unused __attribute__((unused));
        //lint -restore

        getmaxyx(d.status, nrows, unused);

        if (nrows > 0)
        {
            print_status(nrows);

            // Output vertical line to divide command window from status window

            chtype ch = ACS_VLINE | COLOR_PAIR(LINE); //lint !e835

            mvwvline(d.status, 0, 0, ch, nrows);

            wrefresh(d.status);
        }
    }                                   //lint !e438 !e550

    if (f.e4.fence)
    {
        whline(d.fence, ACS_HLINE, d.ncols);

        // If we have a status window, then connect its vertical line to the
        // horizontal line we just printed, using a top tee character if the
        // the edit window is on top of the command window, or a bottom tee
        // character if the command window is on top.

        if (f.e4.status)
        {
            chtype ch = (f.e4.invert) ? ACS_BTEE : ACS_TTEE;
            int col = w.width;

            col -= (w.status == 0) ? STATUS_WIDTH : w.status;

            mvwaddch(d.fence, 0, col, ch);
        }

        wrefresh(d.fence);
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
    assert(header != NULL);
    assert(data != NULL);

    char buf[STATUS_WIDTH - 1];
    int nbytes = snprintf(buf, sizeof(buf), " %s", header);
    int rem = (int)sizeof(buf) - nbytes;

    snprintf(buf + nbytes, (size_t)(uint)rem, "%*.*s ", rem - 1, rem - 1, data);
    mvwprintw(d.status, line, 1, "%s", buf);
}
