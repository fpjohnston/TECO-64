///
///  @file    q_cmd.c
///  @brief   Execute Q command.
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

#include "teco.h"
#include "estack.h"
#include "exec.h"
#include "qreg.h"


///
///  @brief    Scan Q command: return numeric value of Q-register, or size of
///            text string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_Q(struct cmd *cmd)
{
    assert(cmd != NULL);

    int n;

    if (pop_expr(&n))                   // nQq
    {
        n = get_qchr(cmd->qname, cmd->qlocal, n);
    }
    else if (cmd->colon_set)            // :Qq
    {
        n = (int)get_qsize(cmd->qname, cmd->qlocal);
    }
    else                                // Qq
    {
        n = get_qnum(cmd->qname, cmd->qlocal);
    }

    push_expr(n, EXPR_VALUE);
}

