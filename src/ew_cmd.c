///
///  @file    ew_cmd.c
///  @brief   Execute EW command.
///
///  @author  Nowwith Treble Software
///
///  @bug     No known bugs.
///
///  @copyright  tbd
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "errors.h"
#include "exec.h"


///
///  @brief    Execute EW command (open file for output)
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EW(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->text1.len == 0)            // EW`?
    {
        ostream = OFILE_PRIMARY;        // Yes, switch to primary output stream

        return;
    }

    FILE *fp = ofiles[ostream].fp;

    if (fp != NULL)
    {
        print_err(E_OFO);               // Output file is already open
    }

    if (open_output(&cmd->text1, NOBACKUP_FILE) == EXIT_FAILURE)
    {
        if (!cmd->colon_set)
        {
            prints_err(E_UFO, last_file);
        }

        push_expr(OPEN_FAILURE, EXPR_VALUE);
    }
    else if (cmd->colon_set)
    {
        push_expr(OPEN_SUCCESS, EXPR_VALUE);
    }
}
