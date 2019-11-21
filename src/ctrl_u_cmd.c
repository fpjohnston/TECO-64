///
///  @file    ctrl_u_cmd.c
///  @brief   Execute ^U (CTRL/U) command.
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
///  @brief    Execute ^U (CTRL/U) command - copy/append string in Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_u(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (operand_expr())                 // Is there an operand available?
    {
        cmd->n = get_n_arg();

        if (cmd->text1.len != 0)
        {
            print_err(E_MOD);           // Can't have both modifiers
        }

        printf("%s character '%c' to Q-register %s%c\r\n",
               cmd->got_colon ? "append" : "copy", cmd->n,
               cmd->qlocal ? "." : "", cmd->qreg);
        fflush(stdout);
    }
    else
    {
        if (cmd->text1.len == 0)
        {
            print_err(E_MOD);           // Must have at least one modifier
        }

        printf("%s string '%.*s' to Q-register %s%c\r\n",
               cmd->got_colon ? "append" : "copy",
               cmd->text1.len, cmd->text1.buf,
               cmd->qlocal ? "s" : "", cmd->qreg);
        fflush(stdout);
    }
}

