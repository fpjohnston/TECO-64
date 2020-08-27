///
///  @file    ez_cmd.c
///  @brief   Execute EZ command.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include "qreg.h"

///
///  @brief    Execute EZ command: compress text in Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EZ(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    struct qreg *qreg = get_qreg(cmd->qname, cmd->qlocal);

    assert(qreg != NULL);               // Error if no Q-register

    // Nothing to do if macro is empty

    if (qreg->text.len == 0)
    {
        return;
    }

    qreg->text.pos = 0;

    struct buffer *saved_cbuf = command;
    uint saved_base = set_expr();       // Set temporary new base

    command = &qreg->text;

    int comment = NUL;

    if (cmd->n_set)
    {
        if (isprint(cmd->n_arg) && cmd->n_arg != '!')
        {
            comment = cmd->n_arg;
        }
    }

    uint pos = command->pos;
    bool saved_dryrun = f.e0.dryrun;
    uint total = 0;

    f.e0.dryrun = true;

    while (next_cmd(cmd) != NULL)
    {
        int c = cmd->c1;
        const char *p = command->buf + pos;

        if (c != '!' || (cmd->text1.len != 0 && comment != NUL &&
                         cmd->text1.buf[0] != comment))
        {
            while (pos < command->pos)
            {
                c = *p++;

                if (c != NUL && (!isspace(c) || c == TAB))
                {
                    break;
                }

                ++pos;
            }

            uint nbytes = command->pos - pos;

            total += nbytes;

            printf("command: \"%.*s\", %u bytes\r\n", (int)nbytes,
                   command->buf + pos, nbytes);
        }

        pos = command->pos;
    }

    printf("size change from %u to %u\r\n", command->len, total);

    f.e0.dryrun = saved_dryrun;

    reset_expr(saved_base);             // Restore old base

    command = saved_cbuf;
}
