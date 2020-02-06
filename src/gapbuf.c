///
///  @file    gapbuf.c
///  @brief   Text buffer functions.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
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
#include "eflags.h"
#include "textbuf.h"


///  @var    t
///
///  @brief  Text data buffer (external)

struct text t =
{
    .B        = 0,
    .Z        = 0,
    .dot      = 0,
};

///  @var     tb
///
///  @brief   Text buffer data (internal)

static struct
{
    char *buf;                  ///< Start of buffer
    int size;                   ///< Current size of buffer, in bytes
    int minsize;                ///< Initial and minimum size, in bytes
    int maxsize;                ///< Maximum size, in bytes
    int stepsize;               ///< Increment size, in bytes
    int lowsize;                ///< Low water mark for gap
    int warn;                   ///< Warning threshold (0-100%)
    int left;                   ///< No. of bytes before gap
    int gap;                    ///< No. of bytes in gap
    int right;                  ///< No. of bytes after gap
} tb =
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

bool tbuf_changed;              ///< true if text buffer modified

// Local functions

static bool expand_tbuf(void);

static void free_tbuf(void);

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

int add_tbuf(int c)
{
    assert(tb.buf != NULL);

    if (tb.gap == 0)
    {
        return EDIT_ERROR;              // Buffer is already full
    }

    if (t.dot < tb.left)
    {
        shift_right(tb.left - t.dot);
    }
    else if (t.dot > tb.left)
    {
        shift_left(t.dot - tb.left);
    }

    tb.buf[tb.left++] = (char)c;

    tbuf_changed = true;

    ++t.dot;
    ++t.Z;
    --tb.gap;

    if (tb.gap <= tb.lowsize)           // Below low water mark?
    {
        if (!expand_tbuf())             // Try to make buffer bigger
        {
            if (tb.gap == 0)
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

void delete_tbuf(int n)
{
    if (t.dot == 0 && n == tb.left + tb.right)
    {
        tb.left = tb.right = 0;
    }
    else
    {
        if (t.dot < tb.left)
        {
            shift_right(tb.left - t.dot);
        }
        else if (t.dot > tb.left)
        {
            shift_left(t.dot - tb.left);
        }

        if (n < 0)
        {
            n = -n;

            if (n > tb.left)
            {
                return;
            }

            tb.left -= n;
            t.dot -= n;
        }
        else if (n > 0)
        {
            if (n > tb.right)
            {
                return;
            }

            tb.right -= n;
        }
    }

    tb.gap += n;

    t.Z = tb.left + tb.right;

    tbuf_changed = true;
}


///
///  @brief    Expand edit buffer.
///
///  @returns  true if able to expand, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool expand_tbuf(void)
{
    if (tb.stepsize == 0 || tb.size >= tb.maxsize)
    {
        return false;
    }

    // Buffer: [left][gap][right]

    shift_left(tb.right);

    // Buffer: [left + right][gap]

    tb.buf   = expand_mem(tb.buf, (uint)tb.size, (uint)(tb.size + tb.stepsize));
    tb.size += tb.stepsize;

    if (tb.size > tb.maxsize)
    {
        tb.size = tb.maxsize;
    }

    tb.lowsize = tb.size - ((tb.size * tb.warn) / 100);

    shift_right(tb.right);

    // Buffer: [left][gap][right]

    tb.gap += tb.stepsize;

    print_size(tb.size);

    return true;
}


///
///  @brief    Free up memory for edit buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void free_tbuf(void)
{
    free_mem(&tb.buf);

    t.B         = 0;
    t.Z         = 0;
    t.dot       = 0;

    tb.size     = 0;
    tb.minsize  = 0;
    tb.maxsize  = 0;
    tb.stepsize = 0;
    tb.lowsize  = 0;
    tb.warn     = 0;
    tb.left     = 0;
    tb.gap      = 0;
    tb.right    = 0;
}


///
///  @brief    Get ASCII value of nth character before or after dot.
///
///  @returns  ASCII value, or -1 if character outside of edit buffer.
///
////////////////////////////////////////////////////////////////////////////////

int getchar_tbuf(int n)
{
    int pos = t.dot + n;

    if (pos < tb.left + tb.right)
    {
        if (pos >= tb.left)
        {
            pos += tb.gap;
        }

        return tb.buf[pos];
    }

    return -1;
}


///
///  @brief    Return number of characters between dot and nth line terminator.
///
///  @returns  Number of characters relative to dot (can be plus or minus).
///
////////////////////////////////////////////////////////////////////////////////

int getdelta_tbuf(int n)
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

int getlines_tbuf(int n)
{
    int start = (n > 0) ? 0 : -t.dot;
    int end   = (n < 0) ? 0 :  tb.left + tb.right;
    int nlines = 0;

    for (int pos = start; pos < end; ++pos)
    {
        int c = getchar_tbuf(pos);

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

void init_tbuf(
    int minsize,                        ///< Initial and min. size of buffer
    int maxsize,                        ///< Maximum size of buffer, in bytes
    int stepsize,                       ///< Incremental increase, in bytes
    int warn)                           ///< Warning threshold (0-100)
{
    assert(tb.buf == NULL);

    if (warn > 100)                     // Buffer can't be more than 100% full
    {
        warn = 100;
    }

    t.Z         = 0;
    t.dot       = 0;

    tb.size     = minsize;
    tb.minsize  = minsize;
    tb.maxsize  = maxsize;
    tb.stepsize = stepsize;
    tb.lowsize  = minsize - ((minsize * warn) / 100);
    tb.buf      = alloc_mem((uint)tb.size);
    tb.warn     = warn;
    tb.left     = 0;
    tb.gap      = minsize;
    tb.right    = 0;

    if (atexit(free_tbuf) != 0)         // Ensure we clean up on exit
    {
        exit(EXIT_FAILURE);
    }

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

        if (pos >= tb.left)
        {
            i += tb.gap;
        }

        c = tb.buf[i];

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
    for (int pos = t.dot; pos < tb.left + tb.right; ++pos)
    {
        int i = pos;

        if (pos >= tb.left)
        {
            i += tb.gap;
        }

        int c = tb.buf[i];

        if (isdelim(c) && --nlines == 0)
        {
            return ++pos;
        }
    }

    return tb.left + tb.right;
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
        if ((size /= 1024) > 1024)
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

int putchar_tbuf(int n, int c)
{
    int pos = t.dot + n;

    if (pos < tb.left + tb.right)
    {
        int i = pos;

        if (pos >= tb.left)
        {
            i += tb.gap;
        }

        int orig = tb.buf[i];

        tb.buf[i] = (char)c;

        tbuf_changed = true;

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

void setpos_tbuf(int n)
{
    if (n <= tb.left + tb.right)
    {
        t.dot = n;

        tbuf_changed = true;
    }
}


///
///  @brief    Set memory size.
///
///  @returns  New memory size (may be unchanged).
///
////////////////////////////////////////////////////////////////////////////////

int setsize_tbuf(int n)
{
    n *= 1024;                          // Make it K bytes

    if (n < tb.minsize || n < tb.left + tb.right || n == tb.size)
    {
        return tb.size;
    }

    if (n > tb.maxsize)
    {
        n = tb.maxsize;
    }

    if (n < tb.size)
    {
        tb.buf = shrink_mem(tb.buf, (uint)tb.size, (uint)n);
    }
    else
    {
        tb.buf = expand_mem(tb.buf, (uint)tb.size, (uint)n);
    }

    t.dot     = 0;

    // TODO: is this correct?

    tb.size    = n;
    tb.lowsize = tb.size - ((tb.size * tb.warn) / 100);
    tb.left    = 0;
    tb.gap     = tb.size;
    tb.right   = 0;

    print_size(tb.size);

    return tb.size;
}


///
///  @brief    Move characters from right side of gap to left side.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void shift_left(int nbytes)
{
    char *src   = tb.buf + tb.size - tb.right;
    char *dst   = tb.buf + tb.left;

    tb.left  += nbytes;
    tb.right -= nbytes;

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
    tb.left  -= nbytes;
    tb.right += nbytes;

    char *src   = tb.buf + tb.left;
    char *dst   = tb.buf + tb.size - tb.right;

    memmove(dst, src, (size_t)(uint)nbytes);
}
