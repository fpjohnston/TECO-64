///
///  @file    status.c
///  @brief   Display functions that support the status window.
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
#include <ncurses.h>
#include <stdio.h>

#define DISPLAY_INTERNAL            ///< Enable internal definitions

#include "teco.h"
#include "display.h"
#include "editbuf.h"
#include "exec.h"
#include "page.h"


#if     INT_T == 64

#define FMT     "%ld"               ///< 64-bit numeric format

#else

#define FMT     "%d"                ///< 32-bit numeric format

#endif


// Local functions

static void status_line(int line, const char *header, const char *data);


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
        char buf[STATUS_WIDTH + 1];
        int nrows, line = 0;

        //lint -save -e438 -e550
        int unused __attribute__((unused));
        //lint -restore

        getmaxyx(d.status, nrows, unused);

        // Output current position and no. of characters in edit buffer

        snprintf(buf, sizeof(buf), FMT "/" FMT, t->dot, t->Z);
        status_line(line++, "./Z", buf);

        // Output row and column for display

        int row = t->before;

        if (t->dot == t->Z)
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

        snprintf(buf, sizeof(buf), FMT, t->total);
        status_line(line++, "Lines", buf);

        // Output memory size

        uint_t memsize = t->size;

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
        whline(d.line, ACS_HLINE, d.ncols);

        // If we have a status window, then connect its vertical line to the
        // horizontal line we just printed, using a top tee character if the
        // the edit window is on top of the command window, or a bottom tee
        // character if the command window is on top.

        if (f.e4.status)
        {
            chtype ch = (f.e4.invert) ? ACS_BTEE : ACS_TTEE;

            mvwaddch(d.line, 0, w.width, ch);
        }

        wrefresh(d.line);
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
