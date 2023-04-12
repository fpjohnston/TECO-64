///
///  @file    em_cmd.c
///  @brief   Execute EM command.
///
///  @copyright 2019-2023 Franklin P. Johnston / Nowwith Treble Software
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

#include "teco.h"
#include "eflags.h"
#include "exec.h"
#include "qreg.h"


///
///  @brief    Execute EM command: echo macro in Q-register according to bits
///            set in specified Q-register. Used to "squish" macros.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EM(struct cmd *cmd)
{
    assert(cmd != NULL);

    struct qreg *qreg = get_qreg(cmd->qindex);

    assert(qreg != NULL);               // Error if no Q-register

    if (qreg->text.len == 0)            // Nothing to do if no text
    {
        return;
    }

    tbuffer macro = qreg->text;
    int saved_trace = f.trace.flag;
    bool saved_exec = f.e0.exec;

    f.trace.enable  = true;
    f.trace.nospace = false;
    f.trace.noblank = false;
    f.trace.nowhite = false;
    f.trace.nobang  = false;
    f.trace.nobang2 = false;

    if (cmd->n_set)
    {
        f.trace.nospace = (cmd->n_arg & 1)  ? true : false;
        f.trace.noblank = (cmd->n_arg & 2)  ? true : false;
        f.trace.nowhite = (cmd->n_arg & 4)  ? true : false;
        f.trace.nobang  = (cmd->n_arg & 8)  ? true : false;
        f.trace.nobang2 = (cmd->n_arg & 16) ? true : false;
    }

    f.e0.exec = false;                  // Don't actually execute commands

    exec_macro(&macro, NULL);

    f.e0.exec = saved_exec;
    f.trace.flag = saved_trace;
}


///
///  @brief    Scan EM command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_EM(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd->m_set);
    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);
    scan_qreg(cmd);

    return false;
}
