///
///  @file    fr_cmd.c
///  @brief   Execute FR command.
///
///  @copyright 2019-2022 Franklin P. Johnston / Nowwith Treble Software
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
#include "exec.h"


///  @var    last_len
///  @brief  Length of last string searched for, or length of last insert by I,
///          tab, or G command.

uint_t last_len;


///
///  @brief    Execute FR command: delete and replace.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_FR(struct cmd *cmd)
{
    assert(cmd != NULL);

    int_t n = cmd->n_arg;
    int_t m;

    if (cmd->m_set)                     // m,nD - same as m,nK
    {
        m = cmd->m_arg;

        if (m < t->B || m > t->Z || n < t->B || n > t->Z)
        {
            throw(E_POP, "FR");         // Pointer off page
        }

        if (m > n)                      // Swap m and n if needed
        {
            m ^= n;
            n ^= m;
            m ^= n;
        }

        set_dot(m);                     // Go to first position

        n -= m;                         // And delete this many chars
    }

    delete_ebuf(n);
    exec_insert(cmd->text1.data, cmd->text1.len);
}


///
///  @brief    Scan FR command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_FR(struct cmd *cmd)
{
    assert(cmd != NULL);

    default_n(cmd, -(int_t)last_len);   // FRtext` => ^SFRtext`
    reject_neg_m(cmd->m_set, cmd->m_arg);
    reject_dcolon(cmd->dcolon);
    scan_texts(cmd, 1, ESC);

    return false;
}
