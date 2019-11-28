///
///  @file    buffer.c
///  @brief   Buffer-handling functions.
///
///  @author  Nowwith Treble Software
///
///  @bug     No known bugs.
///
///  @copyright  tbd
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
#include "errors.h"

static struct buffer cmd =
{
    .buf  = NULL,
    .size = 0,
    .put  = 0,
    .get  = 0,
};

static struct buffer *curbuf;

// Local functions

static void free_buf(void);


///
///  @brief    Create copy of current buffer and return it.
///
///  @returns  String copy of buffer.
///
////////////////////////////////////////////////////////////////////////////////

struct tstr copy_buf(void)
{
    assert(curbuf != NULL);
    assert(curbuf->size != 0);

    struct tstr str = { .buf = curbuf->buf, .len = curbuf->put };
    
    return str;
}


///
///  @brief    Return number of characters in buffer.
///
///  @returns  No. of characters left to be read.
///
////////////////////////////////////////////////////////////////////////////////

uint count_buf(void)
{
    assert(curbuf != NULL);
    
    return curbuf->put - curbuf->get;
}


///
///  @brief    Delete last character from buffer and return it.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

int delete_buf(void)
{
    assert(curbuf != NULL);
    
    if (curbuf->put == 0)               // Anything in buffer?
    {
        return EOF;                     // No
    }

    return curbuf->buf[--curbuf->put];  // Delete character and return it
}


///
///  @brief    Echo all characters in buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void echo_buf(int pos)
{
    assert(curbuf != NULL);
    
    assert((uint)pos <= curbuf->put);

    // Just echo everything we're supposed to print. Note that this is not the
    // same as typing out what's in a buffer, so things such as the settings of
    // the EU flag don't matter here.

    for (uint i = (uint)pos; i < curbuf->put; ++i)
    {
        echo_chr(curbuf->buf[i]);
    }
}


///
///  @brief    Check if buffer is empty.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

bool empty_buf(void)
{
    assert(curbuf != NULL);
    
    return ((curbuf->put - curbuf->get) == 0);
}


///
///  @brief    Fetch next character from buffer.
///
///  @returns  Character fetched.
///
////////////////////////////////////////////////////////////////////////////////

int fetch_buf(void)
{
    assert(curbuf != NULL);
    
    if (curbuf->get == curbuf->put)
    {
        curbuf->get = curbuf->put = 0;

        print_err(E_UTC);               // Unterminated command
    }

    return curbuf->buf[curbuf->get++];
}


///
///  @brief    Free up memory for command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void free_buf(void)
{
    curbuf = &cmd;

    if (curbuf->buf != NULL)
    {
        free(curbuf->buf);

        curbuf->buf = NULL;
        curbuf->size = curbuf->get = curbuf->put = 0;
    }
}


///
///  @brief    Initialize command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_buf(void)
{
    curbuf = &cmd;

    assert(curbuf->size == 0);         // Allow only one call
  
    curbuf->size = STR_SIZE_INIT;

    if ((curbuf->buf = calloc(1uL, (size_t)curbuf->size)) == NULL)
    {
        exit(EXIT_FAILURE);             // Clean up, reset, and exit
    }

    curbuf->get = curbuf->put = 0;

    if (atexit(free_buf) != 0)          // Ensure we clean up on exit
    {
        exit(EXIT_FAILURE);
    }
}


///
///  @brief    See if beginning of buffer matches passed string.
///
///  @returns  true if match, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool match_buf(const char *str)
{
    assert(str != NULL);
    assert(curbuf != NULL);

    uint len = (uint)strlen(str);

    if (strncasecmp(curbuf->buf, str, (size_t)len))
    {
        return false;
    }
    else
    {
        return true;
    }
}


///
///  @brief    Get pointer to next character in buffer.
///
///  @returns  Pointer to next character.
///
////////////////////////////////////////////////////////////////////////////////

char *next_buf(void)
{
    assert(curbuf != NULL);

    if (curbuf->get == curbuf->put)
    {
        return NULL;
    }

    return curbuf->buf + curbuf->get;
}


///
///  @brief    Reset command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_buf(void)
{
    curbuf = &cmd;

    curbuf->get = curbuf->put = 0;
}


///
///  @brief    Get start of line.
///
///  @returns  Index of start of line.
///
////////////////////////////////////////////////////////////////////////////////

uint start_buf(void)
{
    assert(curbuf != NULL);

    uint i = curbuf->put;

    while (i > 0)
    {
        // Back up on line until we find a line terminator.

        int c = curbuf->buf[i];

        if (c == LF || c == VT || c == FF)
        {
            break;
        }

        --i;
    }

    return i;                           // Return index of start of line
}


///
///  @brief    Store new character in buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_buf(int c)
{
    assert(curbuf != NULL);

    // If we run out of room for the command string, try to increase it by
    // calling realloc(). Note that this may move the block, so we have to
    // reinitialize all of our pointers.

    if (curbuf->put == curbuf->size)            // Has buffer filled up?
    {
        // If size is 0, then init_buf() hasn't been called yet. realloc() will
        // function like malloc() if its first argument is zero, but it's a bad
        // coding practice, so we don't allow it.

        assert(curbuf->size != 0);
        assert(curbuf->buf != NULL);

        uint newsize = curbuf->size + STR_SIZE_INIT;
        char *newbuf = alloc_more(curbuf->buf, curbuf->size, newsize);

        curbuf->size = newsize;
        curbuf->buf  = newbuf;
    }

    curbuf->buf[curbuf->put++] = (char)c;
    curbuf->buf[curbuf->put] = NUL;
}


///
///  @brief    Returns previously fetched character to buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void unfetch_buf(int c)
{
    assert(curbuf != NULL);

    if (curbuf->get != 0)
    {
        curbuf->buf[--curbuf->get] = (char)c;
    }
}
