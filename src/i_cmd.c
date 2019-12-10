///
///  @file    i_cmd.c
///  @brief   Execute I command.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "edit_buf.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"

char insert_string[1024 + 1];

///
///  @brief    Execute I command (insert text).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_I(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->n_set && cmd->text1.len != 0)
    {
        if (f.ei.strict)
        {
            print_err(E_MOD);
        }
    }

    if (cmd->text1.len != 0)
    {
        assert(cmd->text1.len <= 1024); // FIXME!

        sprintf(insert_string, "%.*s", (int)cmd->text1.len, cmd->text1.buf);

        insert_edit(insert_string, cmd->text1.len);
    }
    else if (cmd->n_set)
    {
        char c = (char)cmd->n_arg;

        insert_edit(&c, 1);
    }
    else
    {
        // TODO: print error or warning?
    }
}

