///
///  @file    q_cmd.c
///  @brief   Execute Q command.
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

#include "teco.h"
#include "errors.h"
#include "exec.h"


///
///  @brief    Execute Q command - Return numeric part of Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_Q(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (operand_expr())                 // nQq
    {
        cmd->n = get_n_arg();

        if (cmd->got_colon)
        {
            print_err(E_MOD);
        }

        printf("get ASCII value of chr. #%d in Q-register %s%c\r\n",
               cmd->n, cmd->qlocal ? "." : "", cmd->qreg);

        push_expr(1, EXPR_OPERAND);
    }
    else if (cmd->got_colon)            // :Qq
    {
        printf("return # of chrs. in Q-register %s%c\r\n",
               cmd->qlocal ? "." : "", cmd->qreg);

        push_expr(1, EXPR_OPERAND);
    }
    else                                // Qq
    {
        printf("return numeric value in Q-register %s%c\r\n",
               cmd->qlocal ? "." : "", cmd->qreg);
        
        push_expr(1, EXPR_OPERAND);
    }
        
    fflush(stdout);
}

