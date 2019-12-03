///
///  @file    ctrl_x_cmd.c
///  @brief   Execute ^X (CTRL/X) command.
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
///  @brief    Execute ^X (CTRL/X) command: set value of search mode flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_x(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->n_set)                     // n argument?
    {
        v.ctrl_x = cmd->n_arg;
    }
}


///
///  @brief    Scan ^X (CTRL/X) command: read value of search mode flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_ctrl_x(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (!operand_expr())                // ^X`?
    {
        push_expr(v.ctrl_x, EXPR_VALUE);
    }
    else
    {
        cmd->n_arg = get_n_arg();
        cmd->n_set = true;
    }
}

