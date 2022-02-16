///
///  @file    el_cmd.c
///  @brief   Execute EL command.
///
///  @copyright 2019-2022 Franklin P. Johnston / Nowwith Treble Software
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
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "estack.h"
#include "exec.h"
#include "file.h"


///
///  @brief    Execute EL command: open or close log file.
///
///            ELfile`         - open log file for write.
///            EL`             - close log file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EL(struct cmd *cmd)
{
    assert(cmd != NULL);

    const char *name = cmd->text1.data;
    uint_t len       = cmd->text1.len;
    uint stream      = OFILE_LOG;

    close_output(stream);

    if (len == 0)
    {
        return;
    }

    assert(name != NULL);               // Error if no buffer

    if ((name = init_filename(name, len, cmd->colon)) != NULL)
    {
        if (open_output(name, stream, cmd->colon, 'L') != NULL)
        {
            if (cmd->colon)
            {
                push_x(SUCCESS, X_OPERAND);
            }

            if (cmd->n_set)
            {
                f.e3.noin  = (cmd->n_arg & 1) ? true : false;
                f.e3.noout = (cmd->n_arg & 2) ? true : false;
            }

            return;
        }
    }

    // Only here if error occurred when colon modifier specified.

    push_x(FAILURE, X_OPERAND);
}


///
///  @brief    Scan EL command.
///
///  Returns: false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_EL(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd->m_set);
    reject_dcolon(cmd->dcolon);
    scan_texts(cmd, 1, ESC);

    return false;
}
