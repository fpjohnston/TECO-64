///
///  @file    en_cmd.c
///  @brief   Execute EN command.
///
///  @author  Nowwith Treble Software
///
///  @bug     No known bugs.
///
///  @copyright  tbd
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
#include "errors.h"
#include "exec.h"


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

            push_expr(0, EXPR_OPERAND);
        }
        else
        {
            if (cmd->colon_set)
            {
                push_expr(-1, EXPR_OPERAND);
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

