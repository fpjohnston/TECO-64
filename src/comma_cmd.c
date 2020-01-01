///
///  @file    comma_cmd.c
///  @brief   Process comma argument separator.
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
#include "errors.h"
#include "exec.h"


///
///  @brief    Process "," (comma argument separator).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void scan_comma(struct cmd *cmd)
{
    assert(cmd != NULL);
    
    if (scan.comma_set || cmd->h_set)   // Already seen comma or H?
    {
        print_err(E_ARG);               // Invalid arguments
    }

    if (!pop_expr(&cmd->m_arg))         // Any n argument specified?
    {
        print_err(E_NAC);               // No argument before ,
    }

    // If we've seen a comma, then what's on the expression is an "m" argument,
    // not an "n" argument (numeric arguments can take the form m,n).

    if (cmd->m_arg < 0)
    {
        print_err(E_NCA);               // Negative argument to ,
    }

    cmd->m_set = true;                  // And say we have one
    cmd->n_set = false;                 // But forget about n argument

    scan.comma_set = true;
}
