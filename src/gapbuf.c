///
///  @file    gapbuf.c
///  @brief   Edit buffer-handling functions.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
///
///  Permission is hereby granted, free of charge, to any person obtaining a copy
///  of this software and associated documentation files (the "Software"), to deal
///  in the Software without restriction, including without limitation the rights
///  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
///  copies of the Software, and to permit persons to whom the Software is
///  furnished to do so, subject to the following conditions:
///
///  The above copyright notice and this permission notice shall be included in
///  all copies or substantial portions of the Software.
///
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
///  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
///
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "textbuf.h"


///  @var     e
///
///  @brief   Define edit buffer

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
} e =
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
    assert(e.buf != NULL);

    if (e.gap == 0)
    {
        return EDIT_ERROR;              // Buffer is already full
    }

    if (e.dot < e.left)
    {
        shift_right(e.left - e.dot);
    }
    else if (e.dot > e.left)
    {
        shift_left(e.dot - e.left);
    }

    e.buf[e.left++] = (char)c;

    ++e.dot;
    --e.gap;

    if (e.gap <= e.lowsize)             // Below low water mark?
    {
        if (!expand_tbuf())             // Try to make buffer bigger
        {
            if (e.gap == 0)
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
    if (e.dot == 0 && (uint)n == e.left + e.right)
    {
        e.left = e.right = 0;
    }
    else
    {
        if (e.dot < e.left)
        {
            shift_right(e.left - e.dot);
        }
        else if (e.dot > e.left)
        {
            shift_left(e.dot - e.left);
        }

        if (n < 0)
        {
            n = -n;

            if ((uint)n > e.left)
            {
                return;
            }

            e.left -= (uint)n;
            e.dot -= (uint)n;
        }
        else if (n > 0)
        {
            if ((uint)n > e.right)
            {
                return;
            }

            e.right -= (uint)n;
        }
    }

    e.gap += (uint)n;
}


///
///  @brief    Expand edit buffer.
///
///  @returns  true if able to expand, else false.
///
////////////////////////////////////////////////////////////////////////////////

static bool expand_tbuf(void)
{
    if (e.stepsize == 0 || e.size >= e.maxsize)
    {
        return false;
    }

    char *newbuf = expand_mem(e.buf, e.size, e.size + e.stepsize);

    e.buf   = newbuf;
    e.size += e.stepsize;

    if (e.size > e.maxsize)
    {
        e.size = e.maxsize;
    }

    e.lowsize = e.size - ((e.size * e.warn) / 100);
    e.gap    += e.stepsize;

    print_size(e.size);

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
    free_mem(&e.buf);

    e.size     = 0;
    e.minsize  = 0;
    e.maxsize  = 0;
    e.stepsize = 0;
    e.lowsize  = 0;
    e.warn     = 0;
    e.dot      = 0;
    e.left     = 0;
    e.gap      = 0;
    e.right    = 0;
}


///
///  @brief    Get ASCII value of nth character before or after dot.
///
///  @returns  ASCII value, or -1 if character outside of edit buffer.
///
////////////////////////////////////////////////////////////////////////////////

int getchar_tbuf(int n)
{
    uint pos = (uint)((int)e.dot + n);

    if (pos < e.left + e.right)
    {
        uint i = pos;

        if (pos >= e.left)
        {
            i += e.gap;
        }

        return e.buf[i];
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
        return (int)(next_delim((uint)n) - e.dot);
    }
    else
    {
        return (int)(last_delim((uint)-n) - e.dot);
    }

}


///
///  @brief    Get current position in buffer.
///
///  @returns  Current position in buffer (a.k.a. dot).
///
////////////////////////////////////////////////////////////////////////////////

uint getpos_tbuf(void)
{
    return e.dot;
}


///
///  @brief    Get last position in buffer.
///
///  @returns  Last position in buffer (a.k.a. Z).
///
////////////////////////////////////////////////////////////////////////////////

uint getsize_tbuf(void)
{
    return e.left + e.right;
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
    assert(e.buf == NULL);

    if (warn > 100)                     // Buffer can't be more than 100% full
    {
        warn = 100;
    }

    e.buf      = alloc_mem(minsize);
    e.size     = minsize;
    e.minsize  = minsize;
    e.maxsize  = maxsize;
    e.stepsize = stepsize;
    e.lowsize  = minsize - ((minsize * warn) / 100);
    e.warn     = warn;
    e.dot      = 0;
    e.left     = 0;
    e.gap      = minsize;
    e.right    = 0;

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
    uint pos = e.dot;

    for (;;)
    {
        uint i = pos;
        
        if (pos >= e.left)
        {
            i += e.gap;
        }

        c = e.buf[i];

        if (c == LF || c == VT || c == FF)
        {
            if (nlines-- == 0)
            {
                return ++pos;
            }
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
    for (uint pos = e.dot; pos < e.left + e.right; ++pos)
    {
        uint i = pos;
        
        if (pos >= e.left)
        {
            i += e.gap;
        }

        int c = e.buf[i];

        if (c == LF || c == VT || c == FF)
        {
            if (--nlines == 0)
            {
                return ++pos;
            }
        }
    }

    return e.left + e.right;
}


///
///  @brief    Print buffer size.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void print_size(uint size)
{
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
///  @brief    Set buffer position.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void setpos_tbuf(uint n)
{
    if (n <= e.left + e.right)
    {
        e.dot = n;
    }
}


///
///  @brief    Move characters from right side of gap to left side.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void shift_left(uint nbytes)
{
    char *src   = e.buf + e.size - e.right;
    char *dst   = e.buf + e.left;

    e.left  += nbytes;
    e.right -= nbytes;

    memmove(dst, src, (size_t)nbytes);
    memset(src, '{', (size_t)nbytes);   // TODO: temporary
}


///
///  @brief    Move characters from left side of gap to right side.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void shift_right(uint nbytes)
{
    e.left  -= nbytes;
    e.right += nbytes;

    char *src   = e.buf + e.left;
    char *dst   = e.buf + e.size - e.right;

    memmove(dst, src, (size_t)nbytes);
    memset(src, '}', (size_t)nbytes);   // TODO: temporary
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

    if (n < e.minsize || n < e.left + e.right || n == e.size)
    {
        return e.size;
    }

    if (n > e.maxsize)
    {
        n = e.maxsize;
    }

    if (n < e.size)
    {
        e.buf = shrink_mem(e.buf, e.size, n);
    }
    else
    {
        e.buf = expand_mem(e.buf, e.size, n);
    }

    e.size    = n;
    e.lowsize = e.size - ((e.size * e.warn) / 100);
    e.dot     = 0;
    e.left    = 0;
    e.gap     = e.size;
    e.right   = 0;

    print_size(e.size);

    return e.size;
}
