///
///  @file    insert_cmd.c
///  @brief   Execute TAB (CTRL/I) and I commands.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "errors.h"
#include "exec.h"
#include "textbuf.h"


static char insert_string[1024 + 1];    ///< Last string inserted in buffer


///
///  @brief    Execute ^I (CTRL/I) command (insert text).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_i(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->text1.len != 0)
    {
        insert_string[0] = TAB;

        assert(cmd->text1.len <= 1023); // FIXME!

        sprintf(insert_string + 1, "%.*s", (int)cmd->text1.len, cmd->text1.buf);

        exec_insert(insert_string, 1 + cmd->text1.len);
    }
    else
    {
        // TODO: print error or warning?
    }
}


///
///  @brief    Execute I command (insert text).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_I(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->n_set && cmd->text1.len != 0) // nItext`?
    {
        print_err(E_IIA);               // Illegal insert argument
    }

    if (cmd->text1.len != 0)
    {
        assert(cmd->text1.len <= 1024); // FIXME!

        sprintf(insert_string, "%.*s", (int)cmd->text1.len, cmd->text1.buf);

        exec_insert(insert_string, cmd->text1.len);
    }
    else if (cmd->n_set)
    {
        char c = (char)cmd->n_arg;

        exec_insert(&c, 1);
    }
    else
    {
        // TODO: print error or warning?
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
    if (buf == NULL)
    {
        printf("buffer is NULL\r\n");
    }

    assert(buf != NULL);

    for (uint i = 0; i < len; ++i)
    {
        int c = *buf++;

        if (c == CR)
        {
            continue;
        }

        switch (add_tbuf(c))
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
