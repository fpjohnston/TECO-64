///
///  @file    g_cmd.c
///  @brief   Execute G command.
///
///  @copyright 2019-2021 Franklin P. Johnston / Nowwith Treble Software
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
#include "eflags.h"
#include "exec.h"
#include "file.h"
#include "qreg.h"
#include "search.h"
#include "term.h"


// Local functions

static void copy_G(struct cmd *cmd);

static void type_G(struct cmd *cmd);


///
///  @brief    Copy Q-register or special string to edit buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void copy_G(struct cmd *cmd)
{
    assert(cmd != NULL);

    const char *p;
    struct qreg *qreg;

    switch (cmd->qname)
    {
        case '*':                       // Copy filespec string
            if ((last_len = (uint_t)strlen(last_file)) != 0)
            {
                exec_insert(last_file, last_len);
            }

            break;

        case '+':                       // Copy EG result
            p = eg_result;

            if (p != NULL && (last_len = (uint_t)strlen(p)) != 0)
            {
                exec_insert(p, last_len);
            }

            break;

        case '_':                       // Copy search result
            if ((last_len = last_search.len) != 0)
            {
                exec_insert(last_search.data, last_len);
            }

            break;

        default:                        // Copy Q-register
            qreg = get_qreg(cmd->qindex);

            assert(qreg != NULL);       // Error if no Q-register

            if (qreg->text.size != 0)
            {
                last_len = qreg->text.len;

                exec_insert(qreg->text.data, last_len);
            }

            break;
    }
}


///
///  @brief    Execute "G" command: print Q-register text, or copy to buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_G(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->colon)
    {
        type_G(cmd);
    }
    else
    {
        copy_G(cmd);
    }
}


///
///  @brief    Type out Q-register or special string to edit buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void type_G(struct cmd *cmd)
{
    assert(cmd != NULL);

    const char *p;

    switch (cmd->qname)
    {
        case '*':                       // Print filespec string
            tprint("%s", last_file);

            break;

        case '+':                       // Print EG result
            if ((p = eg_result) != NULL && (last_len = (uint_t)strlen(p)) != 0)
            {
                while (*p != NUL)
                {
                    type_out(*p++);
                }
            }

            break;

        case '_':                       // Print search result
            if ((last_len = last_search.len) != 0)
            {
                tprint("%.*s", (int)last_search.len, last_search.data);
            }

            break;

        default:                        // Print Q-register
            print_qreg(cmd->qindex);

            break;
    }
}
