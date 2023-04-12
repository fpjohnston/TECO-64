///
///  @file    insert_cmd.c
///  @brief   Execute TAB (CTRL/I) and I commands.
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
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "editbuf.h"
#include "eflags.h"
#include "errcodes.h"
#include "exec.h"


///
///  @brief    Execute CTRL/I command: insert text.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_I(struct cmd *cmd)
{
    assert(cmd != NULL);

    char c = TAB;

    exec_insert(&c, (uint_t)1);
    exec_insert(cmd->text1.data, cmd->text1.len);

    ++last_len;                         // Correct count for added TAB
}


///
///  @brief    Execute I command: insert text.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_I(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->n_set && cmd->text1.len != 0) // nItext`?
    {
        throw(E_IIA);                   // Invalid insert argument
    }

    if (cmd->text1.len != 0)
    {
        exec_insert(cmd->text1.data, cmd->text1.len);
    }
    else if (cmd->n_set)
    {
        char c = (char)cmd->n_arg;

        if (cmd->m_set && f.e1.insert)  // m,nI is an extended feature
        {
            if (cmd->m_arg <= 0)
            {
                return;                 // Don't insert if count is <= 0
            }
        }
        else
        {
            cmd->m_arg = 1;             // Default is to insert 1 chr.
        }

        int_t n = cmd->m_arg;

        while (n-- > 0)
        {
            exec_insert(&c, (uint_t)1);
        }

        last_len = (uint_t)cmd->m_arg;  // Adjust length of last insertion
    }
}


///
///  @brief    Insert string at dot.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_insert(const char *buf, uint_t len)
{
    assert(buf != NULL);                // Error if no buffer

    if (len == 0 || !insert_edit(buf, (size_t)len))
    {
        last_len = 0;
    }
    else
    {
        last_len = len;
    }
}


///
///  @brief    Insert newline (LF or CR/LF).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

#if     defined(DEBUG)

void insert_newline(void)
{
    if (f.e3.CR_in)
    {
        (void)insert_edit("\r", 1uL);
    }

    (void)insert_edit("\n", 1uL);
}

#endif


///
///  @brief    Scan CTRL/I command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_ctrl_I(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd->m_set);
    reject_n(cmd->n_set);
    reject_colon(cmd->colon);
    scan_texts(cmd, 1, ESC);

    return false;
}


///
///  @brief    Scan I command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_I(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_neg_m(cmd->m_set, cmd->m_arg);
    require_n(cmd->m_set, cmd->n_set);
    reject_colon(cmd->colon);

    if (!cmd->n_set || cmd->atsign || !f.e1.insert)
    {
        scan_texts(cmd, 1, ESC);
    }

    return false;
}
