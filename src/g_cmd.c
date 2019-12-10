///
///  @file    g_cmd.c
///  @brief   Execute G command.
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
#include "exec.h"


///
///  @brief    Execute G command: print Q-register contents, or copy to buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_G(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->colon_set)
    {
        if (cmd->qreg == '*')           // :G* -> print filename buffer
        {
            printf("%s", filename_buf);
            (void)fflush(stdout);
        }
        else if (cmd->qreg == '_')      // :G_ -> print search string buffer
        {
            printf("<print search string buffer here>\r\n");
            (void)fflush(stdout);
        }
        else                            // :Gq -> print Q-register
        {
            print_qreg(cmd->qreg, cmd->qlocal);
        }
    }
    else
    {
        if (cmd->qreg == '*')           // G* -> copy filename to buffer
        {
            insert_edit(filename_buf, (uint)strlen(filename_buf));
        }
        else if (cmd->qreg == '_')      // G_ -> copy search string to buffer
        {
            // TODO: add code here
        }
        else                            // Gq -> copy Q-register to buffer
        {
            // TODO: add code here
        }
    }
}

