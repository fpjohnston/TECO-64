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
#include "eflags.h"
#include "exec.h"
#include "qreg.h"


///
///  @brief    Execute G command: print Q-register contents, or copy to buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_G(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (f.e1.noexec || f.e0.dryrun)
    {
        return;
    }

    if (cmd->colon_set)
    {
        if (cmd->qname == '*')          // :G* -> print filename buffer
        {
            print_str("%s", filename_buf);
        }
        else if (cmd->qname == '_')     // :G_ -> print search string buffer
        {
            print_str("%.*s", (int)last_search.len, last_search.buf);
        }
        else                            // :Gq -> print Q-register
        {
            print_qreg(cmd->qname, cmd->qlocal);
        }
    }
    else
    {
        if (cmd->qname == '*')          // G* -> copy filename to buffer
        {
            exec_insert(filename_buf, (uint)strlen(filename_buf));

            last_len = (uint)strlen(filename_buf);
        }
        else if (cmd->qname == '_')     // G_ -> copy search string to buffer
        {
            exec_insert(last_search.buf, last_search.len);

            last_len = last_search.len;
        }
        else                           // Gq -> copy Q-register to buffer
        {
            struct qreg *qreg = get_qreg(cmd->qname, cmd->qlocal);

            assert(qreg != NULL);

            if (qreg->text.size != 0)
            {
                exec_insert(qreg->text.buf, qreg->text.len);

                last_len = qreg->text.len;
            }
        }
    }
}

