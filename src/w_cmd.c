///
///  @file    w_cmd.c
///  @brief   Execute W command.
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


///
///  @brief    Execute W command: process window functions.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_W(struct cmd *cmd)
{
    assert(cmd != NULL);

    // TODO: finish window command handling
}


///
///  @brief    Scan W command: process window functions.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_W(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (!operand_expr())                // W by itself does nothing
    {
        // TODO: error?
        return;
    }

    int n = get_n_arg();

    if (cmd->colon_set)                 // n:W returns a value
    {
        push_expr(1, EXPR_VALUE);       // Dummy expression

        return;
    }


    // Here for nW - send value to exec function.

    cmd->n_arg = n;
    cmd->n_set = true;

    scan_state = SCAN_DONE;
}
