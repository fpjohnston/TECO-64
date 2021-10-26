///
///  @file    ctrl_t_cmd.c
///  @brief   Execute CTRL/T command.
///
///  @copyright 2019-2021 Franklin P. Johnston / Nowwith Treble Software
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
///  @brief    Execute CTRL/T command: read input or type character.
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
    assert(cmd != NULL);

    if (cmd->n_set)                     // n^T -> type out character
    {
        int_t m = 1;
        int_t n = cmd->n_arg;

        if (cmd->m_set)
        {
            m = cmd->m_arg;
        }

        for (int i = 0; i < m; ++i)
        {
            bool saved_image = f.et.image;

            f.et.image = cmd->colon;

            if (n >= 0)
            {
                int c = (int)n & 0xFF;

                if (c == LF && f.e3.CR_type)
                {
                    type_out(CR);
                }

                type_out(c);
            }
            else
            {
                type_out(NL);
            }

            f.et.image = saved_image;
        }
    }
    else                                // ^T -> read character from terminal
    {
        int c = getc_term((bool)!f.et.nowait);

        if (!f.et.noecho && c != -1)
        {
            type_out(c);
        }

        push_x((int_t)c, X_OPERAND);
    }
}


///
///  @brief    Scan CTRL/T command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_ctrl_T(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_neg_m(cmd->m_set, cmd->m_arg);
    require_n(cmd->m_set, cmd->n_set);
    reject_dcolon(cmd->dcolon);
    reject_atsign(cmd->atsign);

    return false;
}
