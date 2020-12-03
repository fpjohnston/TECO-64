///
///  @file    ctrl_u_cmd.c
///  @brief   Execute CTRL/U command.
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
#include "errcodes.h"
#include "exec.h"
#include "qreg.h"


///
///  @brief    Execute "^U" (CTRL/U) command: store/append string/character in
///            Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_U(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->n_set)                     // n^Uq`?
    {
        if (cmd->text1.len != 0)        // Does it have a text string?
        {
            throw(E_NAT);               // Invalid n argument and text string
        }

        if (cmd->colon)                 // n:^Uq`?
        {
            append_qchr(cmd->qindex, cmd->n_arg);
        }
        else                            // n^Uq`
        {
            store_qchr(cmd->qindex, cmd->n_arg);
        }
    }
    else                                // No n argument
    {
        if (cmd->colon)                 // :^Utext`
        {
            const char *p = cmd->text1.data;

            for (uint i = 0; i < cmd->text1.len; ++i)
            {
                append_qchr(cmd->qindex, *p++);
            }
        }
        else if (cmd->text1.len == 0)   // ^Uq`
        {
            store_qtext(cmd->qindex, NULL);
        }
        else                            // ^Uqtext`
        {
            uint len = cmd->text1.len;
            void *buf = alloc_mem(len);

            struct buffer text =
            {
                .data = buf,
                .size = len,
                .pos  = 0,
                .len  = len,
            };

            memcpy(text.data, cmd->text1.data, (ulong)text.size);
            store_qtext(cmd->qindex, &text);
        }
    }
}
