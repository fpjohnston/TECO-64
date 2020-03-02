///
///  @file    er_cmd.c
///  @brief   Execute ER command.
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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "file.h"


///
///  @brief    Execute ER command: open file for input.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ER(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->text1.len == 0)            // ER`?
    {
        istream = IFILE_PRIMARY;

        return;
    }

    create_filename(&cmd->text1);

    if (open_input(filename_buf, istream) == EXIT_FAILURE)
    {
        if (!cmd->colon_set || (errno != ENOENT && errno != ENODEV))
        {
            prints_err(E_FNF, last_file);
        }

        push_expr(TECO_FAILURE, EXPR_VALUE);
    }
    else if (cmd->colon_set)
    {
        push_expr(TECO_SUCCESS, EXPR_VALUE);
    }
}

