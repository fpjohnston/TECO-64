///
///  @file    cmd_buf.c
///  @brief   Command buffer-handling functions.
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


struct buffer *cmd_buf;                 ///< Current command string buffer.

static struct buffer *term_buf;         ///< Command buffer for terminal input

// Local functions

static void free_buf(void);


///
///  @brief    Create copy of current buffer and return it. This is used by the
///            *x immediate-mode action command to save the command string in a
///            Q-register.
///
///  @returns  String copy of buffer.
///
////////////////////////////////////////////////////////////////////////////////

struct buffer *copy_buf(void)
{
    assert(cmd_buf != NULL);
    assert(cmd_buf->size != 0);

    struct buffer *clone = alloc_mem((uint)sizeof(struct buffer));

    clone->put  = cmd_buf->put;
    clone->get  = cmd_buf->get;
    clone->size = cmd_buf->size;
    clone->buf  = alloc_mem(clone->size);

    if (clone->put > 0 && cmd_buf->buf[clone->put - 1] == ESC)
    {
        --clone->put;
    }

    if (clone->put > 0 && cmd_buf->buf[clone->put - 1] == ESC)
    {
        --clone->put;
    }

    memcpy(clone->buf, cmd_buf->buf, (ulong)clone->put);

    return clone;
}


///
///  @brief    Delete last character from buffer and return it.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

int delete_buf(void)
{
    assert(cmd_buf != NULL);
    
    if (cmd_buf->put == 0)              // Anything in buffer?
    {
        return EOF;                     // No
    }

    return cmd_buf->buf[--cmd_buf->put]; // Delete character and return it
}


///
///  @brief    Echo all characters in buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void echo_buf(int pos)
{
    assert(cmd_buf != NULL);
    
    assert((uint)pos <= cmd_buf->put);

    // Just echo everything we're supposed to print. Note that this is not the
    // same as typing out what's in a buffer, so things such as the settings of
    // the EU flag don't matter here.

    for (uint i = (uint)pos; i < cmd_buf->put; ++i)
    {
        echo_chr(cmd_buf->buf[i]);
    }
}


///
///  @brief    Check if buffer is empty.
///
///  @returns  true if buffer is empty, false if it's not empty.
///
////////////////////////////////////////////////////////////////////////////////

bool empty_buf(void)
{
    assert(cmd_buf != NULL);

    return (cmd_buf->put == cmd_buf->get);
}


///
///  @brief    Fetch next character from buffer.
///
///  @returns  Character fetched, or EOF if no character available.
///
////////////////////////////////////////////////////////////////////////////////

int fetch_buf(void)
{
    assert(cmd_buf != NULL);
    
    if (cmd_buf->get == cmd_buf->put)
    {
        cmd_buf->get = cmd_buf->put = 0;

        return EOF;
    }

    return cmd_buf->buf[cmd_buf->get++];
}


///
///  @brief    Free up memory for command string. The only command level for
///            which we need be concerned here is level 0; all other levels are
///            for macros, which will be freed up by Q-register functions.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void free_buf(void)
{
    cmd_buf = NULL;

    if (term_buf != NULL && term_buf->buf != NULL)
    {
        term_buf->size = 0;
        term_buf->get  = 0;
        term_buf->put  = 0;

        free_mem(&term_buf->buf);
        free_mem(&term_buf);
    }
}


///
///  @brief    Get current command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

struct buffer *get_buf(void)
{
    return cmd_buf;
}


///
///  @brief    Initialize command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_buf(void)
{
    term_buf = alloc_mem((uint)sizeof(struct buffer));

    term_buf->put  = 0;
    term_buf->get  = 0;
    term_buf->size = STR_SIZE_INIT;
    term_buf->buf  = alloc_mem(term_buf->size);

    cmd_buf = term_buf;

    if (atexit(free_buf) != 0)          // Ensure we clean up on exit
    {
        exit(EXIT_FAILURE);
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
    assert(cmd_buf != NULL);

    if (cmd_buf->get == cmd_buf->put)
    {
        return NULL;
    }

    return cmd_buf->buf + cmd_buf->get;
}


///
///  @brief    Reset command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_buf(void)
{
    assert(term_buf != NULL);

    cmd_buf = term_buf;

    cmd_buf->get = 0;
    cmd_buf->put = 0;
}


///
///  @brief    Set command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void set_buf(struct buffer *buf)
{
    assert(buf != NULL);

    cmd_buf = buf;
}


///
///  @brief    Get start of line.
///
///  @returns  Index of start of line.
///
////////////////////////////////////////////////////////////////////////////////

uint start_buf(void)
{
    uint i = cmd_buf->put;

    while (i > 0)
    {
        // Back up on line until we find a line terminator.

        int c = cmd_buf->buf[i];

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
    // If we haven't yet allocated space for the command string, do so now.
    // If we have filled up the current command string, try to increase it by
    // calling realloc(). Note that this may move the block, so we have to
    // reinitialize all of our pointers.

    assert(cmd_buf != NULL);
    assert(cmd_buf->buf != NULL);

    if (cmd_buf->put == cmd_buf->size)    // Has buffer filled up?
    {
        assert(cmd_buf->size != 0);

        // Round up size to a multiple of STR_SIZE_INIT

        cmd_buf->size += STR_SIZE_INIT - 1;
        cmd_buf->size /= STR_SIZE_INIT;
        cmd_buf->size *= STR_SIZE_INIT;

        uint newsize = cmd_buf->size + STR_SIZE_INIT;
        char *newbuf = expand_mem(cmd_buf->buf, cmd_buf->size, newsize);

        cmd_buf->size = newsize;
        cmd_buf->buf  = newbuf;
    }

    cmd_buf->buf[cmd_buf->put++] = (char)c;
    cmd_buf->buf[cmd_buf->put] = NUL;
}


///
///  @brief    Returns previously fetched character to buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void unfetch_buf(int c)
{
    assert(cmd_buf != NULL);

    if (cmd_buf->get != 0)
    {
        cmd_buf->buf[--cmd_buf->get] = (char)c;
    }
}
