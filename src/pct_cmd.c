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
///  @brief    Execute "%" command: add value to Q-register, and return result
///            (unless command was colon-modified).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_pct(struct cmd *cmd)
{
    assert(cmd != NULL);

    cmd->n_arg += get_qnum(cmd->qindex);

    store_qnum(cmd->qindex, cmd->n_arg);

    if (!cmd->colon)
    {
        push_x(cmd->n_arg, X_OPERAND);
    }
    else if (!f.e1.percent)             // :%q, but is it allowed?
    {
        throw(E_EXT);                   // Extended feature not enabled
    }
}


///
///  @brief    Scan "%" command with format "n:Xq".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_pct(struct cmd *cmd)
{
    assert(cmd != NULL);

    default_n(cmd, 1);
    reject_m(cmd->m_set);
    reject_dcolon(cmd->dcolon);
    reject_atsign(cmd->atsign);
    scan_qreg(cmd);

    return false;
}
