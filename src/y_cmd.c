///
///  @file    y_cmd.c
///  @brief   Execute Y command.
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


///
///  @brief    Execute Y command: yank text into buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_Y(struct cmd *cmd)
{
    assert(cmd != NULL);

    struct ifile *ifile = &ifiles[istream];

    if (ifile->eof)
    {
        if (cmd->colon_set)
        {
            push_expr(TECO_FAILURE, EXPR_VALUE);

            return;
        }
    }

    if (cmd->n_set)
    {
        if (cmd->n_arg == -1)
        {
            print_err(E_T32);           // TECO-32 feature
        }
        else
        {
            print_err(E_NYA);           // Numeric argument with Y
        }
    }

    // If data in buffer and yank protection is enabled, then abort.

    if (get_Z() && !f.ed.yank)
    {
        print_err(E_YCA);               // Y command aborted
    }

    kill_edit(EDIT_NOSHRINK);

    while (append_line())               // Read what we can
    {
        ;
    }

    if (cmd->colon_set)
    {
        push_expr(TECO_SUCCESS, EXPR_VALUE);
    }
}
