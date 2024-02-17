///
///  @file    m_cmd.c
///  @brief   Execute M command.
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
#include <stdio.h>

#include "teco.h"
#include "cmdbuf.h"
#include "eflags.h"                 // Needed for confirm()
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "qreg.h"


#define MACRO_MAX   64                  ///< Maximum macro depth

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

    tbuffer macro = qreg->text;

    if (cmd->colon || cmd->qlocal)      // :Mq or using local Q-register?
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

void exec_macro(tbuffer *macro, struct cmd *cmd)
{
    assert(macro != NULL);
    assert(macro->data != NULL);

    if (macro_depth == MACRO_MAX)
    {
        throw(E_MAX);                   // Internal program limit reached
    }

    // Save current state

    struct ctrl saved_ctrl = ctrl;
    uint_t saved_line      = cmd_line;
    uint_t saved_pos       = macro->pos;
    tbuffer *saved_cbuf    = cbuf;

    // Initialize for new command string

    new_x();                            // Make new expression stack

    cbuf = macro;                       // Switch command strings
    cbuf->pos = 0;
    ctrl.depth = 0;
    ctrl.level = 0;

#if     !defined(NSTRICT)

    f.e0.digit = false;

#endif

    struct cmd newcmd = null_cmd;       // Initialize new command

    // If we were passed the previous command, then copy any m and n arguments.

    ++macro_depth;

    if (cmd == NULL)
    {
        exec_cmd(&newcmd);
    }
    else
    {
        newcmd.m_set = cmd->m_set;
        newcmd.m_arg = cmd->m_arg;

        if (cmd->n_set)
        {
            store_val(cmd->n_arg);
        }

        exec_cmd(&newcmd);

        if (query_x(&cmd->n_arg))
        {
            cmd->m_set = newcmd.m_set;
            cmd->m_arg = newcmd.m_arg;
            cmd->n_set = true;
            cmd->keep = true;           // Say we need to retain m & n args.
        }
        else
        {
            cmd->m_set = cmd->n_set = false;
            cmd->m_arg = cmd->n_arg = 0;
        }
    }

    --macro_depth;

    // Restore previous state

    cbuf = saved_cbuf;                  // Restore previous command string
    macro->pos = saved_pos;
    cmd_line = saved_line;
    ctrl = saved_ctrl;

    delete_x();                         // Restore previous expression stack
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


///
///  @brief    Scan M command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_M(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);
    confirm(cmd, NO_DCOLON, NO_ATSIGN);

    if (!scan_qreg(cmd))
    {
        throw(E_IQN, cmd->qname);       // Invalid Q-register name
    }

    return false;
}
