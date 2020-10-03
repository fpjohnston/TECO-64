///
///  @file    g_cmd.c
///  @brief   Execute G command.
///
///  @copyright 2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "exec.h"
#include "file.h"
#include "qreg.h"
#include "search.h"
#include "term.h"


///
///  @brief    Execute G command: print Q-register contents, or copy to buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_G(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (cmd->colon)
    {
        if (cmd->qname == '*')          // :G* -> print filename buffer
        {
            if (last_file != NULL)
            {
                print_str("%s", last_file);
            }
        }
        else if (cmd->qname == '$')     // :G$ -> print command output
        {
            const char *p = eg_result;

            while (*p != NUL)
            {
                print_chr(*p++);
            }
        }
        else if (cmd->qname == '_')     // :G_ -> print search string buffer
        {
            if (last_search.len != 0)
            {
                print_str("%.*s", (int)last_search.len, last_search.data);
            }
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
            if (last_file == NULL)
            {
                last_len = 0;
            }
            else
            {
                last_len = (uint)strlen(last_file);

                exec_insert(last_file, last_len);
            }
        }
        else if (cmd->qname == '$')     // G* -> copy EG result to buffer
        {
            if (eg_result == NULL || strlen(eg_result) == 0)
            {
                last_len = 0;
            }
            else
            {
                last_len = (uint)strlen(eg_result);

                exec_insert(eg_result, last_len);
            }
        }
        else if (cmd->qname == '_')     // G_ -> copy search string to buffer
        {
            if (last_search.data == NULL)
            {
                last_len = 0;
            }
            else
            {
                last_len = last_search.len;

                exec_insert(last_search.data, last_len);
            }
        }
        else                            // Gq -> copy Q-register to buffer
        {
            struct qreg *qreg = get_qreg(cmd->qname, cmd->qlocal);

            assert(qreg != NULL);       // Error if no Q-register

            if (qreg->text.size != 0)
            {
                last_len = qreg->text.len;

                exec_insert(qreg->text.data, last_len);
            }
        }
    }
}

