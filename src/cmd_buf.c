///
///  @file    cmd_buf.c
///  @brief   Command buffer-handling functions.
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
#include "exec.h"
#include "errors.h"


struct buffer *current;                 ///< Current command string buffer

static struct buffer *cmd_buf;          ///< Command string command buffer

// Local functions

static void exit_cbuf(void);


///
///  @brief    Check to see if next command is ; or :;. Normally search commands
///            return values only if preceded by a colon, but we need to do a
///            lookahead here so that we can return a value if a semi-colon
///            follows a search command.
///
///  @returns  true if next command is ; or :;, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool check_semi(void)
{
    bool colon = false;                 // Allow : before ;

    // Check all remaining characters in command buffer.

    for (uint i = current->pos; i < current->len; ++i)
    {
        int c = current->buf[i];

        if (c == ';')                   // semi-colon found
        {
            return true;
        }
        else if (c == ':' && !colon)    // Allow colon if we haven't seen one
        {
            colon = true;
        }
        else if (c != NUL && c != LF && c != FF && c != CR && c != SPACE)
        {
            break;                      // Quit if any non-whitespace
        }
    }

    return false;                       // Semi-colon not found
}


///
///  @brief    Clean up memory before we exit from TECO.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exit_cbuf(void)
{
    current = NULL;

    if (cmd_buf != NULL && cmd_buf->buf != NULL)
    {
        cmd_buf->size = 0;
        cmd_buf->pos  = 0;
        cmd_buf->len  = 0;

        free_mem(&cmd_buf->buf);
        free_mem(&cmd_buf);
    }
}


///
///  @brief    Fetch next character from buffer.
///
///  @returns  Character fetched, or EOF if no character available.
///
////////////////////////////////////////////////////////////////////////////////

int fetch_cbuf(bool start)
{
    assert(current != NULL);

    if (current->pos == current->len)
    {
        // End of buffer if we're at the start of a command

        if (start)
        {
            return EOF;
        }
        else if (check_macro())
        {
            print_err(E_UTM);           // Unterminated macro
        }
        else if (loop_depth != 0)
        {
            print_err(E_UTL);           // Unterminated loop
        }
        else if (if_depth != 0)
        {
            print_err(E_UTQ);           // Unterminated conditional
        }
        else
        {
            print_err(E_UTC);           // Unterminated command
        }
    }

    int c = current->buf[current->pos++];

    return c;
}


///
///  @brief    Get current command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

struct buffer *get_cbuf(void)
{
    return current;
}


///
///  @brief    Initialize command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_cbuf(void)
{
    register_exit(exit_cbuf);

    cmd_buf = alloc_mem((uint)sizeof(struct buffer));

    cmd_buf->len  = 0;
    cmd_buf->pos  = 0;
    cmd_buf->size = STR_SIZE_INIT;
    cmd_buf->buf  = alloc_mem(cmd_buf->size);

    current = cmd_buf;
}


///
///  @brief    Get pointer to next character in buffer.
///
///  @returns  Pointer to next character.
///
////////////////////////////////////////////////////////////////////////////////

char *next_cbuf(void)
{
    assert(current != NULL);

    if (current->pos == current->len)
    {
        return NULL;
    }

    return current->buf + current->pos;
}


///
///  @brief    Reset command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_cbuf(void)
{
    assert(cmd_buf != NULL);

    current = cmd_buf;

    current->pos = 0;
    current->len = 0;
}


///
///  @brief    Set command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void set_cbuf(struct buffer *buf)
{
    assert(buf != NULL);

    current = buf;
}


///
///  @brief    Store new character in buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void store_cbuf(int c)
{
    // If we haven't yet allocated space for the command string, do so now.
    // If we have filled up the current command string, try to increase it by
    // calling realloc(). Note that this may move the block, so we have to
    // reinitialize all of our pointers.

    assert(current != NULL);
    assert(current->buf != NULL);

    if (current->len == current->size)    // Has buffer filled up?
    {
        assert(current->size != 0);

        // Round up size to a multiple of STR_SIZE_INIT

        current->size += STR_SIZE_INIT - 1;
        current->size /= STR_SIZE_INIT;
        current->size *= STR_SIZE_INIT;

        uint newsize = current->size + STR_SIZE_INIT;
        char *newbuf = expand_mem(current->buf, current->size, newsize);

        current->size = newsize;
        current->buf  = newbuf;
    }

    current->buf[current->len++] = (char)c;
    current->buf[current->len] = NUL;
}


///
///  @brief    Returns previously fetched character to buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void unfetch_cbuf(int c)
{
    assert(current != NULL);

    if (current->pos != 0)
    {
        current->buf[--current->pos] = (char)c;
    }
}
