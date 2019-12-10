///
///  @file    edit_buf.c
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
#include "edit_buf.h"


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

static void expand_edit(void);

static void free_edit(void);

static uint last_delim(uint nlines);

static uint next_delim(uint nlines);

static void print_size(uint size);

static void shift_left(uint nbytes);

static void shift_right(uint nbytes);

void bug(int n);

void bug(int n)
{
    if (n > 0)
        shift_right((uint)n);
    else if (n < 0)
        shift_left((uint)-n);
    else
    {
        printf("[bug check]\r\n");
        (void)fflush(stdout);
    }
}


///
///  @brief    Add character to edit buffer.
///
///  @returns  EDIT_OK, EDIT_WARN, EDIT_FULL, or EDIT_ERROR.
///
////////////////////////////////////////////////////////////////////////////////

estatus add_edit(int c)
{
    assert(e.buf != NULL);

    if (e.gap == 0)
    {
        return EDIT_ERROR;              // Buffer is already full
    }

    e.buf[e.left++] = (char)c;

    --e.gap;

    if (e.gap <= e.lowsize)             // Below low water mark?
    {
        expand_edit();                  // Try to make buffer bigger

        if (e.gap == 0)
        {
            return EDIT_FULL;           // Buffer is now full
        }
        else if (e.gap <= e.lowsize)
        {
            return EDIT_WARN;           // Buffer is getting full
        }
    }

    return EDIT_OK;                     // Insertion was successful
}


///
///  @brief    Get ASCII value of nth character before or after dot.
///
///  @returns  ASCII value, or -1 if character outside of edit buffer.
///
////////////////////////////////////////////////////////////////////////////////

int char_edit(int n)
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
///  @brief    Delete n chars at current position.
///
///  @returns  true if success, false if delete was too big.
///
////////////////////////////////////////////////////////////////////////////////

bool delete_edit(int n)
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
        if ((uint)n > e.left)
        {
            return false;
        }

        e.left -= (uint)n;
    }
    else if (n > 0)
    {
        if ((uint)n > e.right)
        {
            return false;
        }

        e.right -= (uint)n;
    }

    e.gap += (uint)n;

    return true;
}


///
///  @brief    Expand edit buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void expand_edit(void)
{
    if (e.stepsize == 0 || e.size >= e.maxsize)
    {
        return;
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
}


///
///  @brief    Free up memory for edit buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void free_edit(void)
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
///  @brief    Initialize edit buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_edit(
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

    if (atexit(free_edit) != 0)         // Ensure we clean up on exit
    {
        exit(EXIT_FAILURE);
    }

}


///
///  @brief    Insert string at dot.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void insert_edit(const char *buf, uint len)
{
    assert(buf != NULL);

    //
    //  The physical buffer looks like this:
    //
    //  [left][gap][right]
    //
    //  and our dot can be anywhere in the left or right halves. What we need
    //  to do before inserting is to ensure that the current position (dot) is
    //  at the end of the left half. This makes insertion easy, because we're
    //  just reducing the gap, and don't have to move data in the right half.
    //

    if (e.dot < e.left)
    {
        shift_right(e.left - e.dot);
    }
    else if (e.dot > e.left)
    {
        shift_left(e.dot - e.left);
    }

    for (uint i = 0; i < len; ++i)
    {
        int c = *buf++;

        if (c == CR)
        {
            continue;
        }
        
        switch (add_edit(c))
        {
            case EDIT_FULL:
                ++e.dot;
                //lint -fallthrough

            case EDIT_ERROR:
                return;

            default:
                ++e.dot;
                break;
        }                
    }
}


///
///  @brief    Jump to position in buffer.
///
///  @returns  true if success, false if attempt to move pointer off page.
///
////////////////////////////////////////////////////////////////////////////////

bool jump_edit(uint n)
{
    if (n > e.left + e.right)
    {
        return false;
    }

    e.dot = n;

    return true;
}


