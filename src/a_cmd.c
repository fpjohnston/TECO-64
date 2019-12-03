///
///  @file    a_cmd.c
///  @brief   Execute A command.
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
#include <string.h>

#include "teco.h"
#include "exec.h"


///
///  @brief    Execute A command: append text to buffer.
///
///              A -> Append page to buffer.
///             :A -> Same as A, but returns -1/0 for success/failure.
///            n:A -> Appends n lines of text. Returns -1/0 for success/failure.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_A(struct cmd *cmd)
{
    assert(cmd != NULL);
}


///
///  @brief    Parse A command: get value of character in buffer.
///
///            nA - Value of nth character in buffer, 0 = first, -1 = last.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_A(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (operand_expr())                 // Is it nA or n:A?
    {
        cmd->n_arg = get_n_arg();
        cmd->n_set = true;

        if (!cmd->colon_set)            // nA or n:A?
        {
            // TODO: get actual value of character in buffer.

            push_expr(1, EXPR_VALUE);   // Dummy expression

            return;
        }
    }

    scan_state = SCAN_DONE;
}

