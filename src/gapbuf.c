///
///  @file    gapbuf.c
///  @brief   Text buffer functions.
///
///  @copyright  2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "editbuf.h"
#include "eflags.h"
#include "term.h"


///  @var    t
///
///  @brief  Text data buffer (external)

struct text t =
{
    .B        = 0,
    .Z        = 0,
    .dot      = 0,
    .size     = 0,
};

///  @var     eb
///
///  @brief   Edit buffer data (internal)

static struct
{
    char *buf;                  ///< Start of buffer
    int size;                   ///< Current size of buffer, in bytes
    int minsize;                ///< Initial and minimum size, in bytes
    int maxsize;                ///< Maximum size, in bytes
    int stepsize;               ///< How much to increment size (percent)
    int lowsize;                ///< Low water mark for gap
    int warn;                   ///< Warning threshold (0-100%)
    int left;                   ///< No. of bytes before gap
    int gap;                    ///< No. of bytes in gap
    int right;                  ///< No. of bytes after gap
} eb =
{
    .buf      = NULL,
    .size     = 0,
    .minsize  = 0,
    .maxsize  = 0,
    .stepsize = 0,
    .lowsize  = 0,
    .warn     = 100,
    .left     = 0,
    .gap      = 0,
    .right    = 0,
};

bool ebuf_changed;              ///< true if text buffer modified

// Local functions

static void exit_ebuf(void);

static bool expand_ebuf(void);

static int last_delim(int nlines);

static int next_delim(int nlines);

static void print_size(int size);

static void shift_left(int nbytes);

static void shift_right(int nbytes);


///
///  @brief    Add character to edit buffer.
///
///  @returns  EDIT_OK, EDIT_WARN, EDIT_FULL, or EDIT_ERROR.
///
////////////////////////////////////////////////////////////////////////////////

int add_ebuf(int c)
{
    assert(eb.buf != NULL);             // Error if no edit buffer

    if (eb.gap == 0)
    {
        return EDIT_ERROR;              // Buffer is already full
    }

    if (t.dot < eb.left)
    {
        shift_right(eb.left - t.dot);
    }
    else if (t.dot > eb.left)
    {
        shift_left(t.dot - eb.left);
    }

    eb.buf[eb.left++] = (char)c;

    ebuf_changed = true;

    ++t.dot;
    ++t.Z;
    --eb.gap;

    if (eb.gap <= eb.lowsize)           // Below low water mark?
    {
        if (!expand_ebuf())             // Try to make buffer bigger
        {
            if (eb.gap == 0)
            {
                return EDIT_FULL;       // Buffer just filled up
            }
            else
            {
                return EDIT_WARN;       // Buffer is getting full
            }
        }
    }

    return EDIT_OK;                     // Insertion was successful
}


///
///  @brief    Delete n chars relative to current position.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void delete_ebuf(int n)
{
    if (t.dot == 0 && n == eb.left + eb.right)
    {
        eb.left = eb.right = 0;
    }
    else
    {
        if (t.dot < eb.left)
        {
            shift_right(eb.left - t.dot);
        }
        else if (t.dot > eb.left)
        {
            shift_left(t.dot - eb.left);
        }

        if (n < 0)
        {
            n = -n;

            if (n > eb.left)
            {
                return;
            }

            eb.left -= n;
            t.dot -= n;
        }
        else if (n > 0)
        {
            if (n > eb.right)
            {
                return;
            }

            eb.right -= n;
        }
    }

    eb.gap += n;

    t.Z = eb.left + eb.right;

    ebuf_changed = true;
}


///
///  @brief    Clean up memory before we exit from TECO.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exit_ebuf(void)
{
    free_mem(&eb.buf);

    t.B         = 0;
    t.Z         = 0;
    t.dot       = 0;
    t.size      = 0;

    eb.size     = 0;
    eb.minsize  = 0;
    eb.maxsize  = 0;
    eb.stepsize = 0;
    eb.lowsize  = 0;
    eb.warn     = 0;
    eb.left     = 0;
    eb.gap      = 0;
    eb.right    = 0;
}


