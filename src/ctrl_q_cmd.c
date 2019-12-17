///
///  @file    ctrl_q_cmd.c
///  @brief   Execute ^Q (CTRL/Q) command.
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
#include "textbuf.h"
#include "exec.h"


///
///  @brief    Scan ^Q (CTRL/Q) command: get no. of characters between dot and
///            nth line terminator. n may be negative.
///
///  @returns  nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_ctrl_q(struct cmd *cmd)
{
    assert(cmd != NULL);

    int n = cmd->n_arg;

    if (operand_expr())
    {
        cmd->n_set = true;
        cmd->n_arg = get_n_arg();
    }
    else
    {
        cmd->n_arg = 0;
    }

    int nchrs = getdelta_tbuf(n);
    
    push_expr(nchrs, EXPR_VALUE);
}
