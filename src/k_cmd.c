///
///  @file    k_cmd.c
///  @brief   Execute k command.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "edit_buf.h"
#include "errors.h"
#include "exec.h"


///
///  @brief    Execute K command: kill (delete) lines.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_K(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->h_set)                     // HK?
    {
        kill_edit();                    // Yes, kill the whole buffer

        return;
    }

    int n = 1;
    int m;
    
    if (cmd->n_set)
    {
        n = cmd->n_arg;
    }

    if (cmd->m_set)                     // m,nK
    {
        m = cmd->m_arg;

        uint z = size_edit();

        if (m < 0 || (uint)m > z || n < 0 || (uint)n > z || m > n)
        {
            print_err(E_POP);           // Pointer off page
        }

        (void)jump_edit((uint)m);       // Go to first position

        n -= m;                         // And delete this many chars
    }
    else
    {
        n = nchars_edit(n);
    }

    (void)delete_edit(n);
}

