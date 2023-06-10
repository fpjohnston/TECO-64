///
///  @file    ctrl_a_cmd.c
///  @brief   Execute CTRL/A command.
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
#include <stdlib.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "term.h"


///
///  @brief    Execute CTRL/A command: type out string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_A(struct cmd *cmd)
{
    assert(cmd != NULL);
    assert(cmd->text1.data != NULL);    // Error if no edit buffer

    tstring text = build_string(cmd->text1.data, cmd->text1.len);

    const char *p = text.data;
    int last = EOF;

    for (uint_t i = 0; i < text.len; ++i)
    {
        int c = *p++;

        if (c == LF && f.e3.CR_type && last != CR)
        {
            type_out(CR);
        }

        type_out(c);

        last = c;
    }

    if (cmd->colon)
    {
        if (!f.e1.ctrl_a)
        {
            throw(E_EXT);               // Extended feature not enabled
        }

        if (f.e3.CR_type && last != CR)
        {
            type_out(CR);
        }

        type_out(LF);
    }
}


///
///  @brief    Scan CTRL/A command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_ctrl_A(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);
    confirm(cmd, NO_M, NO_N, NO_DCOLON);

    scan_texts(cmd, 1, CTRL_A);

    return false;
}
