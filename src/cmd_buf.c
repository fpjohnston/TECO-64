///
///  @file    cmd_buf.c
///  @brief   Command buffer-handling functions.
///
///  @copyright  2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include "eflags.h"
#include "errors.h"
#include "exec.h"
#include "estack.h"
#include "term.h"


struct buffer *cbuf;                    ///< Current command string buffer

static struct buffer *root;             ///< Command string buffer root

// Local functions

static void exit_cbuf(void);


///
///  @brief    Check to see if next command is ; or :;. Normally search commands
///            return values only if preceded by a colon, but we need to do a
///            lookahead here so that we can return a value if a semi-colon
///            follows a search command.
///
///  @returns  true if next command is ;, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool check_semi(void)
{
    int saved_pos = cbuf->pos;
    struct cmd cmd;

    while (cbuf->pos < cbuf->len)
    {
        if (!skip_cmd(&cmd, ";"))
        {
            break;                      // No more commands, so quit
        }

        assert(cmd.c1 == ';');

        cbuf->pos = saved_pos;

        return true;
    }

    cbuf->pos = saved_pos;

    return false;
}


///
///  @brief    Clean up memory before we exit from TECO.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exit_cbuf(void)
{
    cbuf = NULL;

    if (root != NULL && root->data != NULL)
    {
        root->size = 0;
        root->pos  = 0;
        root->len  = 0;

        free_mem(&root->data);
        free_mem(&root);
    }
}


///
///  @brief    Fetch next character from command string. Issues error if no
///            character available.
///
///  @returns  Command character.
///
////////////////////////////////////////////////////////////////////////////////

int fetch_cbuf(void)
{
    if (empty_cbuf())
    {
        if (loop_depth != 0)
        {
            throw(E_UTL);               // Unterminated loop
        }
        else if (if_depth != 0)
        {
            throw(E_UTQ);               // Unterminated conditional
        }
        else if (check_macro())
        {
            throw(E_UTM);               // Unterminated macro
        }
        else
        {
            throw(E_UTC);               // Unterminated command
        }
    }

    int c = cbuf->data[cbuf->pos++];

    if (f.e0.trace)
    {
        echo_in(c);
    }

    return c;
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

    root = alloc_mem((uint)sizeof(*root));

    root->len  = 0;
    root->pos  = 0;
    root->size = STR_SIZE_INIT;
    root->data = alloc_mem(root->size);

    cbuf = root;
}


///
///  @brief    Reset command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_cbuf(void)
{
    assert(root != NULL);               // Verify default command buffer

    cbuf = root;

    cbuf->pos = 0;
    cbuf->len = 0;
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

    assert(cbuf != NULL);               // Verify command string
    assert(cbuf->data != NULL);         // Verify command buffer

    if (cbuf->len == cbuf->size)        // Has buffer filled up?
    {
        assert(cbuf->size != 0);        // Verify non-zero size

        // Round up size to a multiple of STR_SIZE_INIT

        cbuf->size += STR_SIZE_INIT - 1;
        cbuf->size /= STR_SIZE_INIT;
        cbuf->size *= STR_SIZE_INIT;

        uint newsize = cbuf->size + STR_SIZE_INIT;
        char *newbuf = expand_mem(cbuf->data, cbuf->size, newsize);

        cbuf->size = newsize;
        cbuf->data  = newbuf;
    }

    cbuf->data[cbuf->len++] = (char)c;
    cbuf->data[cbuf->len] = NUL;
}
