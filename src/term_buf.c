///
///  @file    term_buf.c
///  @brief   Terminal buffer-handling functions.
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
#include "term.h"


struct buffer *term_buf;                ///< Command buffer for terminal input

// Local functions

static void exit_tbuf(void);


///
///  @brief    Delete last character from buffer and return it.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

int delete_tbuf(void)
{
    assert(term_buf != NULL);

    if (term_buf->len == 0)             // Anything in buffer?
    {
        return EOF;                     // No
    }

    return term_buf->buf[--term_buf->len]; // Delete character and return it
}


///
///  @brief    Echo all characters in buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void echo_tbuf(int pos)
{
    assert(term_buf != NULL);

    assert((uint)pos <= term_buf->len);

    // Just echo everything we're supposed to print. Note that this is not the
    // same as typing out what's in a buffer, so things such as the settings of
    // the EU flag don't matter here.

    for (uint i = (uint)pos; i < term_buf->len; ++i)
    {
        echo_out(term_buf->buf[i]);
    }
}


///
///  @brief    Clean up memory before we exit from TECO. Note that the only
///            command level for which we need be concerned here is level 0;
///            all other levels are for macros, which will be freed up by
///            Q-register functions.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exit_tbuf(void)
{
    if (term_buf != NULL)
    {
        if (term_buf->buf != NULL)
        {
            free_mem(&term_buf->buf);
        }

        term_buf->size = 0;
        term_buf->pos  = 0;
        term_buf->len  = 0;

        free_mem(&term_buf);
    }
}


///
///  @brief    Fetch next character from buffer.
///
///  @returns  Character fetched, or EOF if no character available.
///
////////////////////////////////////////////////////////////////////////////////

int fetch_tbuf(void)
{
    assert(term_buf != NULL);

    if (term_buf->pos == term_buf->len)
    {
        return EOF;
    }

    return term_buf->buf[term_buf->pos++];
}


///
///  @brief    Initialize command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_tbuf(void)
{
    register_exit(exit_tbuf);

    term_buf = alloc_mem((uint)sizeof(struct buffer));

    term_buf->len  = 0;
    term_buf->pos  = 0;
    term_buf->size = STR_SIZE_INIT;
    term_buf->buf  = alloc_mem(term_buf->size);
}


///
///  @brief    Reset command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_tbuf(void)
{
    assert(term_buf != NULL);

    term_buf->pos = 0;
    term_buf->len = 0;
}


///
///  @brief    Get start of line.
///
///  @returns  Index of start of line.
///
////////////////////////////////////////////////////////////////////////////////

uint start_tbuf(void)
{
    uint i = term_buf->len;

    while (i > 0)
    {
        // Back up on line until we find a line terminator.

        int c = term_buf->buf[i];

        if (isdelim(c))
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

void store_tbuf(int c)
{
    // If we haven't yet allocated space for the command string, do so now.
    // If we have filled up the current command string, try to increase it by
    // calling realloc(). Note that this may move the block, so we have to
    // reinitialize all of our pointers.

    assert(term_buf != NULL);
    assert(term_buf->buf != NULL);

    if (term_buf->len == term_buf->size)    // Has buffer filled up?
    {
        assert(term_buf->size != 0);

        // Round up size to a multiple of STR_SIZE_INIT

        term_buf->size += STR_SIZE_INIT - 1;
        term_buf->size /= STR_SIZE_INIT;
        term_buf->size *= STR_SIZE_INIT;

        uint newsize = term_buf->size + STR_SIZE_INIT;
        char *newbuf = expand_mem(term_buf->buf, term_buf->size, newsize);

        term_buf->size = newsize;
        term_buf->buf  = newbuf;
    }

    term_buf->buf[term_buf->len++] = (char)c;
    term_buf->buf[term_buf->len] = NUL;
}