///
///  @brief    Kill entire edit buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void kill_edit(void)
{
    e.dot   = 0;
    e.left  = 0;
    e.gap   = e.size;
    e.right = 0;
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
///  @brief    Move n characters forward or backward in buffer.
///
///  @returns  true if success, false if attempt to move pointer off page.
///
////////////////////////////////////////////////////////////////////////////////

bool move_edit(int n, bool error)
{
    if (n > 0)
    {
        if ((uint)n > e.left + e.right + e.dot)
        {
            if (error)
            {
                e.dot = e.left + e.right;
            }

            return false;
        }

        e.dot += (uint)n;
    }
    else if (n < 0)
    {
        if ((uint)-n > e.dot)
        {
            if (error)
            {
                e.dot = 0;
            }

            return false;
        }

        e.dot -= (uint)-n;
    }

    return true;
}


///
///  @brief    Return number of characters between dot and nth line terminator.
///
///  @returns  Number of characters relative to dot (can be plus or minus).
///
////////////////////////////////////////////////////////////////////////////////

int nchars_edit(int n)
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
///  @brief    Get current position in buffer.
///
///  @returns  Current position in buffer (a.k.a. dot).
///
////////////////////////////////////////////////////////////////////////////////

uint pos_edit(void)
{
    return e.dot;
}


///
///  @brief    Print edit buffer lines. Called by T and V commands:
///
///            T    -> current position through line terminator (same as 1T)
///            0T   -> past last line terminator to current position
///            +nT  -> current position through next n line terminators
///            V    -> equivalent to 0T 1T
///            nV   -> equivalent to 1-nT nT
///            m,nV -> equivalent to -(m - 1)T n-1T
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void print_edit(int m, int n, void (*print_cb)(int c))
{
    assert(m != -1 || n != -1);
    assert(print_cb != NULL);

    uint x, y;

    if (m == -1)                        // Print previous line(s)?
    {
        x = e.dot;                      // No, just use dot
    }
    else
    {
        x = last_delim((uint)m);
    }

    if (n == -1)                        // Print following line(s)?
    {
        y = e.dot;                      // No, just use dot
    }
    else
    {
        y = next_delim((uint)n);
    }

    assert(y <= e.left + e.right);
    
    (void)type_edit(x, y, print_cb);
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

    printf("[%u%s bytes]", size, type);

    (void)fflush(stdout);

    putc_term(CRLF);
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

void set_edit(uint n)
{
    n *= 1024;                          // Make it K bytes

    if (n < e.minsize || n < e.left + e.right || n == e.size)
    {
        return;
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
}


///
///  @brief    Get last position in buffer.
///
///  @returns  Last position in buffer (a.k.a. Z).
///
////////////////////////////////////////////////////////////////////////////////

uint size_edit(void)
{
    return e.left + e.right;
}


///
///  @brief    Type edit buffer characters. Called by T command:
///
///            m,nT -> Type buffer between positions m and n.
///
///  @returns  true if success, false if attempt to move pointer off page.
///
////////////////////////////////////////////////////////////////////////////////

bool type_edit(uint m, uint n, void (*print_cb)(int c))
{
    assert(print_cb != NULL);

    uint z = e.left + e.right;

    if (m > z || n > z)                 // Both positions within buffer?
    {
        return false;                   // Nope
    }

    if (m > n)
    {
        return true;                    // TODO: print error instead?
    }

    for (uint pos = m; pos < n; ++pos)
    {
        uint i = pos;                   // Index into buffer
        
        if (pos >= e.left)              // Left or right of gap?
        {
            i += e.gap;                 // Right, so add bias
        }

        (*print_cb)(e.buf[i]);          // Print character
    }

    return true;
}


///
///  @brief    Write out edit buffer. Called by P command.
///
///            Since we have a gap buffer, there are three possible cases
///            regarding the data to be output:
///
///            1. It is entirely to the left of the gap.
///            2. It is entirely to the right of the gap.
///            3. It is in both the left and right parts.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void write_edit(uint m, uint n, void (*write_cb)(const char *buf, uint len))
{
    assert(write_cb != NULL);

    uint z = e.left + e.right;

    if (m > z || n > z)                 // Both positions within buffer?
    {
        return;                         // Nope
    }

    if (m > n)
    {
        return;                         // TODO: is this an error?
    }

    if (n < e.left)                     // Just output left side?
    {
        (*write_cb)(e.buf + m, n - m);
    }
    else if (m >= e.left)               // Just output right side?
    {
        (*write_cb)(e.buf + e.gap + m, n - m);
    }
    else                                // Output from both sides
    {
        (*write_cb)(e.buf + m, e.left);
        (*write_cb)(e.buf + e.left + e.gap, n - e.left);
    }
}
