///
///  @file    eq_cmd.c
///  @brief   Execute EQ command.
///
///  @copyright 2019-2023 Franklin P. Johnston / Nowwith Treble Software
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

#include "teco.h"
#include "ascii.h"
#include "eflags.h"                 // Needed for confirm()
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "file.h"
#include "qreg.h"


///
///  @brief    Execute EQ command: read file into Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EQ(struct cmd *cmd)
{
    assert(cmd != NULL);

    const char *name = cmd->text1.data;
    uint_t len       = cmd->text1.len;
    uint stream      = IFILE_QREGISTER;

    if ((name = init_filename(name, len, cmd->colon)) != NULL)
    {
        struct ifile *ifile;
        tbuffer text = { .size = 0, .pos = 0, .len = 0, .data = NULL };

        if ((ifile = open_command(name, stream, cmd->colon, &text.size)) != NULL)
        {
            if (text.size == 0)
            {
                delete_qtext(cmd->qindex);
            }
            else
            {
                read_command(ifile, stream, &text);
                store_qtext(cmd->qindex, &text);
            }

            if (cmd->colon)
            {
                store_val(SUCCESS);
            }

            return;
        }
    }

    if (cmd->colon)
    {
        store_val(FAILURE);
    }
}


///
///  @brief    Scan EQ command. Also E% and FQ.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_EQ(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_M, NO_N, NO_DCOLON);

    if (!scan_qreg(cmd))
    {
        throw(E_IQN, cmd->qname);       // Invalid Q-register name
    }

    scan_texts(cmd, 1, ESC);

    return false;
}
