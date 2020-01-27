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
#include "eflags.h"
#include "errors.h"


struct buffer *cmdbuf;                  ///< Current command string buffer.

static struct buffer *termbuf;         ///< Command buffer for terminal input

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
    assert(cmdbuf != NULL);
    assert(cmdbuf->size != 0);

    struct buffer *clone = alloc_mem((uint)sizeof(struct buffer));

    clone->len  = cmdbuf->len;
    clone->pos  = cmdbuf->pos;
    clone->size = cmdbuf->size;
    clone->buf  = alloc_mem(clone->size);

    if (clone->len > 0 && cmdbuf->buf[clone->len - 1] == ESC)
    {
        --clone->len;
    }

    if (clone->len > 0 && cmdbuf->buf[clone->len - 1] == ESC)
    {
        --clone->len;
    }

    memcpy(clone->buf, cmdbuf->buf, (ulong)clone->len);

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
    assert(cmdbuf != NULL);
    
    if (cmdbuf->len == 0)              // Anything in buffer?
    {
        return EOF;                     // No
    }

    return cmdbuf->buf[--cmdbuf->len]; // Delete character and return it
}


///
///  @brief    Echo all characters in buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void echo_buf(int pos)
{
    assert(cmdbuf != NULL);
    
    assert((uint)pos <= cmdbuf->len);

    // Just echo everything we're supposed to print. Note that this is not the
    // same as typing out what's in a buffer, so things such as the settings of
    // the EU flag don't matter here.

    for (uint i = (uint)pos; i < cmdbuf->len; ++i)
    {
        echo_out(cmdbuf->buf[i]);
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
    assert(cmdbuf != NULL);

    return (cmdbuf->len == cmdbuf->pos);
}


///
///  @brief    Fetch next character from buffer.
///
///  @returns  Character fetched, or EOF if no character available.
///
////////////////////////////////////////////////////////////////////////////////

int fetch_buf(bool start)
{
    assert(cmdbuf != NULL);
    
    if (cmdbuf->pos == cmdbuf->len)
    {
        if (start)
        {
            return EOF;
        }
        else if (macro_depth > 0)
        {
            print_err(E_UTM);           // Unterminated macro
        }
        else
        {
            print_err(E_UTC);           // Unterminated command
        }
    }

    int c = cmdbuf->buf[cmdbuf->pos++];

    if (v.trace && !f.e0.dryrun)
    {
        echo_in(c);
    }

    return c;
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
    cmdbuf = NULL;

    if (termbuf != NULL && termbuf->buf != NULL)
    {
        termbuf->size = 0;
        termbuf->pos  = 0;
        termbuf->len  = 0;

        free_mem(&termbuf->buf);
        free_mem(&termbuf);
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
    return cmdbuf;
}


///
///  @brief    Initialize command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_buf(void)
{
    termbuf = alloc_mem((uint)sizeof(struct buffer));

    termbuf->len  = 0;
    termbuf->pos  = 0;
    termbuf->size = STR_SIZE_INIT;
    termbuf->buf  = alloc_mem(termbuf->size);

    cmdbuf = termbuf;

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
    assert(cmdbuf != NULL);

    if (cmdbuf->pos == cmdbuf->len)
    {
        return NULL;
    }

    return cmdbuf->buf + cmdbuf->pos;
}


///
///  @brief    Reset command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_buf(void)
{
    assert(termbuf != NULL);

    cmdbuf = termbuf;

    cmdbuf->pos = 0;
    cmdbuf->len = 0;
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

    cmdbuf = buf;
}


///
///  @brief    Get start of line.
///
///  @returns  Index of start of line.
///
////////////////////////////////////////////////////////////////////////////////

uint start_buf(void)
{
    uint i = cmdbuf->len;

    while (i > 0)
    {
        // Back up on line until we find a line terminator.

        int c = cmdbuf->buf[i];

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

    assert(cmdbuf != NULL);
    assert(cmdbuf->buf != NULL);

    if (cmdbuf->len == cmdbuf->size)    // Has buffer filled up?
    {
        assert(cmdbuf->size != 0);

        // Round up size to a multiple of STR_SIZE_INIT

        cmdbuf->size += STR_SIZE_INIT - 1;
        cmdbuf->size /= STR_SIZE_INIT;
        cmdbuf->size *= STR_SIZE_INIT;

        uint newsize = cmdbuf->size + STR_SIZE_INIT;
        char *newbuf = expand_mem(cmdbuf->buf, cmdbuf->size, newsize);

        cmdbuf->size = newsize;
        cmdbuf->buf  = newbuf;
    }

    cmdbuf->buf[cmdbuf->len++] = (char)c;
    cmdbuf->buf[cmdbuf->len] = NUL;
}


///
///  @brief    Returns previously fetched character to buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void unfetch_buf(int c)
{
    assert(cmdbuf != NULL);

    if (cmdbuf->pos != 0)
    {
        cmdbuf->buf[--cmdbuf->pos] = (char)c;
    }
}
