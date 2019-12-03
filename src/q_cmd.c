///
///  @file    q_cmd.c
///  @brief   Execute Q command.
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

#include "teco.h"
#include "errors.h"
#include "exec.h"


///
///  @brief    Scan Q command: return numeric value of Q-register, or size of
///            text string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_Q(struct cmd *cmd)
{
    assert(cmd != NULL);

    int n;

    if (operand_expr())                 // nQq`?
    {
        n = get_n_arg();

        if (cmd->colon_set)
        {
            print_err(E_MOD);
        }

        n = get_qchr(cmd->qreg, cmd->qlocal, n);
    }
    else if (cmd->colon_set)            // :Qq`?
    {
        n = (int)get_qsize(cmd->qreg, cmd->qlocal);
    }
    else                                // Qq`
    {
        n = get_qnum(cmd->qreg, cmd->qlocal);
    }
        
    push_expr(n, EXPR_VALUE);
}

