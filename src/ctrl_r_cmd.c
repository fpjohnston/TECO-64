///
///  @file    ctrl_r_cmd.c
///  @brief   Execute ^R (CTRL/R) command.
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
///  @brief    Execute ^R (CTRL/R) command to set radix (8, 10, or 16 only).
///
///  @returns  nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_r(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (!cmd->got_n)                    // n argument?
    {
        push_expr(radix, EXPR_OPERAND); // No, just save what we have

        return;
    }

    if (cmd->n != 8 && cmd->n != 10 && cmd->n != 16)
    {
        print_err(E_IRA);               // Illegal radix argument
    }

    radix = cmd->n;                     // Set the radix
}
