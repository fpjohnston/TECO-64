///
///  @file    m_cmd.c
///  @brief   Execute M command.
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
#include "errors.h"
#include "exec.h"
#include "qreg.h"

#define MAX_LEVELS      64              ///< Max. macro levels

static uint macro_level = 0;            ///< Current macro level


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
    assert(cmd != NULL);

    struct qreg *qreg = get_qreg(cmd->qname, cmd->qlocal);

    assert(qreg != NULL);

    // Nothing to do if macro is empty

    if (qreg->text == NULL || qreg->text->put == 0)
    {
        return;
    }

    if (++macro_level == MAX_LEVELS)    // Have we reached maximum depth?
    {
        macro_level = 0;

        print_err(E_MEM);               // Memory overflow
    }

    qreg->text->get = 0;

    uint saved_put = qreg->text->put;
    struct buffer *saved_buf = get_buf();

    set_buf(qreg->text);

    // If invoked with nMq or m,nMq, then pass argument(s) to macro.

    if (cmd->n_set)
    {
        if (cmd->m_set)
        {
            push_expr(cmd->m_arg, EXPR_VALUE);
        }

        push_expr(cmd->n_arg, EXPR_VALUE);
    }

    // TODO: check for saving local Q-registers.

    exec_cmd();

    set_buf(saved_buf);

    qreg->text->put = saved_put;

    --macro_level;
}
