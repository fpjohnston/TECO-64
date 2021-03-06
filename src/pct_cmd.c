///
///  @file    pct_cmd.c
///  @brief   Scan and execute "%" command.
///
///  @copyright 2019-2021 Franklin P. Johnston / Nowwith Treble Software
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

#include "teco.h"
#include "eflags.h"
#include "estack.h"
#include "exec.h"
#include "qreg.h"


///
///  @brief    Execute "%" command: add value to Q-register, and return result.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_pct(struct cmd *cmd)
{
    assert(cmd != NULL);
    assert(cmd->n_set);

    store_qnum(cmd->qindex, cmd->n_arg);
}


///
///  @brief    Scan "%" command with format "n:Xq".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_pct(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->n_set)                     // n%q adds to Q-register q
    {
        cmd->n_arg += get_qnum(cmd->qindex);
    }
    else if (cmd->colon)                // :%q decrements Q-register q
    {
        cmd->n_arg = get_qnum(cmd->qindex) - 1;
    }
    else                                // %q increments Q-register q
    {
        cmd->n_arg = get_qnum(cmd->qindex) + 1;
    }

    cmd->n_set = true;

    push_x(cmd->n_arg, X_OPERAND);

    return false;
}
