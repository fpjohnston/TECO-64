///
///  @file    eu_cmd.c
///  @brief   Execute TECO EU command.
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
#include <stdlib.h>

#include "teco.h"
#include "eflags.h"
#include "exec.h"


///
///  @brief    Execute EU command: set upper/lower case flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EU(struct cmd *cmd)
{
    assert(cmd != NULL);
    assert(cmd->n_set == true);
    
    f.eu = cmd->n_arg;
}


///
///  @brief    Scan EU command: read upper/lower case flag.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_EU(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (pop_expr(&cmd->n_arg))
    {
        cmd->n_set = true;

        scan.state = SCAN_PASS2;
    }
    else
    {
        push_expr(f.eu, EXPR_VALUE);
    }
}

