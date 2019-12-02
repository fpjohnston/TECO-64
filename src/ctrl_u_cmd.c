///
///  @file    ctrl_u_cmd.c
///  @brief   Execute ^U (CTRL/U) command.
///
///  @author  Nowwith Treble Software
///
///  @bug     No known bugs.
///
///  @copyright  tbd
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


///
///  @brief    Execute ^U (CTRL/U) command: store/append string/character in
///            Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_u(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (operand_expr())                 // n^Uq`?
    {
        cmd->n_arg = get_n_arg();
        cmd->n_set = true;

        if (cmd->text1.len != 0)        // Does it have a text string?
        {
            print_err(E_MOD);           // Yes, that's an error
        }

        if (cmd->colon_set)             // n:^Uq`?
        {
            append_qchr(cmd->qreg, cmd->qlocal, cmd->n_arg);
        }
        else                            // n^Uq`
        {
            store_qchr(cmd->qreg, cmd->qlocal, cmd->n_arg);
        }
    }
    else                                // No n argument
    {
        if (cmd->text1.len == 0)        // No text either?
        {
            print_err(E_MOD);           // Yes, that's wrong
        }

        if (cmd->colon_set)             // :^Utext`
        {
            append_qtext(cmd->qreg, cmd->qlocal, cmd->text1);
        }
        else                            // ^Uqtext`
        {
            store_qtext(cmd->qreg, cmd->qlocal, cmd->text1);
        }
    }
}

