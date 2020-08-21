///
///  @file    insert_cmd.c
///  @brief   Execute TAB (CTRL/I) and I commands.
///
///  @bug     No known bugs.
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
#include "errors.h"
#include "exec.h"


#define INSERT_MAX      1024            ///< Max. length of insert string

static char insert_string[INSERT_MAX + 1]; ///< Last string inserted in buffer


///
///  @brief    Execute ^I (CTRL/I) command (insert text).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_I(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    insert_string[0] = TAB;

    int nbytes = snprintf(insert_string + 1, sizeof(insert_string) - 1, "%.*s",
                          (int)cmd->text1.len, cmd->text1.buf);

    if (nbytes >= (int)sizeof(insert_string) - 1)
    {
        throw(E_MIX);                   // Maximum insert string exceeded
    }

    exec_insert(insert_string, 1 + cmd->text1.len);
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
        int nbytes = snprintf(insert_string, sizeof(insert_string), "%.*s",
                              (int)cmd->text1.len, cmd->text1.buf);

        if (nbytes >= (int)sizeof(insert_string))
        {
            throw(E_MIX);               // Maximum insert string exceeded
        }

        exec_insert(insert_string, cmd->text1.len);
    }
    else if (cmd->n_set)
    {
        char c = (char)cmd->n_arg;

        exec_insert(&c, 1);
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

    for (uint i = 0; i < len; ++i)
    {
        int c = *buf++;

        if (c == CR)
        {
            continue;
        }

        switch (add_ebuf(c))
        {
            case EDIT_FULL:
            case EDIT_ERROR:
                return;

            case EDIT_WARN:
            default:
                break;
        }
    }

    last_len = len;
}
