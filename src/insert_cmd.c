///
///  @file    insert_cmd.c
///  @brief   Execute TAB (CTRL/I) and I commands.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "editbuf.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"


///
///  @brief    Execute ^I (CTRL/I) command (insert text).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_I(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    char c = TAB;

    exec_insert(&c, (uint)sizeof(c));
    exec_insert(cmd->text1.data, cmd->text1.len);

    ++last_len;                         // Correct count for added TAB
}


///
///  @brief    Execute I command (insert text).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_I(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (cmd->n_set && cmd->text1.len != 0) // nItext`?
    {
        throw(E_IIA);                   // Illegal insert argument
    }

    if (cmd->text1.len != 0)
    {
        exec_insert(cmd->text1.data, cmd->text1.len);
    }
    else if (cmd->n_set)
    {
        char c = (char)cmd->n_arg;
        uint n = 1;                     // Default: insert 1 character

        if (cmd->m_set)
        {
            if (cmd->m_arg < 0)
            {
                throw(E_IIA);           // Illegal insert argument
            }
            else if ((n = (uint)cmd->m_arg) == 0)
            {
                return;                 // Don't insert if count is 0
            }
        }

        while (n-- > 0)
        {
            exec_insert(&c, (uint)sizeof(c));
        }
    }
}


///
///  @brief    Insert string at dot.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_insert(const char *buf, uint len)
{
    assert(buf != NULL);                // Error if no buffer

    last_len = 0;

    for (uint i = 0; i < len; ++i)
    {
        int c = *buf++;

        if (c == CR && !f.e3.icrlf)
        {
            continue;
        }

        int retval = add_ebuf(c);

        if (retval == EDIT_FULL || retval == EDIT_ERROR)
        {
            break;
        }

        ++last_len;
    }
}
