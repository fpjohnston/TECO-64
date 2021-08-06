///
///  @file    term_buf.c
///  @brief   Terminal buffer-handling functions.
///
///  @copyright 2019-2021 Franklin P. Johnston / Nowwith Treble Software
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


static tbuffer term_buf;            ///< Terminal input block


///
///  @brief    Create copy of terminal buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

tbuffer copy_tbuf(void)
{
    tbuffer buf =
    {
        .len  = term_buf.len,
        .pos  = term_buf.pos,
        .size = term_buf.size,
    };

    buf.data  = alloc_mem(term_buf.size);

    memcpy(buf.data, &term_buf.data, (size_t)term_buf.size);

    return buf;
}


///
///  @brief    Delete last character from buffer and return it.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

int delete_tbuf(void)
{
    if (term_buf.len == 0)              // Anything in buffer?
    {
        return EOF;                     // No
    }

    return term_buf.data[--term_buf.len]; // Delete character and return it
}


///
///  @brief    Echo all characters in buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void echo_tbuf(uint pos)
{
    assert(pos <= term_buf.len);        // Error if no data overrun

    // Just echo everything we're supposed to print. Note that this is not the
    // same as typing out what's in a buffer, so things such as the settings of
    // the EU flag don't matter here.

    for (uint i = pos; i < term_buf.len; ++i)
    {
        type_out(term_buf.data[i]);
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

void exit_tbuf(void)
{
    if (term_buf.data != NULL)
    {
        free_mem(&term_buf.data);
    }

    term_buf.size = 0;
    term_buf.pos  = 0;
    term_buf.len  = 0;
}


///
///  @brief    Fetch next character from buffer.
///
///  @returns  Character fetched, or EOF if no character available.
///
////////////////////////////////////////////////////////////////////////////////

int fetch_tbuf(void)
{
    if (term_buf.pos == term_buf.len)
    {
        return EOF;
    }

    return term_buf.data[term_buf.pos++];
}


///
///  @brief    Get current length of terminal buffer.
///
///  @returns  Buffer length.
///
////////////////////////////////////////////////////////////////////////////////

int getlen_tbuf(void)
{
    return (int)term_buf.len;
}


///
///  @brief    Initialize command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_tbuf(void)
{
    term_buf.len  = 0;
    term_buf.pos  = 0;
    term_buf.size = KB;
    term_buf.data = alloc_mem(term_buf.size);
}


///
///  @brief    Reset command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_tbuf(void)
{
    term_buf.pos = 0;
    term_buf.len = 0;
}


///
///  @brief    Get start of line.
///
///  @returns  Index of start of line.
///
////////////////////////////////////////////////////////////////////////////////

uint_t start_tbuf(void)
{
    uint_t i = term_buf.len;

    while (i > 0)
    {
        // Back up on line until we find a line terminator.

        int c = term_buf.data[i];

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
    // If we have filled up the current command string, try to increase it by
    // calling realloc(). Note that this may move the block, so we have to
    // reinitialize all of our pointers.

    assert(term_buf.data != NULL);      // Error if no buffer in block

    if (term_buf.len == term_buf.size)  // Has buffer filled up?
    {
        assert(term_buf.size != 0);     // Error if no data

        char *newbuf = expand_mem(&term_buf.data, term_buf.size, KB);

        term_buf.size += KB;
        term_buf.data = newbuf;
    }

    term_buf.data[term_buf.len++] = (char)c;
}
