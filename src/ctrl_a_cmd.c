///
///  @file    ctrl_a_cmd.c
///  @brief   Execute CTRL/A command.
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
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "ascii.h"
#include "exec.h"
#include "term.h"


///
///  @brief    Execute "^A" (CTRL/A) command: type out string.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_A(struct cmd *cmd)
{
    assert(cmd != NULL);
    assert(cmd->text1.data != NULL);    // Error if no edit buffer

    const char *p = cmd->text1.data;

    for (unsigned int i = 0; i < cmd->text1.len; ++i)
    {
        type_out(*p++);
    }

    if (cmd->colon)
    {
        type_out(LF);
    }
}
