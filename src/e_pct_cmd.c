///
///  @file    e_pct_cmd.c
///  @brief   Execute E%q command.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
///
///  Permission is hereby granted, free of charge, to any person obtaining a copy
///  of this software and associated documentation files (the "Software"), to deal
///  in the Software without restriction, including without limitation the rights
///  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
///  copies of the Software, and to permit persons to whom the Software is
///  furnished to do so, subject to the following conditions:
///
///  The above copyright notice and this permission notice shall be included in
///  all copies or substantial portions of the Software.
///
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
///  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
///
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "errors.h"
#include "exec.h"
#include "qreg.h"


///
///  @brief    Execute E%q command: write Q-register to file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_E_pct(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->text1.len == 0)            // If no file name, then done
    {
        return;
    }

    create_filename(&cmd->text1);

    if (open_output(cmd, OFILE_QREGISTER) == EXIT_FAILURE)
    {
        if (!cmd->colon_set)
        {
            prints_err(E_UFO, last_file);
        }

        push_expr(TECO_FAILURE, EXPR_VALUE);
    }
    else if (cmd->colon_set)
    {
        push_expr(TECO_SUCCESS, EXPR_VALUE);
    }

    struct ofile *ofile = &ofiles[OFILE_QREGISTER];
    struct qreg *qreg = get_qreg(cmd->qname, cmd->qlocal);
    
    assert(qreg != NULL);

    uint size = qreg->text.len;    

    if (size != 0)
    {
        if (fwrite(qreg->text.buf, 1uL, (ulong)size, ofile->fp) != size)
        {
            fatal_err(errno, E_SYS, NULL);
        }
    }

    rename_output(ofile);

    fclose(ofile->fp);

    ofile->fp = NULL;

    if (cmd->colon_set)
    {
        push_expr(TECO_SUCCESS, EXPR_VALUE);
    }
}

