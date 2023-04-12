///
///  @file    cmd_buf.c
///  @brief   Command buffer-handling functions.
///
///  @copyright 2019-2023 Franklin P. Johnston / Nowwith Treble Software
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

#include "teco.h"
#include "ascii.h"
#include "cbuf.h"
#include "eflags.h"
#include "errcodes.h"
#include "exec.h"
#include "term.h"


tbuffer *cbuf;                      ///< Current command string buffer

static tbuffer *root;               ///< Command string buffer root


///
///  @brief    Throw exception because of premature end of command string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

noreturn void abort_cbuf(void)
{
    if (check_macro())
    {
        throw(E_UTM);
    }
    else
    {
        throw(E_UTC);
    }
}


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
    uint_t saved_pos = cbuf->pos;
    bool colon = false;

    while (cbuf->pos < cbuf->len)
    {
        int c = cbuf->data[cbuf->pos++];

        if (c == ';')
        {
            cbuf->pos = saved_pos;

            return true;
        }
        else if (c == ':')
        {
            if (colon)                  // Allow only 1 colon before semi-colon
            {
                break;
            }

            colon = true;
        }
        else if (!isspace(c) || c == TAB)
        {
            break;
        }
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

void exit_cbuf(void)
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
///  @brief    Initialize command buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_cbuf(void)
{
    root = alloc_mem((uint_t)sizeof(*root));

    root->len  = 0;
    root->pos  = 0;
    root->size = KB;
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
    cbuf->data[0] = NUL;
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

        // Round up size to a multiple of 1 KB

        cbuf->size += KB - 1;
        cbuf->size /= KB;
        cbuf->size *= KB;

        char *newbuf = expand_mem(cbuf->data, cbuf->size, KB);

        cbuf->size += KB;
        cbuf->data = newbuf;
    }

    cbuf->data[cbuf->len++] = (char)c;
    cbuf->data[cbuf->len] = NUL;
}
