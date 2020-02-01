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


///  @var     t
///
///  @brief   Define text buffer

static struct
{
    char *buf;                  ///< Start of buffer
    uint size;                  ///< Current size, in bytes
    uint minsize;               ///< Initial and minimum size, in bytes
    uint maxsize;               ///< Maximum size, in bytes
    uint stepsize;              ///< Increment size, in bytes
    uint lowsize;               ///< Low water mark for gap
    uint warn;                  ///< Warning threshold (0-100%)
    uint dot;                   ///< Current position in buffer
    uint left;                  ///< No. of bytes before gap
    uint gap;                   ///< No. of bytes in gap
    uint right;                 ///< No. of bytes after gap
} t =
{
    .buf      = NULL,
    .size     = 0,
    .minsize  = 0,
    .maxsize  = 0,
    .stepsize = 0,
    .lowsize  = 0,
    .warn     = 100,
    .dot      = 0,
    .left     = 0,
    .gap      = 0,
    .right    = 0,
};


// Local functions

static bool expand_tbuf(void);

static void free_tbuf(void);

static uint last_delim(uint nlines);

static uint next_delim(uint nlines);

static void print_size(uint size);

static void shift_left(uint nbytes);

static void shift_right(uint nbytes);


///
///  @brief    Add character to edit buffer.
///
///  @returns  EDIT_OK, EDIT_WARN, EDIT_FULL, or EDIT_ERROR.
///
////////////////////////////////////////////////////////////////////////////////

