///
///  @file    term_buf.c
///  @brief   Terminal buffer-handling functions.
///
///  @copyright 2019-2020 Franklin P. Johnston / Nowwith Treble Software
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


struct buffer *term_block;              ///< Terminal input block

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
    assert(term_block != NULL);         // Error if no terminal buffer

    if (term_block->len == 0)           // Anything in buffer?
    {
        return EOF;                     // No
    }

    return term_block->data[--term_block->len]; // Delete character and return it
}


///
///  @brief    Echo all characters in buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void echo_tbuf(int pos)
{
    assert(term_block != NULL);           // Error if no terminal buffer
    assert((uint)pos <= term_block->len); // Error if no data overrun

    // Just echo everything we're supposed to print. Note that this is not the
    // same as typing out what's in a buffer, so things such as the settings of
    // the EU flag don't matter here.

    for (uint i = (uint)pos; i < term_block->len; ++i)
    {
        echo_out(term_block->data[i]);
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
    if (term_block != NULL)
    {
        if (term_block->data != NULL)
        {
            free_mem(&term_block->data);
        }

        term_block->size = 0;
        term_block->pos  = 0;
        term_block->len  = 0;

        free_mem(&term_block);
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
    assert(term_block != NULL);         // Error if no terminal buffer

    if (term_block->pos == term_block->len)
    {
        return EOF;
    }

    return term_block->data[term_block->pos++];
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

    term_block = alloc_mem((uint)sizeof(*term_block));

    term_block->len  = 0;
    term_block->pos  = 0;
    term_block->size = STR_SIZE_INIT;
    term_block->data  = alloc_mem(term_block->size);
}


///
///  @brief    Reset command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_tbuf(void)
{
    assert(term_block != NULL);         // Error if no terminal buffer

    term_block->pos = 0;
    term_block->len = 0;
}


///
///  @brief    Get start of line.
///
///  @returns  Index of start of line.
///
////////////////////////////////////////////////////////////////////////////////

uint start_tbuf(void)
{
    uint i = term_block->len;

    while (i > 0)
    {
        // Back up on line until we find a line terminator.

        int c = term_block->data[i];

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

    assert(term_block != NULL);         // Error if no terminal block
    assert(term_block->data != NULL);   // Error if no buffer in block

    if (term_block->len == term_block->size) // Has buffer filled up?
    {
        assert(term_block->size != 0);  // Error if no data

        // Round up size to a multiple of STR_SIZE_INIT

        term_block->size += STR_SIZE_INIT - 1;
        term_block->size /= STR_SIZE_INIT;
        term_block->size *= STR_SIZE_INIT;

        uint newsize = term_block->size + STR_SIZE_INIT;
        char *newbuf = expand_mem(term_block->data, term_block->size, newsize);

        term_block->size = newsize;
        term_block->data  = newbuf;
    }

    term_block->data[term_block->len++] = (char)c;
    term_block->data[term_block->len] = NUL;
}
