///
///  @file    pct_cmd.c
///  @brief   Execute % command.
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
#include "exec.h"
#include "qreg.h"


///
///  @brief    Scan % command: add value to Q-register, and read result.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_pct(struct cmd *cmd)
{
    assert(cmd != NULL);

    int n = 1;                          // Assume %q`

    if (scan_state != SCAN_PASS2)
    {
        push_expr(1, EXPR_VALUE);

        return;
    }

    if (pop_expr(&cmd->n_arg))          // n%q`?
    {
        cmd->n_set = true;
    }

    n += get_qnum(cmd->qreg, cmd->qlocal);

    store_qnum(cmd->qreg, cmd->qlocal, n);

    push_expr(n, EXPR_VALUE);
}

