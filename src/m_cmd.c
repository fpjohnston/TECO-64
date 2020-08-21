///
///  @file    m_cmd.c
///  @brief   Execute M command.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston / Nowwith Treble Software
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

static uint macro_depth = 0;            ///< Current macro depth


///
///  @brief    Check to see if we're in a macro.
///
///  @returns  true if we're in a macro, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool check_macro(void)
{
    if (macro_depth != 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}


///
///  @brief    Execute M command: invoke macro in Q-register.
///
///                Mq -> Execute Q-register q as a command string.
///               nMq -> Same as Mq; use n as numeric argument.
///             m,nMq -> Same as Mq; use m,n as numeric arguments.
///               :Mq -> Same as Mq; no new local Q-registers.
///              n:Mq -> Same as nMq; no new local Q-registers.
///            m,n:Mq -> Same as m,nMq; no new local Q-registers.
///
///            All of the above combinations may be used for a local Q-register,
///            but no new set of local Q-registers is created.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_M(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    struct qreg *qreg = get_qreg(cmd->qname, cmd->qlocal);

    assert(qreg != NULL);               // Error if no Q-register

    // Nothing to do if macro is empty

    if (qreg->text.len == 0)
    {
        return;
    }

    qreg->text.pos = 0;

    uint saved_put = qreg->text.len;
    struct buffer *saved_buf = get_cbuf();

    set_cbuf(&qreg->text);

    // If no colon modifier, and not a local Q-register, then save current
    // local Q-registers before executing macro, and restore them afterwards.

    bool save_local = (!cmd->colon && !cmd->qlocal);

    if (save_local)
    {
        push_qlocal();
    }

    uint saved_base = set_expr();       // Set temporary new base

    if (cmd->n_set)
    {
        if (cmd->m_set)
        {
            push_expr(cmd->m_arg, EXPR_VALUE);
        }

        push_expr(cmd->n_arg, EXPR_VALUE);
    }
            
    ++macro_depth;
    exec_cmd();
    --macro_depth;

    reset_expr(saved_base);             // Restore old base

    if (save_local)
    {
        pop_qlocal();
    }

    set_cbuf(saved_buf);

    qreg->text.len = saved_put;
}


///
///  @brief    Reset macro depth.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_macro(void)
{
    macro_depth = 0;
}


