///
///  @file    yank_cmd.c
///  @brief   Execute EY and Y commands.
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
#include <string.h>

#include "teco.h"
#include "editbuf.h"
#include "eflags.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"
#include "file.h"
#include "page.h"


///
///  @brief    Execute "EY" command: yank text into buffer (no protection).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EY(struct cmd *cmd)
{
    assert(cmd != NULL);

    struct ifile *ifile = &ifiles[istream];

    if (ifile->fp == NULL)
    {
        throw(E_NFI);                   // No file for input
    }

    if (feof(ifile->fp))
    {
        if (cmd->colon)
        {
            push_x(0, X_OPERAND);

            return;
        }
    }

    if (cmd->n_set)
    {
        if (cmd->n_arg == -1)
        {
            yank_backward(ifile->fp);   // Try to yank previous page
        }
        else
        {
            throw(E_NYA);               // Numeric argument with Y
        }
    }
    else
    {
        (void)next_yank();
    }

    if (cmd->colon)
    {
        push_x(-1, X_OPERAND);
    }

    setpos_ebuf(t.B);                   // Position to start of buffer
}


///
///  @brief    Execute "Y" command: yank text into buffer (with protection).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_Y(struct cmd *cmd)
{
    // If data in buffer and yank protection is enabled, then abort.

    if (t.Z && !f.ed.yank)
    {
        throw(E_YCA);                   // Y command aborted
    }

    exec_EY(cmd);
}


///
///  @brief    Yank next page into buffer.
///
///  @returns  true if buffer has data, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool next_yank(void)
{
    kill_ebuf();

    while (append_line())               // Read what we can
    {
        ;
    }

    return (t.Z != 0) ? true : false;
}
