///
///  @file    eb_cmd.c
///  @brief   Execute EB command.
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
#include <string.h>

#include "teco.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"
#include "file.h"


///
///  @brief    Execute EB command: open file for backup.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EB(struct cmd *cmd)
{
    assert(cmd != NULL);

    const char *name = cmd->text1.data;
    uint_t len = cmd->text1.len;
    struct ifile *ifile;

    if (len == 0)                       // ER`?
    {
        throw(E_NFI);                   // No file for input
    }

    assert(name != NULL);               // Error if no file name

    if ((name = init_filename(name, len, cmd->colon)) != NULL)
    {
        if ((ifile = open_input(name, istream, cmd->colon)) != NULL)
        {
            if (open_output(ifile->name, ostream, cmd->colon, 'B') != NULL)
            {
                if (cmd->colon)
                {
                    push_x(SUCCESS, X_OPERAND);
                }

                return;
            }
        }
        else if (!cmd->colon)
        {
            throw(E_FNF, name);         // File not found
        }
    }

    // Only here if error occurred when colon modifier specified.

    push_x(FAILURE, X_OPERAND);
}