int add_tbuf(int c)
{
    assert(t.buf != NULL);

    if (t.gap == 0)
    {
        return EDIT_ERROR;              // Buffer is already full
    }

    if (t.dot < t.left)
    {
        shift_right(t.left - t.dot);
    }
    else if (t.dot > t.left)
    {
        shift_left(t.dot - t.left);
    }

    t.buf[t.left++] = (char)c;

    ++t.dot;
    --t.gap;

    if (t.gap <= t.lowsize)             // Below low water mark?
    {
        if (!expand_tbuf())             // Try to make buffer bigger
        {
            if (t.gap == 0)
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
    if (t.dot == 0 && (uint)n == t.left + t.right)
    {
        t.left = t.right = 0;
    }
    else
    {
        if (t.dot < t.left)
        {
            shift_right(t.left - t.dot);
        }
        else if (t.dot > t.left)
        {
            shift_left(t.dot - t.left);
        }

        if (n < 0)
        {
            n = -n;

            if ((uint)n > t.left)
            {
                return;
            }

            t.left -= (uint)n;
            t.dot -= (uint)n;
        }
        else if (n > 0)
        {
            if ((uint)n > t.right)
            {
                return;
            }

            t.right -= (uint)n;
        }
    }

    t.gap += (uint)n;
}


///
///  @brief    Expand edit buffer.
///
///  @returns  true if able to expand, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool expand_tbuf(void)
{
    if (t.stepsize == 0 || t.size >= t.maxsize)
    {
        return false;
    }

    // Buffer: [left][gap][right]

    shift_left(t.right);

    // Buffer: [left + right][gap]

    t.buf   = expand_mem(t.buf, t.size, t.size + t.stepsize);
    t.size += t.stepsize;

    if (t.size > t.maxsize)
    {
        t.size = t.maxsize;
    }

    t.lowsize = t.size - ((t.size * t.warn) / 100);

    shift_right(t.right);

    // Buffer: [left][gap][right]

    t.gap += t.stepsize;

    print_size(t.size);

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
    free_mem(&t.buf);

    t.size     = 0;
    t.minsize  = 0;
    t.maxsize  = 0;
    t.stepsize = 0;
    t.lowsize  = 0;
    t.warn     = 0;
    t.dot      = 0;
    t.left     = 0;
    t.gap      = 0;
    t.right    = 0;
}


///
///  @brief    Get ASCII value of nth character before or after dot.
///
///  @returns  ASCII value, or -1 if character outside of edit buffer.
///
////////////////////////////////////////////////////////////////////////////////

int getchar_tbuf(int n)
{
    uint pos = (uint)((int)t.dot + n);

    if (pos < t.left + t.right)
    {
        if (pos >= t.left)
        {
            pos += t.gap;
        }

        return t.buf[pos];
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
        return (int)(next_delim((uint)n) - t.dot);
    }
    else
    {
        return (int)(last_delim((uint)-n) - t.dot);
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
    int start = (n > 0) ? 0 : -(int)t.dot;
    int end   = (n < 0) ? 0 :  (int)t.size;
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
///  @brief    Get current position in buffer.
///
///  @returns  Current position in buffer (a.k.a. dot).
///
////////////////////////////////////////////////////////////////////////////////

uint getpos_tbuf(void)
{
    return t.dot;
}


///
///  @brief    Get last position in buffer.
///
///  @returns  Last position in buffer (a.k.a. Z).
///
////////////////////////////////////////////////////////////////////////////////

uint getsize_tbuf(void)
{
    return t.left + t.right;
}


///
///  @brief    Initialize edit buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_tbuf(
    uint minsize,                       ///< Initial and min. size of buffer
    uint maxsize,                       ///< Maximum size of buffer, in bytes
    uint stepsize,                      ///< Incremental increase, in bytes
    uint warn)                          ///< Warning threshold (0-100)
{
    assert(t.buf == NULL);

    if (warn > 100)                     // Buffer can't be more than 100% full
    {
        warn = 100;
    }

    t.buf      = alloc_mem(minsize);
    t.size     = minsize;
    t.minsize  = minsize;
    t.maxsize  = maxsize;
    t.stepsize = stepsize;
    t.lowsize  = minsize - ((minsize * warn) / 100);
    t.warn     = warn;
    t.dot      = 0;
    t.left     = 0;
    t.gap      = minsize;
    t.right    = 0;

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

static uint last_delim(uint nlines)
{
    int c;
    uint pos = t.dot;

    if (pos-- == 0)
    {
        return 0;
    }

    for (;;)
    {
        uint i = pos;

        if (pos >= t.left)
        {
            i += t.gap;
        }

        c = t.buf[i];

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

    return 0;
}


///
///  @brief    Scan forward nlines in edit buffer.
///
///  @returns  Position including line terminator.
///
////////////////////////////////////////////////////////////////////////////////

static uint next_delim(uint nlines)
{
    for (uint pos = t.dot; pos < t.left + t.right; ++pos)
    {
        uint i = pos;

        if (pos >= t.left)
        {
            i += t.gap;
        }

        int c = t.buf[i];

        if (isdelim(c) && --nlines == 0)
        {
            return ++pos;
        }
    }

    return t.left + t.right;
}


///
///  @brief    Print buffer size.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void print_size(uint size)
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

    print_str("[%u%s bytes]", size, type);
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
    uint pos = (uint)((int)t.dot + n);

    if (pos < t.left + t.right)
    {
        uint i = pos;

        if (pos >= t.left)
        {
            i += t.gap;
        }

        int orig = t.buf[i];

        t.buf[i] = (char)c;

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

void setpos_tbuf(uint n)
{
    if (n <= t.left + t.right)
    {
        t.dot = n;
    }
}


///
///  @brief    Set memory size.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

uint setsize_tbuf(uint n)
{
    n *= 1024;                          // Make it K bytes

    if (n < t.minsize || n < t.left + t.right || n == t.size)
    {
        return t.size;
    }

    if (n > t.maxsize)
    {
        n = t.maxsize;
    }

    if (n < t.size)
    {
        t.buf = shrink_mem(t.buf, t.size, n);
    }
    else
    {
        t.buf = expand_mem(t.buf, t.size, n);
    }

    t.size    = n;
    t.lowsize = t.size - ((t.size * t.warn) / 100);
    t.dot     = 0;
    t.left    = 0;
    t.gap     = t.size;
    t.right   = 0;

    print_size(t.size);

    return t.size;
}


///
///  @brief    Move characters from right side of gap to left side.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void shift_left(uint nbytes)
{
    char *src   = t.buf + t.size - t.right;
    char *dst   = t.buf + t.left;

    t.left  += nbytes;
    t.right -= nbytes;

    memmove(dst, src, (size_t)nbytes);
//    memset(src, '{', (size_t)t.gap);   // TODO: temporary
}


///
///  @brief    Move characters from left side of gap to right side.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void shift_right(uint nbytes)
{
    t.left  -= nbytes;
    t.right += nbytes;

    char *src   = t.buf + t.left;
    char *dst   = t.buf + t.size - t.right;

    memmove(dst, src, (size_t)nbytes);
//    memset(src, '}', (size_t)t.gap);    // TODO: temporary
}
