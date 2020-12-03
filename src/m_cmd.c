///
///  @file    m_cmd.c
///  @brief   Execute M command.
///
///  @copyright 2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include "cbuf.h"
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
///  @brief    Execute "M" command: invoke macro in Q-register.
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

    struct qreg *qreg = get_qreg(cmd->qindex);

    assert(qreg != NULL);               // Error if no Q-register

    // Nothing to do if macro is empty

    if (qreg->text.len == 0)
    {
        return;
    }

    // We make a private copy of the Q-register, since some of the structure
    // members can get modified while processing the macro (esp. len).

    struct buffer macro = qreg->text;

    if (cmd->colon)                     // :Mq?
    {
        exec_macro(&macro, cmd);        // Yes, don't save local Q-registers
    }
    else                                // No, must save local Q-registers
    {
        push_qlocal();

        exec_macro(&macro, cmd);

        pop_qlocal();
    }
}


///
///  @brief    Execute macro. Called for M and EI commands.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_macro(struct buffer *macro, struct cmd *cmd)
{
    assert(macro != NULL);
    assert(cmd != NULL);
    assert(macro->data != NULL);

    // Save current state

    uint saved_base           = set_x(); // Save expression stack level
    uint saved_loop           = loop_depth;
    uint saved_if             = if_depth;
    uint saved_nparens        = nparens;
    uint saved_pos            = macro->pos;
    struct buffer *saved_cbuf = cbuf;

    // Initialize for new command string

    loop_depth = 0;
    if_depth   = 0;
    nparens    = 0;
    macro->pos = 0;
    cbuf       = macro;                 // Switch command strings

    if (cmd->n_set)
    {
        push_x(cmd->n_arg, X_OPERAND);
    }

    ++macro_depth;
    exec_cmd(cmd);
    --macro_depth;

    // Restore previous state

    cbuf       = saved_cbuf;            // Restore previous command string
    macro->pos = saved_pos;
    nparens    = saved_nparens;
    if_depth   = saved_if;
    loop_depth = saved_loop;
    reset_x(saved_base);                // Restore expression stack level
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