///
///  @brief    Expand edit buffer.
///
///  @returns  true if able to expand, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool expand_ebuf(void)
{
    if (eb.stepsize == 0 || eb.size >= eb.maxsize)
    {
        return false;
    }

    // Buffer: [left][gap][right]

    shift_left(eb.right);

    // Buffer: [left + right][gap]

    int addsize = (eb.size * eb.stepsize) / 100;

    eb.buf   = expand_mem(eb.buf, (uint)eb.size, (uint)(eb.size + addsize));
    eb.size += addsize;
    t.size  += eb.size;

    if (eb.size > eb.maxsize)
    {
        eb.size = eb.maxsize;
    }

    eb.lowsize = eb.size - ((eb.size * eb.warn) / 100);

    shift_right(eb.right);

    // Buffer: [left][gap][right]

    eb.gap += addsize;

    print_size(eb.size);

    return true;
}


///
///  @brief    Get ASCII value of nth character before or after dot.
///
///  @returns  ASCII value, or -1 if character outside of edit buffer.
///
////////////////////////////////////////////////////////////////////////////////

int getchar_ebuf(int n)
{
    int pos = t.dot + n;

    if (pos < eb.left + eb.right)
    {
        if (pos >= eb.left)
        {
            pos += eb.gap;
        }

        return eb.buf[pos];
    }

    return -1;
}


///
///  @brief    Return number of characters between dot and nth line terminator.
///
///  @returns  Number of characters relative to dot (can be plus or minus).
///
////////////////////////////////////////////////////////////////////////////////

int getdelta_ebuf(int n)
{
    if (n > 0)
    {
        return next_delim(n) - t.dot;
    }
    else
    {
        return last_delim(-n) - t.dot;
    }

}


///
///  @brief    Count no. of lines relative to current position.
///
///  @returns  No. of total/following/preceding lines.
///
////////////////////////////////////////////////////////////////////////////////

int getlines_ebuf(int n)
{
    int start = (n > 0) ? 0 : -t.dot;
    int end   = (n < 0) ? 0 :  eb.left + eb.right;
    int nlines = 0;

    for (int pos = start; pos < end; ++pos)
    {
        int c = getchar_ebuf(pos);

        if (isdelim(c))
        {
            ++nlines;
        }
    }

    return nlines;
}


///
///  @brief    Initialize edit buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_ebuf(
    int minsize,                        ///< Initial and min. size of buffer
    int maxsize,                        ///< Maximum size of buffer, in bytes
    int stepsize,                       ///< Incremental increase, in bytes
    int warn)                           ///< Warning threshold (0-100)
{
    assert(eb.buf == NULL);             // Error if no edit buffer

    register_exit(exit_ebuf);

    if (warn > 100)                     // Buffer can't be more than 100% full
    {
        warn = 100;
    }

    t.Z         = 0;
    t.dot       = 0;

    eb.size     = minsize;
    t.size      = eb.size;
    eb.minsize  = minsize;
    eb.maxsize  = maxsize;
    eb.stepsize = stepsize;
    eb.lowsize  = minsize - ((minsize * warn) / 100);
    eb.buf      = alloc_mem((uint)eb.size);
    eb.warn     = warn;
    eb.left     = 0;
    eb.gap      = minsize;
    eb.right    = 0;
}


///
///  @brief    Scan backward n lines in edit buffer.
///
///  @returns  Position following line terminator.
///
////////////////////////////////////////////////////////////////////////////////

static int last_delim(int nlines)
{
    int c;
    int pos = t.dot;

    if (pos-- == 0)
    {
        return 0;
    }

    for (;;)
    {
        int i = pos;

        if (pos >= eb.left)
        {
            i += eb.gap;
        }

        c = eb.buf[i];

        if (isdelim(c) && nlines-- == 0)
        {
            return ++pos;
        }

        if (pos > 0)
        {
            --pos;
        }
        else
        {
            break;
        }
    }

    // There aren't n lines preceding the current position, so just back up to
    // the beginning of the buffer.

    return 0;
}


