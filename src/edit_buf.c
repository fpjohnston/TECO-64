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


///  @var     e
///
///  @brief   Define edit buffer

static struct
{
    char *buf;                  ///< Start of buffer
    uint start;                 ///< Starting size, in bytes
    uint size;                  ///< Current size, in bytes
    uint plus;                  ///< Increment size, in bytes
    uint warn;                  ///< Warning threshold (0-100%)
    uint low;                   ///< Low water mark for gap
    bool shrink;                ///< true = shrink buffer when cleared
    uint dot;                   ///< Current position in buffer
    uint B;                     ///< First position in buffer
    uint Z;                     ///< Last position in buffer
    uint left;                  ///< No. of bytes before gap
    uint gap;                   ///< No. of bytes in gap
    uint right;                 ///< No. of bytes after gap
} e =
{    
    .buf    = NULL,
    .start  = 0,
    .size   = 0,
    .plus   = 0,
    .warn   = 100,
    .shrink = false,
    .dot    = 0,
    .B      = 0,
    .Z      = 0,
    .left   = 0,
    .gap    = 0,
    .right  = 0,
};


// Local functions

static void expand_edit(void);

static void free_edit(void);

static uint last_delim(uint nlines);

static uint next_delim(uint nlines);

static void print_size(uint size);


///
///  @brief    Add character to edit buffer.
///
///  @returns  EDIT_OK, EDIT_WARN, or EDIT_FULL.
///
////////////////////////////////////////////////////////////////////////////////

uint add_edit(int c)
{
    assert(e.buf != NULL);

    if (e.gap == 0)
    {
        return EDIT_FULL;
    }

    e.buf[e.left++] = (char)c;
    ++e.Z;

    --e.gap;

    if (e.gap <= e.low)                 // Below low water mark?
    {
        expand_edit();                  // Try to make buffer bigger

        if (e.gap == 0)                 // Buffer full?
        {
            return EDIT_FULL;
        }
        else if (e.gap <= e.low)        // Not full, but getting there?
        {
            return EDIT_WARN;
        }
    }

    return EDIT_OK;
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

    if (pos >= e.B && pos < e.Z)
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
///  @brief    See if edit buffer is empty.
///
///  @returns  true if buffer is empty, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool empty_edit(void)
{
    if (e.buf == NULL || e.gap == e.size)
    {
        return true;
    }
    else
    {
        return false;
    }
}


///
///  @brief    Expand edit buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void expand_edit(void)
{
    if (e.plus == 0)
    {
        return;
    }

    char *newbuf = expand_mem(e.buf, e.size, e.size + e.plus);

    e.buf   = newbuf;
    e.size += e.plus;
    e.low   = e.size - ((e.size * e.warn) / 100);
    e.gap  += e.plus;

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

    e.start  = 0;
    e.size   = 0;
    e.plus   = 0;
    e.warn   = 0;
    e.low    = 0;
    e.shrink = false;
    e.dot    = 0;
    e.B      = 0;
    e.Z      = 0;
    e.left   = 0;
    e.gap    = 0;
    e.right  = 0;
}


///
///  @brief    Get first position in buffer.
///
///  @returns  First position in buffer (a.k.a. B).
///
////////////////////////////////////////////////////////////////////////////////

uint get_B(void)
{
    return e.B;
}


///
///  @brief    Get current position in buffer.
///
///  @returns  Current position in buffer (a.k.a. dot).
///
////////////////////////////////////////////////////////////////////////////////

uint get_dot(void)
{
    return e.dot;
}


///
///  @brief    Get last position in buffer.
///
///  @returns  Last position in buffer (a.k.a. Z).
///
////////////////////////////////////////////////////////////////////////////////

uint get_Z(void)
{
    return e.Z;
}


///
///  @brief    Initialize edit buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_edit(
    uint start,                         ///< Initial size of buffer
    uint plus,                          ///< Size to increase buffer (or 0)
    uint warn,                          ///< Warning threshold (0-100)
    bool shrink)                        ///< true = reset buffer when cleared
{
    assert(e.buf == NULL);

    e.buf    = alloc_mem(start);
    e.start  = start;
    e.size   = start;
    e.plus   = plus;
    e.warn   = warn;
    e.low    = start - ((start * warn) / 100);
    e.shrink = shrink;
    e.dot    = 0;
    e.B      = 0;
    e.Z      = 0;
    e.left   = 0;
    e.gap    = start;
    e.right  = 0;

    if (atexit(free_edit) != 0)         // Ensure we clean up on exit
    {
        exit(EXIT_FAILURE);
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
    if (n < e.B || n > e.Z)
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
    if (e.shrink && e.size > e.start)   // Can we shrink the buffer?
    {
        e.buf  = shrink_mem(e.buf, e.size, e.start);
        e.size = e.start;
        e.low  = e.size - ((e.size * e.warn) / 100);

        print_size(e.size);
    }

    e.dot    = 0;
    e.left   = 0;
    e.gap    = e.size;
    e.right  = 0;
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

        if (pos > e.B)
        {
            --pos;
        }
        else
        {
            break;
        }
    }

    return e.B;
}


///
///  @brief    Move n characters forward or backward in buffer.
///
///  @returns  true if success, false if attempt to move pointer off page.
///
////////////////////////////////////////////////////////////////////////////////

bool move_edit(int n)
{
    if (n == 0)
    {
        return true;
    }

    if (n < 0)
    {
        while (n++ < 0)
        {
            if (e.dot == e.B)
            {
                return false;
            }

            --e.dot;
        }
    }
    else
    {
        while (n-- > 0)
        {
            if (e.dot == e.Z)
            {
                return false;
            }

            ++e.dot;
        }
    }

    return true;
}


///
///  @brief    Scan forward nlines in edit buffer.
///
///  @returns  Position including line terminator.
///
////////////////////////////////////////////////////////////////////////////////

static uint next_delim(uint nlines)
{
    for (uint pos = e.dot; pos < e.Z; ++pos)
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

    return e.Z;
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

void print_edit(int m, int n)
{
    assert(m != -1 || n != -1);

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

    assert(x >= e.B && y <= e.Z);
    
    (void)type_edit(x, y);
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
///  @brief    Step forward or backward n lines.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void step_edit(int n)
{
    if (n < 0)
    {
        e.dot = last_delim((uint)-n);
    }
    else
    {
        e.dot = next_delim((uint)n);
    }
}


///
///  @brief    Type edit buffer characters. Called by T command:
///
///            m,nT -> Type buffer between positions m and n.
///
///  @returns  true if success, false if attempt to move pointer off page.
///
////////////////////////////////////////////////////////////////////////////////

bool type_edit(uint m, uint n)
{
    if (m > e.Z || n > e.Z)             // Both positions within buffer?
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

        int c = e.buf[i];               // Get character from buffer

        if (c == LF)
        {
            echo_chr(CR);               // TODO: use callback?
        }

        echo_chr(c);                    // TODO: use callback?
    }

    return true;
}
