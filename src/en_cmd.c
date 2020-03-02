///
///  @file    en_cmd.c
///  @brief   Execute EN command.
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
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "file.h"


///
///  @brief    Execute EN command (wildcard filename lookup).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EN(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->text1.len == 0)            // Was it EN`?
    {
        if (get_wild() == EXIT_FAILURE)
        {
            if (!cmd->colon_set)
            {
                print_err(E_FNF);       // No more matches
            }

            push_expr(TECO_FAILURE, EXPR_VALUE);
        }
        else
        {
            if (cmd->colon_set)
            {
                push_expr(TECO_SUCCESS, EXPR_VALUE);
            }
        }
    }
    else                                // Have filespec, so must set it
    {
        char scratch[cmd->text1.len + 1];

        sprintf(scratch, "%.*s", (int)cmd->text1.len, cmd->text1.buf);

        set_wild(scratch);
    }
}