///
///  @brief    Scan forward nlines in edit buffer.
///
///  @returns  Position including line terminator.
///
////////////////////////////////////////////////////////////////////////////////

static int next_delim(int nlines)
{
    for (int pos = t.dot; pos < eb.left + eb.right; ++pos)
    {
        int i = pos;

        if (pos >= eb.left)
        {
            i += eb.gap;
        }

        int c = eb.buf[i];

        if (isdelim(c) && --nlines == 0)
        {
            return ++pos;
        }
    }

    return eb.left + eb.right;
}


///
///  @brief    Print buffer size.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void print_size(int size)
{
    if (f.et.abort)                     // Is abort bit set?
    {
        return;                         // Yes, don't print messages then
    }

    const char *type = "";

    if (size > 1024)
    {
        if ((size /= 1024) >= 1024)
        {
            size /= 1024;
            type = "M";
        }
        else
        {
            type = "K";
        }
    }

    print_str("[%d%s bytes]", size, type);
    print_chr(CRLF);
}


///
///  @brief    Replace ASCII value of nth character before or after dot.
///
///  @returns  Original character, or -1 if character outside of edit buffer.
///
////////////////////////////////////////////////////////////////////////////////

int putchar_ebuf(int n, int c)
{
    int pos = t.dot + n;

    if (pos < eb.left + eb.right)
    {
        int i = pos;

        if (pos >= eb.left)
        {
            i += eb.gap;
        }

        int orig = eb.buf[i];

        eb.buf[i] = (char)c;

        ebuf_changed = true;

        return orig;
    }

    return -1;
}


///
///  @brief    Set buffer position.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void setpos_ebuf(int n)
{
    if (n <= eb.left + eb.right)
    {
        t.dot = n;

        ebuf_changed = true;
    }
}


///
///  @brief    Set memory size.
///
///  @returns  New memory size (may be unchanged).
///
////////////////////////////////////////////////////////////////////////////////

int setsize_ebuf(int n)
{
    n *= 1024;                          // Make it K bytes

    int minsize = ((eb.left + eb.right) * 110) / 100;

    if (n < minsize)
    {
        if ((n = minsize) == eb.size)
        {
            return eb.size;
        }
    }
    else if (n > eb.maxsize)
    {
        if ((n = eb.maxsize) == eb.size)
        {
            return eb.size;
        }
    }

    shift_left(eb.right);               // Remove the gap

    if (n < eb.size)
    {
        eb.buf = shrink_mem(eb.buf, (uint)eb.size, (uint)n);
    }
    else
    {
        eb.buf = expand_mem(eb.buf, (uint)eb.size, (uint)n);
    }

    shift_right(eb.right);              // Restore the gap

    t.dot  = 0;
    t.size = n;

    eb.size    = n;
    eb.lowsize = eb.size - ((eb.size * eb.warn) / 100);
    eb.gap     = eb.size - (eb.left + eb.right);

    print_size(eb.size);

    return eb.size;
}


///
///  @brief    Move characters from right side of gap to left side.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void shift_left(int nbytes)
{
    char *src   = eb.buf + eb.size - eb.right;
    char *dst   = eb.buf + eb.left;

    eb.left  += nbytes;
    eb.right -= nbytes;

    memmove(dst, src, (size_t)(uint)nbytes);
}


///
///  @brief    Move characters from left side of gap to right side.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void shift_right(int nbytes)
{
    eb.left  -= nbytes;
    eb.right += nbytes;

    char *src   = eb.buf + eb.left;
    char *dst   = eb.buf + eb.size - eb.right;

    memmove(dst, src, (size_t)(uint)nbytes);
}
