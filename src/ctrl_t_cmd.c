///
///  @file    ctrl_t_cmd.c
///  @brief   Execute ^T (CTRL/T) command.
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
#include "eflags.h"
#include "exec.h"


///
///  @brief    Execute n^T (CTRL/T) command.
///
///              ^T  Read and decode next character typed.
///              ^T= Type ASCII value of next character.
///             n^T  Type ASCII character of value n.
///            n:^T  Output binary byte of value n.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_t(struct cmd *cmd)
{
    assert(cmd != NULL);
    assert(cmd->n_set == true);

    cmd->n_arg &= 0xFF;                 // Limit value to 8 bits

    if (cmd->colon_set || f.et.image)
    {
        putc_term(cmd->n_arg);
    }
    else
    {
        echo_chr(cmd->n_arg);
    }
}


///
///  @brief    Scan ^T (CTRL/T) command.
///
///              ^T  Read and decode next character typed.
///              ^T= Type ASCII value of next character.
///             n^T  Type ASCII character of value n.
///            n:^T  Output binary byte of value n.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_ctrl_t(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (operand_expr())                 // n argument?
    {
        scan_state = SCAN_DONE;
    }
    else if (scan_state != SCAN_DONE)
    {
        push_expr(DUMMY_VALUE, EXPR_VALUE);
    }
    else
    {
        bool wait = f.et.nowait ? false : true;
        int c = getc_term(wait);

        // TODO: check for CTRL/C?

        if (!f.et.noecho)
        {
            echo_chr(c);
        }

        push_expr(c, EXPR_VALUE);
    }
}

