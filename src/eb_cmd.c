///
///  @file    eb_cmd.c
///  @brief   Execute EB command.
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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"


///
///  @brief    Execute EB command (open file for backup)
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EB(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->text1.len == 0)
    {
        print_err(E_NFI);               // No file for input
    }

    if (open_input(&cmd->text1) == EXIT_FAILURE)
    {
        if (!cmd->got_colon || (errno != ENOENT && errno != ENODEV))
        {
            prints_err(E_FNF, last_file);
        }

        push_expr(OPEN_FAILURE, EXPR_OPERAND);
    }
    else if (cmd->got_colon)
    {
        push_expr(OPEN_SUCCESS, EXPR_OPERAND);
    }

    if (open_output(&cmd->text1, BACKUP_FILE) == EXIT_FAILURE)
    {
        if (!cmd->got_colon)
        {
            prints_err(E_UFO, last_file);
        }

        push_expr(OPEN_FAILURE, EXPR_OPERAND);
    }
    else if (cmd->got_colon)
    {
        push_expr(OPEN_SUCCESS, EXPR_OPERAND);
    }
}

