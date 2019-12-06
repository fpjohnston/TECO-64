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
    .left   = 0,
    .gap    = 0,
    .right  = 0,
};


// Local functions

static void expand_edit(void);

static void free_edit(void);

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
    e.dot = e.left;

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
    e.left   = 0;
    e.gap    = 0;
    e.right  = 0;
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
    e.left   = 0;
    e.gap    = start;
    e.right  = 0;

    if (atexit(free_edit) != 0)         // Ensure we clean up on exit
    {
        exit(EXIT_FAILURE);
    }

}


///
///  @brief    Kill entire edit buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void kill_edit(void)
{
    if (e.shrink)                       // Can we shrink the buffer?
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
///  @brief    Print buffer size.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void print_size(uint size)
{
    if (size > 1024 * 1024)
    {
        printf("[%uM Bytes]\r\n", size / (1024 * 1024));
    }
    else if (size > 1024)
    {
        printf("[%uK Bytes]\r\n", size / 1024);
    }
    else
    {
        printf("[%u Bytes]\r\n", size);
    }

    (void)fflush(stdout);
}


///
///  @brief    Type edit buffer line.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void type_edit(void)
{
    const char *p = e.buf;

    for (uint i = 0; i < e.left; ++i)
    {
        int c = *p++;

        if (c == LF)
        {
            echo_chr(CR);
        }

        echo_chr(c);
    }
}

