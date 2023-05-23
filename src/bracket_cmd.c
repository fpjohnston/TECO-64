///
///  @file    bracket_cmd.c
///  @brief   Execute bracket commands.
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
#include <stdbool.h>
#include <stdio.h>

#include "teco.h"
#include "eflags.h"                 // Needed for confirm()
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "qreg.h"


///
///  @brief    Execute ] command: pop Q-register from push-down list.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_popQ(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (!pop_qreg(cmd->qindex))
    {
        if (!cmd->colon)
        {
            throw(E_CPQ);               // Can't pop into Q-register
        }

        store_val(FAILURE);
    }
    else if (cmd->colon)
    {
        store_val(SUCCESS);
    }
}


///
///  @brief    Execute [ command - Push Q-register onto push-down list.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_pushQ(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (!push_qreg(cmd->qindex))
    {
        throw(E_PDO);                   // Push-down list is full
    }
}


///
///  @brief    Scan ] command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_popQ(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_NEG_M, NO_M_ONLY, NO_DCOLON, NO_ATSIGN);

    scan_qreg(cmd);

    return false;
}


///
///  @brief    Scan [ command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_pushQ(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_M_ONLY, NO_COLON, NO_ATSIGN);

    scan_qreg(cmd);

    return false;
}
