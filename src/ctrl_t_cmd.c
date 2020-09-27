///
///  @file    ctrl_t_cmd.c
///  @brief   Execute ^T (CTRL/T) command.
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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "estack.h"
#include "exec.h"
#include "term.h"


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

void exec_ctrl_T(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (cmd->n_set)                     // n^T -> type out character
    {
        int m = 1;
        int n = cmd->n_arg;

        if (cmd->m_set)
        {
            m = cmd->m_arg;
        }

        for (int i = 0; i < m; ++i)
        {
            if (cmd->colon || f.et.image)
            {
                if (n >= 0)
                {
                    print_chr(n & 0xFF);
                }
                else
                {
                    print_chr(CR);
                    print_chr(LF);
                }
            }
            else
            {
                if (n >= 0)
                {
                    echo_out(n & 0xFF);
                }
                else
                {
                    echo_out(CR);
                    echo_out(LF);
                }
            }
        }
    }
    else                                // ^T -> read character from terminal
    {
        bool wait = f.et.nowait ? false : true;
        int c = getc_term(wait);

        if (!f.et.noecho && c != -1)
        {
            echo_out(c);
        }

        push_expr(c, EXPR_VALUE);
    }
}

