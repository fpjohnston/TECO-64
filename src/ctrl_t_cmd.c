///
///  @file    ctrl_t_cmd.c
///  @brief   Execute CTRL/T command.
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
        int_t m = cmd->m_set ? cmd->m_arg : 1;
        int_t n = cmd->n_arg;
        bool saved_image = f.et.image;

        f.et.image = cmd->colon;

        for (int i = 0; i < m; ++i)
        {
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
        }

        f.et.image = saved_image;
    }
    else                                // ^T -> read character from terminal
    {
        f.e0.ctrl_t = true;             // Flag this as CTRL/T input

        int c;

        if (f.et.nowait)
        {
            c = getc_term(NO_WAIT);
        }
        else
        {
            c = getc_term(WAIT);
        }

        f.e0.ctrl_t = false;

        if (!f.et.noecho && c != -1)
        {
            type_out(c);
        }

        store_val((int_t)c);
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

    confirm(cmd, NO_NEG_M, NO_M_ONLY, NO_DCOLON, NO_ATSIGN);

    return false;
}
